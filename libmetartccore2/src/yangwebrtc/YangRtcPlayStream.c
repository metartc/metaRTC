//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangwebrtc/YangRtcPlayStream.h>
#include <yangwebrtc/YangRtcAudioRecvTrack.h>
#include <yangwebrtc/YangVideoRecvTrack.h>

#include <yangrtp/YangRtcp.h>
#include <yangrtp/YangRtcpSR.h>
#include <yangrtp/YangRtcpTWCC.h>
#include <yangrtp/YangRtcpXr.h>
#include <yangrtp/YangRtp.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangrtp/YangRtpPacket.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangutil/sys/YangSsrc.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangSRtp.h>

#include <yangrtp/YangRtp.h>
#include <yangutil/buffer/YangBuffer.h>

#include <yangutil/sys/YangSsrc.h>


#include <math.h>

int32_t yang_rtcplay_on_twcc(YangRtcContext *context, YangRtcPlayStream *play,uint16_t sn);
void yang_init_rtcplay(YangRtcContext *context, YangRtcPlayStream *play,
		YangRtpBuffer *rtpBuffer) {
	if (context == NULL || play == NULL)		return;
	play->request_keyframe = false;
	play->nn_simulate_nack_drop = 0;
	play->twcc_enabled = false;
	play->twcc_id = 0;
	play->twcc_fb_count = 0;
	play->last_time_send_twcc = 0;

	play->rtpBuffer = NULL;
	play->audio_track = NULL;
	play->video_track = NULL;
	memset(&play->rtp, 0, sizeof(YangRtpPacket));
	yang_init_rtpPacket(&play->rtp);

	play->rtpBuffer = rtpBuffer;

	play->audio_track = (YangRecvTrack*) calloc(1, sizeof(YangRecvTrack));
	yang_init_recvaudioTrack(context, play->audio_track, rtpBuffer);
	play->video_track = (YangVideoRecvTrack*) calloc(1,	sizeof(YangVideoRecvTrack));
	yang_init_recvvideoTrack(context, play->video_track, rtpBuffer);

	play->twcc_enabled = false;

#if Yang_Using_TWCC
	int32_t twcc_id = context->twcc_id;
		if (twcc_id > 0) {
			play->twcc_id = twcc_id;
	#if Yang_Using_RtpExtension
			play->extension_types.register_by_uri(play->twcc_id, kTWCCExt);
	#endif
			play->rtcp_twcc.set_media_ssrc(media_ssrc);
		}
		play->twcc_enabled = true;

		// No TWCC when negotiate, disable it.
		if (twcc_id <= 0) {
			play->twcc_enabled = false;
		}
#endif


}
void yang_destroy_rtcplay(YangRtcPlayStream *play) {
	if (play == NULL)	return;
	yang_destroy_recvaudioTrack(play->audio_track);
	yang_destroy_recvvideoTrack(play->video_track);
	yang_free(play->audio_track);
	yang_free(play->video_track);
	yang_destroy_rtpPacket(&play->rtp);
}

int32_t yang_rtcplay_send_rtcp_rr(YangRtcContext *context,
		YangRtcPlayStream *play) {
	if (context == NULL || play == NULL)
		return 1;
	int32_t err = Yang_Ok;


	if ((err = yang_recvtrack_send_rtcp_rr(context, &play->video_track->recv))
			!= Yang_Ok) {
		return yang_error_wrap(err, "track=%u", play->video_track->recv.ssrc_);
	}
	//}


	if ((err = yang_recvtrack_send_rtcp_rr(context, play->audio_track))
			!= Yang_Ok) {
		return yang_error_wrap(err, "track=%u", play->audio_track->ssrc_);
	}


	return err;
}

int32_t yang_rtcplay_send_rtcp_xr_rrtr(YangRtcContext *context,
		YangRtcPlayStream *play) {
	if (context == NULL || play == NULL)
		return 1;
	int32_t err = Yang_Ok;


	if ((err = yang_recvtrack_send_rtcp_xr_rrtr(context,
			&play->video_track->recv)) != Yang_Ok) {
		return yang_error_wrap(err, "track=%u", play->video_track->recv.ssrc_);
	}

	if ((err = yang_recvtrack_send_rtcp_xr_rrtr(context, play->audio_track))
			!= Yang_Ok) {
		return yang_error_wrap(err, "track=%u", play->audio_track->ssrc_);
	}


	return err;
}

int32_t yang_rtcplay_on_twcc(YangRtcContext *context, YangRtcPlayStream *play,
		uint16_t sn) {
	if (context == NULL || play == NULL)
		return 1;
	int32_t err = Yang_Ok;
#if Yang_Using_TWCC
	int64_t now = yang_get_systeplay->time();
	err = play->rtcp_twcc.recv_packet(sn, now);
#endif

	return err;
}

int32_t yang_rtcplay_on_rtp(YangRtcContext *context, YangRtcPlayStream *play,
		char *data, int32_t nb_data) {
	if (context == NULL || play == NULL)
		return 1;
	int32_t err = Yang_Ok;

	 if (play->twcc_enabled) {
	 // We must parse the TWCC from RTP header before SRTP unprotect, because:
	 //      1. Client may send some padding packets with invalid SequenceNumber, which causes the SRTP fail.
	 //      2. Server may send multiple duplicated NACK to client, and got more than one ARQ packet, which also fail SRTP.
	 // so, we must parse the header before SRTP unprotect(which may fail and drop packet).

		 uint16_t twcc_sn = 0;
		 if ((err = yang_rtp_fast_parse_twcc(data, nb_data, play->twcc_id, &twcc_sn))== Yang_Ok) {
			 if((err = yang_rtcplay_on_twcc(context,play,twcc_sn)) != Yang_Ok) {
				 return yang_error_wrap(err, "on twcc");
			 }
		 }
	 }


	char *plaintext = data;
	int32_t nb_plaintext = nb_data;
	if ((err = yang_dec_rtp(&context->srtp, plaintext, &nb_plaintext))
			!= Yang_Ok) {
		if (err == srtp_err_status_replay_fail)
			return Yang_Ok;
		YangBuffer b;
		yang_init_buffer(&b, data, nb_data);
		YangRtpHeader h;
		memset(&h, 0, sizeof(YangRtpHeader));
		h.ignore_padding = true;
		// Ignore any error for header decoding.
		yang_decode_rtpHeader(&b, &h);
		return yang_error_wrap(err,
				"marker=%u, pt=%u, seq=%u, ts=%u, ssrc=%u, pad=%u, payload=%uB",
				h.marker, h.payload_type, h.sequence, h.timestamp, h.ssrc,
				h.padding_length, nb_data - yang_buffer_pos(&b));
	}

	yang_reset_rtpPacket(&play->rtp);
	yang_init_buffer(&play->buf,
			yang_wrap_rtpPacket(&play->rtp, play->rtpBuffer, plaintext,
					nb_plaintext), nb_plaintext);
#if Yang_Using_RtpExtension
	play->rtp.set_extension_types(&play->extension_types);
#endif
	play->rtp.header.ignore_padding = false;

	if ((err = yang_decode_rtpPacket(&play->rtp, &play->buf)) != Yang_Ok) {
		return yang_error_wrap(err, "decode rtp packet");
	}

	uint32_t ssrc = play->rtp.header.ssrc;

	YangRecvTrack *audio_track = yang_rtcplay_get_audio_track(context, play,ssrc);
	YangVideoRecvTrack *video_track = yang_rtcplay_get_video_track(context,	play, ssrc);

	if (audio_track) {
		play->rtp.frame_type = YangFrameTypeAudio;
		if ((err = yang_recvaudioTrack_on_rtp(context, &play->rtp)) != Yang_Ok) {
			return yang_error_wrap(err, "on audio");
		}
	} else if (video_track) {
		play->rtp.frame_type = YangFrameTypeVideo;
		if ((err = yang_recvvideoTrack_on_rtp(context, video_track, &play->rtp))
				!= Yang_Ok) {
			return yang_error_wrap(err, "on video");
		}
	} else {
		return yang_error_wrap(ERROR_RTC_RTP, "unknown ssrc=%u", ssrc);
	}

	// For NACK to handle packet.

	if (audio_track) {
		if ((err = yang_recvtrack_on_nack(context, audio_track, &play->rtp))
				!= Yang_Ok) {
			return yang_error_wrap(err, "on nack");
		}
	} else if (video_track) {
		if ((err = yang_recvtrack_on_nack(context, &video_track->recv,
				&play->rtp)) != Yang_Ok) {
			return yang_error_wrap(err, "on nack");
		}
	}
	return err;
}

int32_t yang_rtcplay_check_send_nacks(YangRtcContext *context,
		YangRtcPlayStream *play) {
	if (context == NULL || play == NULL)
		return 1;
	int32_t err = Yang_Ok;


	if ((err = yang_recvvideoTrack_check_send_nacks(context, play->video_track))
			!= Yang_Ok) {
		return yang_error_wrap(err, "video track=%d",
				play->video_track->recv.ssrc_);
	}

	if ((err = yang_recvaudioTrack_check_send_nacks(context, play->audio_track))
			!= Yang_Ok) {
		return yang_error_wrap(err, "audio track=%d", play->audio_track->ssrc_);
	}


	return err;

}
YangVideoRecvTrack* yang_rtcplay_get_video_track(YangRtcContext *context,
		YangRtcPlayStream *play, uint32_t ssrc) {
	if (context == NULL || play == NULL)
		return NULL;

	if (yang_recvtrack_has_ssrc(context, &play->video_track->recv, ssrc)) {
		return play->video_track;
	}


	return NULL;
}

YangRecvTrack* yang_rtcplay_get_audio_track(YangRtcContext *context,
		YangRtcPlayStream *play, uint32_t ssrc) {
	if (context == NULL || play == NULL)
		return NULL;
	//for (int32_t i = 0; i < (int) play->audio_tracks.size(); ++i) {
	//	YangRtcAudioRecvTrack *track = play->audio_tracks.at(i);
	if (yang_recvtrack_has_ssrc(context, play->audio_track, ssrc)) {
		return play->audio_track;
	}
	//}

	return NULL;
}

int32_t yang_rtcplay_send_periodic_twcc(YangRtcContext *context,
		YangRtcPlayStream *play) {
	if (context == NULL || play == NULL)
		return 1;
	int32_t err = Yang_Ok;
#if Yang_Using_TWCC

	play->last_time_send_twcc = yang_get_systeplay->time();

	if (!play->rtcp_twcc.need_feedback()) {
		return err;
	}

	// limit the max count=1024 to avoid dead loop.
	for (int32_t i = 0; i < 1024 && play->rtcp_twcc.need_feedback(); ++i) {
		char pkt[kMaxUDPDataSize];
		YangBuffer buffer ;
		yang_init_buffer(&buffer,pkt, sizeof(pkt));
		//YangAutoFree(YangBuffer, buffer);

		play->rtcp_twcc.set_feedback_count(play->twcc_fb_count);
		play->twcc_fb_count++;

		if ((err = play->rtcp_twcc.encode(&buffer)) != Yang_Ok) {
			return yang_error_wrap(err, "encode, count=%u", play->twcc_fb_count);
		}

		if ((err = play->session->send_rtcp(pkt, yang_buffer_pos(&buffer))) != Yang_Ok) {
			return yang_error_wrap(err, "send twcc, count=%u", play->twcc_fb_count);
		}

	}
#endif
	return err;
}

int32_t yang_rtcplay_on_rtcp(YangRtcContext *context, YangRtcPlayStream *play,
		YangRtcpCommon *rtcp) {
	if (context == NULL || play == NULL)
		return 1;
	if (YangRtcpType_sr == rtcp->header.type) {
		//YangRtcpSR *sr = dynamic_cast<YangRtcpSR*>(rtcp);
		return yang_rtcplay_on_rtcp_sr(context, play, rtcp);
	} else if (YangRtcpType_xr == rtcp->header.type) {
		//YangRtcpXr *xr = dynamic_cast<YangRtcpXr*>(rtcp);
		return yang_rtcplay_on_rtcp_xr(context, play, rtcp);
	} else if (YangRtcpType_sdes == rtcp->header.type) {
		//ignore RTCP SDES
		return Yang_Ok;
	} else if (YangRtcpType_bye == rtcp->header.type) {
		// TODO: FIXME: process rtcp bye.
		return Yang_Ok;
	} else {
		return yang_error_wrap(ERROR_RTC_RTCP_CHECK, "unknown rtcp type=%u",
				rtcp->header.type);
	}
}

int32_t yang_rtcplay_on_rtcp_sr(YangRtcContext *context,
		YangRtcPlayStream *play, YangRtcpCommon *rtcp) {
	if (context == NULL || play == NULL)
		return 1;
	int32_t err = Yang_Ok;
	YangNtp yang_ntp;
	yang_ntp_to_time_ms(&yang_ntp, rtcp->sr->ntp);
	yang_rtcplay_update_send_report_time(context, play, rtcp->ssrc, &yang_ntp,
			rtcp->sr->rtp_ts);

	return err;
}

int32_t yang_rtcplay_on_rtcp_xr(YangRtcContext *context,
		YangRtcPlayStream *play, YangRtcpCommon *rtcp) {
	if (context == NULL || play == NULL)
		return 1;
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

	YangBuffer stream;
	yang_init_buffer(&stream, rtcp->data, rtcp->nb_data);
	//uint8_t first =
	yang_read_1bytes(&stream);
	uint8_t pt = yang_read_1bytes(&stream);
	if (pt != kXR)
		return yang_error_wrap(ERROR_RTC_RTCP_CHECK, "invalid XR packet");

	uint16_t length = (yang_read_2bytes(&stream) + 1) * 4;
	//uint32_t ssrc =
	yang_read_4bytes(&stream);

	if (length > rtcp->nb_data) {
		return yang_error_wrap(ERROR_RTC_RTCP_CHECK,
				"invalid XR packet, length=%u, nb_buf=%d", length,
				rtcp->nb_data);
	}

	while (yang_buffer_pos(&stream) + 4 < length) {
		uint8_t bt = yang_read_1bytes(&stream);
		yang_buffer_skip(&stream, 1);
		uint16_t block_length = (yang_read_2bytes(&stream) + 1) * 4;

		if (yang_buffer_pos(&stream) + block_length - 4 > rtcp->nb_data) {
			return yang_error_wrap(ERROR_RTC_RTCP_CHECK,
					"invalid XR packet block, block_length=%u, nb_buf=%d",
					block_length, rtcp->nb_data);
		}

		if (bt == 5) {
			for (int32_t i = 4; i < block_length; i += 12) {
				uint32_t ssrc = yang_read_4bytes(&stream);
				uint32_t lrr = yang_read_4bytes(&stream);
				uint32_t dlrr = yang_read_4bytes(&stream);

				YangNtp cur_ntp;
				yang_ntp_from_time_ms(&cur_ntp,
				yang_update_system_time() / 1000);
				uint32_t compact_ntp = (cur_ntp.ntp_second << 16)
						| (cur_ntp.ntp_fractions >> 16);

				int32_t rtt_ntp = compact_ntp - lrr - dlrr;
				int32_t rtt = ((rtt_ntp * 1000) >> 16)
						+ ((rtt_ntp >> 16) * 1000);
				yang_trace("ssrc=%u, compact_ntp=%u, lrr=%u, dlrr=%u, rtt=%d",
						ssrc, compact_ntp, lrr, dlrr, rtt);

				yang_rtcplay_update_rtt(context, play, ssrc, rtt);
			}
		}
	}

	return err;
}

void yang_rtcplay_simulate_nack_drop(YangRtcContext *context,
		YangRtcPlayStream *play, int32_t nn) {
	if (context == NULL || play == NULL)
		return;
	play->nn_simulate_nack_drop = nn;
}

void yang_rtcplay_simulate_drop_packet(YangRtcContext *context,
		YangRtcPlayStream *play, YangRtpHeader *h, int32_t nn_bytes) {
	if (context == NULL || play == NULL)
		return;
	play->nn_simulate_nack_drop--;
}

void yang_rtcplay_update_rtt(YangRtcContext *context, YangRtcPlayStream *play,
		uint32_t ssrc, int32_t rtt) {
	if (context == NULL || play == NULL)
		return;
	YangVideoRecvTrack *video_track = yang_rtcplay_get_video_track(context,
			play, ssrc);
	if (video_track) {
		return yang_recvtrack_update_rtt(&video_track->recv, rtt);
	}

	YangRecvTrack *audio_track = yang_rtcplay_get_audio_track(context, play,
			ssrc);
	if (audio_track) {
		return yang_recvtrack_update_rtt(audio_track, rtt);
	}
}

void yang_rtcplay_update_send_report_time(YangRtcContext *context,
		YangRtcPlayStream *play, uint32_t ssrc, YangNtp *ntp, uint32_t rtp_time) {
	if (context == NULL || play == NULL)
		return;
	YangVideoRecvTrack *video_track = yang_rtcplay_get_video_track(context,
			play, ssrc);
	if (video_track) {

		yang_recvtrack_update_send_report_time(&video_track->recv, ntp,
				rtp_time);
	}

	YangRecvTrack *audio_track = yang_rtcplay_get_audio_track(context, play,
			ssrc);
	if (audio_track) {
		yang_recvtrack_update_send_report_time(audio_track, ntp, rtp_time);

	}
}

