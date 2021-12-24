#include <yangwebrtc/YangRtcPlayStream.h>
#include <math.h>
#include "YangRtcSdp.h"
#include <inttypes.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangutil/sys/YangSsrc.h>
#include <yangutil/sys/YangLog.h>
using namespace std;
YangRtcPlayStream::YangRtcPlayStream(YangRtcSessionI *psession) {
	m_session = psession;
	m_request_keyframe = false;
	m_nn_simulate_nack_drop = 0;
	m_nack_enabled = true;

	m_pt_to_drop = 0;
	m_twcc_enabled = false;
	m_twcc_id = 0;
	m_twcc_fb_count = 0;
	m_last_time_send_twcc = 0;
	m_conf = NULL;
	m_rtpBuffer = NULL;
}

YangRtcPlayStream::~YangRtcPlayStream() {

	for (int32_t i = 0; i < (int) m_video_tracks.size(); ++i) {
		YangVideoRecvTrack *track = m_video_tracks.at(i);
		yang_delete(track);
	}
	m_video_tracks.clear();

	for (int32_t i = 0; i < (int) m_audio_tracks.size(); ++i) {
		YangRtcAudioRecvTrack *track = m_audio_tracks.at(i);
		yang_delete(track);
	}
	m_audio_tracks.clear();

    m_conf = NULL;
    m_session = NULL;
}

int32_t YangRtcPlayStream::initialize(YangRtcContext *conf,
		YangRtcSourceDescription *stream_desc, YangReceiveCallback *cbk,
		YangRtpBuffer *rtpBuffer) {
	int32_t err = Yang_Ok;
	m_conf = conf;
	m_rtpBuffer = rtpBuffer;
	m_mixQueue.setAudioSize(conf->context->rtc.audioQueueCount);
	m_mixQueue.setVideoSize(conf->context->rtc.videoQueueCount);
	if (stream_desc->audio_track_desc_) {
		m_audio_tracks.push_back(
				new YangRtcAudioRecvTrack(conf->streamConf->uid, conf,
						m_session, stream_desc->audio_track_desc_, rtpBuffer,
						&m_mixQueue));

	}

	for (int32_t i = 0; i < (int) stream_desc->video_track_descs_.size(); ++i) {
		YangRtcTrack *desc = stream_desc->video_track_descs_.at(i);
		m_video_tracks.push_back(
				new YangVideoRecvTrack(conf->streamConf->uid, conf,
						m_session, desc, rtpBuffer, &m_mixQueue));
	}

	int32_t twcc_id = -1;
	uint32_t media_ssrc = 0;
	// because audio_track_desc have not twcc id, for example, h5demo
	// fetch twcc_id from video track description,
	for (int32_t i = 0; i < (int) stream_desc->video_track_descs_.size(); ++i) {
		YangRtcTrack *desc = stream_desc->video_track_descs_.at(i);
		twcc_id = desc->get_rtp_extension_id(kTWCCExt);

		media_ssrc = desc->ssrc_;
		break;
	}
	if (twcc_id > 0) {
		m_twcc_id = twcc_id;
		m_extension_types.register_by_uri(m_twcc_id, kTWCCExt);
		m_rtcp_twcc.set_media_ssrc(media_ssrc);
	}

	m_nack_enabled = true;
	m_pt_to_drop = 0;
	m_twcc_enabled = true;

	// No TWCC when negotiate, disable it.
	if (twcc_id <= 0) {
		m_twcc_enabled = false;
	}

	yang_trace("RTC publisher nack=%d,  pt-drop=%u, twcc=%u/%d", m_nack_enabled,
			m_pt_to_drop, m_twcc_enabled, twcc_id);

	// Setup tracks.
	for (int32_t i = 0; i < (int) m_audio_tracks.size(); i++) {
		YangRtcAudioRecvTrack *track = m_audio_tracks.at(i);
		track->setReceiveCallback(cbk);
	}

	for (int32_t i = 0; i < (int) m_video_tracks.size(); i++) {
		YangVideoRecvTrack *track = m_video_tracks.at(i);
		track->setReceiveCallback(cbk);
	}
	return err;
}

int32_t YangRtcPlayStream::send_rtcp_rr() {
	int32_t err = Yang_Ok;

	for (int32_t i = 0; i < (int) m_video_tracks.size(); ++i) {
		YangVideoRecvTrack *track = m_video_tracks.at(i);
		if ((err = track->send_rtcp_rr()) != Yang_Ok) {
			return yang_error_wrap(err, "track=%s",
					track->get_track_id().c_str());
		}
	}

	for (int32_t i = 0; i < (int) m_audio_tracks.size(); ++i) {
		YangRtcAudioRecvTrack *track = m_audio_tracks.at(i);
		if ((err = track->send_rtcp_rr()) != Yang_Ok) {
			return yang_error_wrap(err, "track=%s",
					track->get_track_id().c_str());
		}
	}

	return err;
}

int32_t YangRtcPlayStream::send_rtcp_xr_rrtr() {
	int32_t err = Yang_Ok;

	for (int32_t i = 0; i < (int) m_video_tracks.size(); ++i) {
		YangVideoRecvTrack *track = m_video_tracks.at(i);
		if ((err = track->send_rtcp_xr_rrtr()) != Yang_Ok) {
			return yang_error_wrap(err, "track=%s",
					track->get_track_id().c_str());
		}
	}

	for (int32_t i = 0; i < (int) m_audio_tracks.size(); ++i) {
		YangRtcAudioRecvTrack *track = m_audio_tracks.at(i);
		if ((err = track->send_rtcp_xr_rrtr()) != Yang_Ok) {
			return yang_error_wrap(err, "track=%s",
					track->get_track_id().c_str());
		}
	}

	return err;
}

int32_t YangRtcPlayStream::on_twcc(uint16_t sn) {
	int32_t err = Yang_Ok;

	int64_t now = yang_get_system_time();
	err = m_rtcp_twcc.recv_packet(sn, now);

	return err;
}

int32_t YangRtcPlayStream::on_rtp(char *data, int32_t nb_data) {
	int32_t err = Yang_Ok;
	/**
	 if (m_twcc_id) {
	 // We must parse the TWCC from RTP header before SRTP unprotect, because:
	 //      1. Client may send some padding packets with invalid SequenceNumber, which causes the SRTP fail.
	 //      2. Server may send multiple duplicated NACK to client, and got more than one ARQ packet, which also fail SRTP.
	 // so, we must parse the header before SRTP unprotect(which may fail and drop packet).

	 uint16_t twcc_sn = 0;
	 if ((err = yang_rtp_fast_parse_twcc(data, nb_data, m_twcc_id, twcc_sn))== Yang_Ok) {
	 printf("twcc_%hu,",twcc_sn);
	 if((err = on_twcc(twcc_sn)) != Yang_Ok) {
	 return yang_error_wrap(err, "on twcc");
	 }
	 }
	 }
	 **/

	char *plaintext = data;
	int32_t nb_plaintext = nb_data;
	if ((err = m_session->get_srtp()->dec_rtp(plaintext, &nb_plaintext))!= Yang_Ok) {
		if(err==srtp_err_status_replay_fail) return Yang_Ok;
		 YangBuffer b(data, nb_data); YangRtpHeader h; memset(&h,0,sizeof(YangRtpHeader));h.ignore_padding=true;
		  //int r0 = h.decode(&b); // Ignore any error for header decoding.
		 yang_decode_rtpHeader(&b,&h);
		    return yang_error_wrap(err, "marker=%u, pt=%u, seq=%u, ts=%u, ssrc=%u, pad=%u, payload=%uB", h.marker, h.payload_type,
		            h.sequence, h.timestamp, h.ssrc, h.padding_length, nb_data - b.pos());
	}

	m_rtp.reset();
	m_buf.init(m_rtp.wrap(m_rtpBuffer, plaintext, nb_plaintext), nb_plaintext);

	m_rtp.set_extension_types(&m_extension_types);
	m_rtp.m_header.ignore_padding=false;

	if ((err = m_rtp.decode(&m_buf)) != Yang_Ok) {
		return yang_error_wrap(err, "decode rtp packet");
	}

	uint32_t ssrc = m_rtp.m_header.ssrc;
	YangRtcAudioRecvTrack *audio_track = get_audio_track(ssrc);
	YangVideoRecvTrack *video_track = get_video_track(ssrc);

	if (audio_track) {
		m_rtp.m_frame_type = YangFrameTypeAudio;
		if ((err = audio_track->on_rtp(&m_rtp)) != Yang_Ok) {
			return yang_error_wrap(err, "on audio");
		}
	} else if (video_track) {
		m_rtp.m_frame_type = YangFrameTypeVideo;
		if ((err = video_track->on_rtp(&m_rtp)) != Yang_Ok) {
			return yang_error_wrap(err, "on video");
		}
	} else {
		return yang_error_wrap(ERROR_RTC_RTP, "unknown ssrc=%u", ssrc);
	}

	// For NACK to handle packet.

	if (audio_track) {
		if ((err = audio_track->on_nack(&m_rtp)) != Yang_Ok) {
			return yang_error_wrap(err, "on nack");
		}
	} else if (video_track) {
		if ((err = video_track->on_nack(&m_rtp)) != Yang_Ok) {
			return yang_error_wrap(err, "on nack");
		}
	}
	return err;
}





int32_t YangRtcPlayStream::check_send_nacks() {
	int32_t err = Yang_Ok;

	//if (!m_nack_enabled) {
	//	return err;
	//}

	for (int32_t i = 0; i < (int) m_video_tracks.size(); ++i) {
		YangVideoRecvTrack *track = m_video_tracks.at(i);
		if ((err = track->check_send_nacks()) != Yang_Ok) {
			return yang_error_wrap(err, "video track=%s",
					track->get_track_id().c_str());
		}
	}

	for (int32_t i = 0; i < (int) m_audio_tracks.size(); ++i) {
		YangRtcAudioRecvTrack *track = m_audio_tracks.at(i);
		if ((err = track->check_send_nacks()) != Yang_Ok) {
			return yang_error_wrap(err, "audio track=%s",
					track->get_track_id().c_str());
		}
	}

	return err;

}
YangVideoRecvTrack* YangRtcPlayStream::get_video_track(uint32_t ssrc) {
	for (int32_t i = 0; i < (int) m_video_tracks.size(); ++i) {
		YangVideoRecvTrack *track = m_video_tracks.at(i);
		if (track->has_ssrc(ssrc)) {
			return track;
		}
	}

	return NULL;
}

YangRtcAudioRecvTrack* YangRtcPlayStream::get_audio_track(uint32_t ssrc) {
	for (int32_t i = 0; i < (int) m_audio_tracks.size(); ++i) {
		YangRtcAudioRecvTrack *track = m_audio_tracks.at(i);
		if (track->has_ssrc(ssrc)) {
			return track;
		}
	}

	return NULL;
}


int32_t YangRtcPlayStream::send_periodic_twcc() {
	int32_t err = Yang_Ok;
	m_last_time_send_twcc = yang_get_system_time();

	if (!m_rtcp_twcc.need_feedback()) {
		return err;
	}

	// limit the max count=1024 to avoid dead loop.
	for (int32_t i = 0; i < 1024 && m_rtcp_twcc.need_feedback(); ++i) {
		char pkt[kMaxUDPDataSize];
		YangBuffer *buffer = new YangBuffer(pkt, sizeof(pkt));
		YangAutoFree(YangBuffer, buffer);

		m_rtcp_twcc.set_feedback_count(m_twcc_fb_count);
		m_twcc_fb_count++;

		if ((err = m_rtcp_twcc.encode(buffer)) != Yang_Ok) {
			return yang_error_wrap(err, "encode, count=%u", m_twcc_fb_count);
		}

		if ((err = m_session->send_rtcp(pkt, buffer->pos())) != Yang_Ok) {
			return yang_error_wrap(err, "send twcc, count=%u", m_twcc_fb_count);
		}
	}
	return err;
}

int32_t YangRtcPlayStream::on_rtcp(YangRtcpCommon *rtcp) {
	if (YangRtcpType_sr == rtcp->type()) {
		YangRtcpSR *sr = dynamic_cast<YangRtcpSR*>(rtcp);
		return on_rtcp_sr(sr);
	} else if (YangRtcpType_xr == rtcp->type()) {
		YangRtcpXr *xr = dynamic_cast<YangRtcpXr*>(rtcp);
		return on_rtcp_xr(xr);
	} else if (YangRtcpType_sdes == rtcp->type()) {
		//ignore RTCP SDES
		return Yang_Ok;
	} else if (YangRtcpType_bye == rtcp->type()) {
		// TODO: FIXME: process rtcp bye.
		return Yang_Ok;
	} else {
		return yang_error_wrap(ERROR_RTC_RTCP_CHECK, "unknown rtcp type=%u",
				rtcp->type());
	}
}

int32_t YangRtcPlayStream::on_rtcp_sr(YangRtcpSR *rtcp) {
	int32_t err = Yang_Ok;
	YangNtp yang_ntp = YangNtp::to_time_ms(rtcp->get_ntp());
	//yang_trace("\nrtcp sr.....%ld",rtcp->get_ntp());
	//  yang_debug("sender report, ssrc_of_sender=%u, rtp_time=%u, sender_packet_count=%u, sender_octec_count=%u",
	//  rtcp->get_ssrc(), rtcp->get_rtp_ts(), rtcp->get_rtp_send_packets(), rtcp->get_rtp_send_bytes());

	update_send_report_time(rtcp->get_ssrc(), yang_ntp, rtcp->get_rtp_ts());

	return err;
}

int32_t YangRtcPlayStream::on_rtcp_xr(YangRtcpXr *rtcp) {
	int32_t err = Yang_Ok;

	/*
	 @see: http://www.rfc-editor.org/rfc/rfc3611.html#section-2

	 0                   1                   2                   3
	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |V=2|P|reserved |   PT=XR=207   |             length            |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |                              SSRC                             |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 :                         report blocks                         :
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 */

	YangBuffer stream(rtcp->data(), rtcp->size());
	/*uint8_t first = */stream.read_1bytes();
	uint8_t pt = stream.read_1bytes();
	if (pt != kXR)
		return yang_error_wrap(ERROR_RTC_RTCP_CHECK, "invalid XR packet");
	//srs_assert(pt == kXR);
	uint16_t length = (stream.read_2bytes() + 1) * 4;
	/*uint32_t ssrc = */stream.read_4bytes();

	if (length > rtcp->size()) {
		return yang_error_wrap(ERROR_RTC_RTCP_CHECK,
				"invalid XR packet, length=%u, nb_buf=%d", length, rtcp->size());
	}

	while (stream.pos() + 4 < length) {
		uint8_t bt = stream.read_1bytes();
		stream.skip(1);
		uint16_t block_length = (stream.read_2bytes() + 1) * 4;

		if (stream.pos() + block_length - 4 > rtcp->size()) {
			return yang_error_wrap(ERROR_RTC_RTCP_CHECK,
					"invalid XR packet block, block_length=%u, nb_buf=%d",
					block_length, rtcp->size());
		}

		if (bt == 5) {
			for (int32_t i = 4; i < block_length; i += 12) {
				uint32_t ssrc = stream.read_4bytes();
				uint32_t lrr = stream.read_4bytes();
				uint32_t dlrr = stream.read_4bytes();

				YangNtp cur_ntp = YangNtp::from_time_ms(
				yang_update_system_time() / 1000);
				uint32_t compact_ntp = (cur_ntp.m_ntp_second << 16)
						| (cur_ntp.m_ntp_fractions >> 16);

				int32_t rtt_ntp = compact_ntp - lrr - dlrr;
				int32_t rtt = ((rtt_ntp * 1000) >> 16)
						+ ((rtt_ntp >> 16) * 1000);
				yang_trace("ssrc=%u, compact_ntp=%u, lrr=%u, dlrr=%u, rtt=%d",
						ssrc, compact_ntp, lrr, dlrr, rtt);

				update_rtt(ssrc, rtt);
			}
		}
	}

	return err;
}



void YangRtcPlayStream::simulate_nack_drop(int32_t nn) {
	m_nn_simulate_nack_drop = nn;
}

void YangRtcPlayStream::simulate_drop_packet(YangRtpHeader *h,
		int32_t nn_bytes) {

	m_nn_simulate_nack_drop--;
}

void YangRtcPlayStream::update_rtt(uint32_t ssrc, int32_t rtt) {
	YangVideoRecvTrack *video_track = get_video_track(ssrc);
	if (video_track) {
		return video_track->update_rtt(rtt);
	}

	YangRtcAudioRecvTrack *audio_track = get_audio_track(ssrc);
	if (audio_track) {
		return audio_track->update_rtt(rtt);
	}
}

void YangRtcPlayStream::update_send_report_time(uint32_t ssrc,
		const YangNtp &ntp, uint32_t rtp_time) {
	YangVideoRecvTrack *video_track = get_video_track(ssrc);
	if (video_track) {
		return video_track->update_send_report_time(ntp, rtp_time);
	}

	YangRtcAudioRecvTrack *audio_track = get_audio_track(ssrc);
	if (audio_track) {
		return audio_track->update_send_report_time(ntp, rtp_time);
	}
}


