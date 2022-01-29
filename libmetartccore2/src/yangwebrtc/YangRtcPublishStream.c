#include <yangwebrtc/YangRtcPublishStream.h>
#include <yangwebrtc/YangRtcConnection.h>
#include <yangrtp/YangRtcpPsfbCommon.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangrtp/YangRtpPacket.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/yang_unistd.h>

void yang_init_rtcpublish(YangRtcPublishStream *pub, uint32_t audioSsrc,
		uint32_t videoSsrc) {
	if (pub == NULL)
		return;
	pub->mw_msgs = 0;
	pub->realtime = 1;
	pub->audio_queue = (YangPublishNackBuffer*) calloc(1,sizeof(YangPublishNackBuffer));
	yang_init_pubNackbuffer(pub->audio_queue, 100);

	pub->video_queue = (YangPublishNackBuffer*) calloc(1,sizeof(YangPublishNackBuffer));
	yang_init_pubNackbuffer(pub->video_queue, 1000);

	pub->audioSsrc = audioSsrc;
	pub->videoSsrc = videoSsrc;

}
void yang_destroy_rtcpublish(YangRtcPublishStream *pub) {
	if (pub == NULL)
		return;
	yang_destroy_pubNackbuffer(pub->audio_queue);
	yang_destroy_pubNackbuffer(pub->video_queue);
	yang_free(pub->audio_queue);
	yang_free(pub->video_queue);
}

int32_t yang_rtcpublish_cache_nack(YangRtcPublishStream *pub,
		YangRtpPacket *pkt, char *p, int plen) {
    if (pub == NULL||pkt==NULL||p==NULL)
		return 1;
	int32_t err = Yang_Ok;
	uint16_t seq = pkt->header.sequence;
	if (pkt->frame_type == YangFrameTypeAudio)
		yang_pubnackbuffer_set(pub->audio_queue, seq, p, plen); //pub->audio_queue->set(seq, p,plen);
	else
		yang_pubnackbuffer_set(pub->video_queue, seq, p, plen); //pub->video_queue->set(seq,  p,plen);

	return err;
}

int32_t yang_rtcpublish_on_rtcp(YangRtcContext *context,
		YangRtcPublishStream *pub, YangRtcpCommon *rtcp) {
	if (context == NULL || pub == NULL)
		return 1;
	yang_trace("\npublish rtcp size==%d,nb==%" PRId64, rtcp->nb_data,yang_get_rtcpCommon_nb_bytes(rtcp));

	if (YangRtcpType_rr == rtcp->header.type) {

		return yang_rtcpublish_on_rtcp_rr(context, pub, rtcp);
	} else if (YangRtcpType_rtpfb == rtcp->header.type) {
		//currently rtpfb of nack will be handle by player. TWCC will be handled by YangRtcRtcpI

		return yang_rtcpublish_on_rtcp_nack(context, pub, rtcp);
	} else if (YangRtcpType_psfb == rtcp->header.type) {
		return yang_rtcpublish_on_rtcp_ps_feedback(context, pub, rtcp);
	} else if (YangRtcpType_xr == rtcp->header.type) {

		return yang_rtcpublish_on_rtcp_xr(context, pub, rtcp);
	} else if (YangRtcpType_bye == rtcp->header.type) {
		// TODO: FIXME: process rtcp bye.
		return Yang_Ok;
	} else {
		return yang_error_wrap(ERROR_RTC_RTCP_CHECK, "unknown rtcp type=%u",
				rtcp->header.type);
	}
}

int32_t yang_rtcpublish_on_rtcp_rr(YangRtcContext *context,
		YangRtcPublishStream *pub, YangRtcpCommon *rtcp) {
	int32_t err = Yang_Ok;

	// TODO: FIXME: Implements it.

	return err;
}

int32_t yang_rtcpublish_on_rtcp_xr(YangRtcContext *context,
		YangRtcPublishStream *pub, YangRtcpCommon *rtcp) {
	int32_t err = Yang_Ok;

	// TODO: FIXME: Implements it.

	return err;
}

int32_t yang_rtcpublish_on_rtcp_nack(YangRtcContext *context,
		YangRtcPublishStream *pub, YangRtcpCommon *rtcp) {
	if (context == NULL || pub == NULL)
		return 1;
	int32_t err = Yang_Ok;

	YangPublishNackBuffer *que = NULL;
	uint32_t ssrc = rtcp->nack->media_ssrc;

	if (ssrc == pub->audioSsrc)		que = pub->audio_queue;
	if (ssrc == pub->videoSsrc)		que = pub->video_queue;
	yang_trace("\npublish find lost ssrc=%hu:", ssrc);
	if (!ssrc)
		return yang_error_wrap(err, "track response nack  ssrc=%u", ssrc);;
	if (que
			&& (err = yang_rtcpublish_on_recv_nack(context, pub, que,
					rtcp->nack)) != Yang_Ok) {
		return yang_error_wrap(err, "track response nack.  ssrc=%u", ssrc);
	}
	return err;
}

YangSendNackBuffer* yang_rtcpublish_fetch_rtp_packet(YangRtcContext *context,
		YangRtcPublishStream *pub, YangPublishNackBuffer *que, uint16_t seq) {
	if (context == NULL || pub == NULL)
		return NULL;
	YangSendNackBuffer *pkt = yang_pubnackbuffer_at(que, seq);	//que->at(seq);

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

int32_t yang_rtcpublish_on_recv_nack(YangRtcContext *context,
		YangRtcPublishStream *pub, YangPublishNackBuffer *que,
		YangRtcpNack *nack) {
	if (context == NULL || pub == NULL)
		return 1;
	int32_t err = Yang_Ok;

	for (int32_t i = 0; i < nack->vlen; ++i) {
		uint16_t seq = nack->nacks[i];
		YangSendNackBuffer *pkt = yang_rtcpublish_fetch_rtp_packet(context, pub,
				que, seq);
		if (pkt == NULL) {
			continue;
		}
		yang_trace("\nsendlostsn=%hu,", seq);


		if ((err = yang_send_nackpacket(context, pkt->payload, pkt->nb))
				!= Yang_Ok) {
			return yang_error_wrap(err, "raw send");
		}
	}

	return err;
}

int32_t yang_rtcpublish_do_request_keyframe(YangRtcContext *context,
		uint32_t ssrc) {
	if (context == NULL)
		return 1;
	int32_t err = Yang_Ok;
	yang_do_request_keyframe(context, ssrc);
	return err;
}

int32_t yang_rtcpublish_on_rtcp_ps_feedback(YangRtcContext *context,
		YangRtcPublishStream *pub, YangRtcpCommon *rtcp) {
	if (context == NULL || pub == NULL)
		return 1;
	int32_t err = Yang_Ok;

	uint8_t fmt = rtcp->header.rc;
	switch (fmt) {
	case kPLI: {
		uint32_t ssrc = pub->videoSsrc;
		if (ssrc) {
			yang_rtcpublish_do_request_keyframe(context, ssrc);
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

