#include <yangwebrtc/YangRtcPublishStream.h>
#include <yangrtp/YangRtpPacket.h>
#include <yangrtp/YangRtcpPsfbCommon.h>
#include <yangrtp/YangRtpRingBuffer.h>

#include <yangrtp/YangRtpConstant.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/yang_unistd.h>
#include <sstream>
#include "YangRtcSdp.h"
YangRtcPublishStream::YangRtcPublishStream(YangRtcSessionI *s) {
	m_session = s;
	m_mw_msgs = 0;
	m_realtime = true;
	m_nack_enabled = false;

	m_audio_queue = NULL;
	m_video_queue = NULL;
	m_audioSsrc = 0;
	m_videoSsrc = 0;

}

YangRtcPublishStream::~YangRtcPublishStream() {
	yang_delete(m_audio_queue);
	yang_delete(m_video_queue);
}

int32_t YangRtcPublishStream::init(uint32_t audioSsrc, uint32_t videoSsrc) {
	int32_t err = Yang_Ok;
	if (m_audio_queue == NULL)
		m_audio_queue = new YangPublishNackBuffer(100);
	if (m_video_queue == NULL)
		m_video_queue = new YangPublishNackBuffer(1000);
	m_audioSsrc = audioSsrc;
	m_videoSsrc = videoSsrc;
	m_nack_enabled = true;

	return err;
}

int32_t YangRtcPublishStream::cache_nack(YangRtpPacket *pkt,char* p,int plen) {
	int32_t err = Yang_Ok;
	uint16_t seq = pkt->m_header.sequence;
	if (pkt->is_audio())
		m_audio_queue->set(seq, p,plen);
	else
		m_video_queue->set(seq,  p,plen);

	return err;
}

int32_t YangRtcPublishStream::on_rtcp(YangRtcpCommon *rtcp) {
	yang_trace("\npublish rtcp size==%d,nb==%" PRId64,rtcp->size(),rtcp->nb_bytes());
	if (YangRtcpType_rr == rtcp->type()) {
		YangRtcpRR *rr = dynamic_cast<YangRtcpRR*>(rtcp);
		return on_rtcp_rr(rr);
	} else if (YangRtcpType_rtpfb == rtcp->type()) {
		//currently rtpfb of nack will be handle by player. TWCC will be handled by YangRtcRtcpI
		YangRtcpNack *nack = dynamic_cast<YangRtcpNack*>(rtcp);
		return on_rtcp_nack(nack);
	} else if (YangRtcpType_psfb == rtcp->type()) {
		YangRtcpPsfbCommon *psfb = dynamic_cast<YangRtcpPsfbCommon*>(rtcp);
		return on_rtcp_ps_feedback(psfb);
	} else if (YangRtcpType_xr == rtcp->type()) {
		YangRtcpXr *xr = dynamic_cast<YangRtcpXr*>(rtcp);
		return on_rtcp_xr(xr);
	} else if (YangRtcpType_bye == rtcp->type()) {
		// TODO: FIXME: process rtcp bye.
		return Yang_Ok;
	} else {
		return yang_error_wrap(ERROR_RTC_RTCP_CHECK, "unknown rtcp type=%u",
				rtcp->type());
	}
}

int32_t YangRtcPublishStream::on_rtcp_rr(YangRtcpRR *rtcp) {
	int32_t err = Yang_Ok;

	// TODO: FIXME: Implements it.

	return err;
}

int32_t YangRtcPublishStream::on_rtcp_xr(YangRtcpXr *rtcp) {
	int32_t err = Yang_Ok;

	// TODO: FIXME: Implements it.

	return err;
}

int32_t YangRtcPublishStream::on_rtcp_nack(YangRtcpNack *rtcp) {
	int32_t err = Yang_Ok;
	YangPublishNackBuffer *que = NULL;
	uint32_t ssrc = rtcp->get_media_ssrc();
	vector<uint16_t> seqs = rtcp->get_lost_sns();
	if (ssrc == m_audioSsrc)
		que = m_audio_queue;
	if (ssrc == m_videoSsrc)
		que = m_video_queue;
    yang_trace("\npublish find lost ssrc=%hu:",ssrc);
	if (!ssrc)
		return yang_error_wrap(err, "track response nack  ssrc=%u", ssrc);;
	if (que&&(err = on_recv_nack(que, seqs)) != Yang_Ok) {
		return yang_error_wrap(err, "track response nack.  ssrc=%u", ssrc);
	}
	return err;
}

int32_t YangRtcPublishStream::on_recv_nack(YangPublishNackBuffer *que,
		const vector<uint16_t> &lost_seqs) {
	int32_t err = Yang_Ok;

	for (int32_t i = 0; i < (int) lost_seqs.size(); ++i) {
		uint16_t seq = lost_seqs.at(i);
		YangPublishBuffer *pkt = fetch_rtp_packet(que, seq);
		if (pkt == NULL) {
			continue;
		}
        yang_trace("\nsendlostsn=%hu,",seq);
		if ((err = m_session->send_nackpacket(pkt->payload,pkt->nb)) != Yang_Ok) {
			return yang_error_wrap(err, "raw send");
		}
	}

	return err;
}

YangPublishBuffer* YangRtcPublishStream::fetch_rtp_packet(YangPublishNackBuffer *que,
		uint16_t seq) {
	YangPublishBuffer *pkt = que->at(seq);

	if (pkt == NULL) {
		return pkt;
	}

	// For NACK, it sequence must match exactly, or it cause SRTP fail.
	// Return packet only when sequence is equal.
	if (pkt->seq == seq) {
		return pkt;
	}
	return NULL;
}
int32_t YangRtcPublishStream::on_rtcp_ps_feedback(YangRtcpPsfbCommon *rtcp) {
	int32_t err = Yang_Ok;

	uint8_t fmt = rtcp->get_rc();
	switch (fmt) {
	case kPLI: {
		uint32_t ssrc = m_videoSsrc; //get_video_publish_ssrc(rtcp->get_media_ssrc());
		if (ssrc) {
			do_request_keyframe(ssrc);
		}
		break;
	}
	case kSLI: {
		yang_info("sli");
		break;
	}
	case kRPSI: {
		yang_info("rpsi");
		break;
	}
	case kAFB: {
		yang_info("afb");
		break;
	}
	default: {
		return yang_error_wrap(ERROR_RTC_RTCP,
				"unknown payload specific feedback=%u", fmt);
	}
	}

	return err;
}


int32_t YangRtcPublishStream::do_request_keyframe(uint32_t ssrc) {
	int32_t err = Yang_Ok;

	m_session->do_request_keyframe(ssrc);

	return err;
}

