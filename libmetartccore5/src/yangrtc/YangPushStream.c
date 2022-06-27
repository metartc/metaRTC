//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtc/YangPushStream.h>
#include <yangrtc/YangRtcRtcp.h>
#include <yangrtc/YangBandwidth.h>
#include <yangrtc/YangPush.h>

#include <yangrtp/YangRtcpPsfbCommon.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangrtp/YangRtpPacket.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/yang_unistd.h>



int32_t yang_rtcpush_cache_nack(YangRtcPushStream *pub,
		YangRtpPacket *pkt, char *p, int plen) {
    if (pub == NULL||pkt==NULL||p==NULL)
		return ERROR_RTC_PUBLISH;
	int32_t err = Yang_Ok;
	uint16_t seq = pkt->header.sequence;
	if (pkt->frame_type == YangFrameTypeAudio)
		yang_pubnackbuffer_set(pub->audio_queue, seq, p, plen); //pub->audio_queue->set(seq, p,plen);
	else
		yang_pubnackbuffer_set(pub->video_queue, seq, p, plen); //pub->video_queue->set(seq,  p,plen);

	return err;
}



int32_t yang_rtcpush_on_rtcp_rr(YangRtcContext *context,
		YangRtcPushStream *pub, YangRtcpCommon *rtcp) {
	int32_t err = Yang_Ok;
	uint32_t rttPropDelay=0,rttPropDelayMsec=0;

	YangNtp ntp;
	yang_ntp_from_time_ms(&ntp,yang_get_micro_time());
	uint64_t currentTimeNTP = ntp.ntp;
	if(rtcp->rb->lsr!=0){
	     rttPropDelay = YANG_MID_NTP(currentTimeNTP) - rtcp->rb->lsr - rtcp->rb->dlsr;
	     rttPropDelayMsec = rttPropDelay*1000/65536;
	}
    context->stats.recvStats.reportsReceived++;
    if (rtcp->rb->fraction_lost > -1.0) {
    	context->stats.recvStats.fractionLost = rtcp->rb->fraction_lost;
    }
    context->stats.recvStats.rttMeasurements++;
    context->stats.recvStats.totalRtt += rttPropDelayMsec;
    context->stats.recvStats.rtt = rttPropDelayMsec;
    context->stats.sendStats.videoRtpPacketCount+=rtcp->rb->lost_packets;
	return err;
}

int32_t yang_rtcpush_on_rtcp_xr(YangRtcContext *context,
		YangRtcPushStream *pub, YangRtcpCommon *rtcp) {
	int32_t err = Yang_Ok;

	return err;
}

int32_t yang_rtcpush_check_twcc(YangRtcContext *context,YangRtcPushStream* pub){
	int32_t err = Yang_Ok;
#if Yang_Using_TWCC
	err=context->bandwidth.checkByTwcc(&context->bandwidth.session,&context->twcc.session);
#endif
	return err;
}

int32_t yang_rtcpush_check_rrt(YangRtcContext* context,YangRtcPushStream* pub){
	int32_t err = Yang_Ok;
	err=context->bandwidth.checkByRR(&context->bandwidth.session,&context->stats);
	YangBandwidthSession* bw=&context->bandwidth.session;
		YangRtcCallback *rtcCallback=&context->streamConfig->rtcCallback;
		if(rtcCallback==NULL) return 1;
		//hight lost rate
		if(bw->lostRate>kDefaultHighLossThreshold){
			if(bw->request==Yang_Req_HighLostPacketRate) return err;
			if(rtcCallback->sendRequest){
                rtcCallback->sendRequest(rtcCallback->context,context->streamConfig->uid,pub->videoSsrc,Yang_Req_HighLostPacketRate);
			}
			bw->request=Yang_Req_HighLostPacketRate;

		}else if(bw->lostRate<kDefaultLowLossThreshold){			//low lost rate
			if(bw->request==Yang_Req_LowLostPacketRate) return err;

            rtcCallback->sendRequest(rtcCallback->context,context->streamConfig->uid,pub->videoSsrc,Yang_Req_LowLostPacketRate);
            bw->request=Yang_Req_LowLostPacketRate;
		}
	return err;
}


YangSendNackBuffer* yang_rtcpush_fetch_rtp_packet(YangRtcContext *context,
		YangRtcPushStream *pub, YangPublishNackBuffer *que, uint16_t seq) {
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

int32_t yang_rtcpush_on_recv_nack(YangRtcContext *context,
		YangRtcPushStream *pub, YangPublishNackBuffer *que,
		YangRtcpNack *nack) {
	if (context == NULL || pub == NULL)
		return ERROR_RTC_PUBLISH;
	int32_t err = Yang_Ok;

	for (int32_t i = 0; i < nack->vsize; ++i) {
		uint16_t seq = nack->nacks[i];
		YangSendNackBuffer *pkt = yang_rtcpush_fetch_rtp_packet(context, pub,
				que, seq);
		if (pkt == NULL) {
			continue;
		}
		yang_trace("\nsendlostsn=%hu,", seq);
		context->stats.sendStats.nackCount++;
		if ((err = yang_send_nackpacket(context, pkt->payload, pkt->nb))!= Yang_Ok) {
			return yang_error_wrap(err, "raw send");
		}
	}

	return err;
}
int32_t yang_rtcpush_on_rtcp_nack(YangRtcContext *context,
		YangRtcPushStream *pub, YangRtcpCommon *rtcp) {
	if (context == NULL || pub == NULL||rtcp==NULL)
		return ERROR_RTC_PUBLISH;
	int32_t err = Yang_Ok;

	YangPublishNackBuffer *que = NULL;
    uint32_t ssrc = rtcp->nack->mediaSsrc;

	if (ssrc == pub->audioSsrc)		que = pub->audio_queue;
	if (ssrc == pub->videoSsrc)		que = pub->video_queue;
//	yang_trace("\npublish find lost ssrc=%hu:", ssrc);
	if (!ssrc)	return yang_error_wrap(err, "track response nack  ssrc=%u", ssrc);
	if (que
			&& (err = yang_rtcpush_on_recv_nack(context, pub, que,rtcp->nack)) != Yang_Ok) {
		return yang_error_wrap(err, "track response nack.  ssrc=%u", ssrc);
	}
	return err;
}

int32_t yang_rtcpush_do_request_keyframe(YangRtcContext *context,
		uint32_t ssrc) {
	if (context == NULL)
		return ERROR_RTC_PUBLISH;
	int32_t err = Yang_Ok;
	yang_do_request_keyframe(context, ssrc);
	return err;
}

int32_t yang_rtcpush_on_rtcp_ps_feedback(YangRtcContext *context,
		YangRtcPushStream *pub, YangRtcpCommon *rtcp) {
	if (context == NULL || pub == NULL)
		return ERROR_RTC_PUBLISH;
	int32_t err = Yang_Ok;

	uint8_t fmt = rtcp->header.rc;
	switch (fmt) {
	case kPLI: {
		uint32_t ssrc = pub->videoSsrc;
		if (ssrc) {
			yang_rtcpush_do_request_keyframe(context, ssrc);
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

int32_t yang_rtcpush_on_rtcp(YangRtcContext *context,
		YangRtcPushStream *pub, YangRtcpCommon *rtcp) {
	if (context == NULL || pub == NULL)
		return ERROR_RTC_PUBLISH;

	if (YangRtcpType_rr == rtcp->header.type) {

		return yang_rtcpush_on_rtcp_rr(context, pub, rtcp);
	} else if (YangRtcpType_rtpfb == rtcp->header.type) {
		//currently rtpfb of nack will be handle by player. TWCC will be handled by YangRtcRtcpI

		return yang_rtcpush_on_rtcp_nack(context, pub, rtcp);
	} else if (YangRtcpType_psfb == rtcp->header.type) {
		return yang_rtcpush_on_rtcp_ps_feedback(context, pub, rtcp);
	} else if (YangRtcpType_xr == rtcp->header.type) {

		return yang_rtcpush_on_rtcp_xr(context, pub, rtcp);
	} else if (YangRtcpType_bye == rtcp->header.type) {
		// TODO: FIXME: process rtcp bye.
		return Yang_Ok;
	}
	return Yang_Ok;
}
void yang_create_rtcpush(YangRtcPush *ppush, uint32_t audioSsrc,
		uint32_t videoSsrc) {
	if (ppush == NULL)		return;
	YangRtcPushStream *pub=(YangRtcPushStream*) calloc(1,sizeof(YangRtcPushStream));
	ppush->pubStream=pub;
	pub->mw_msgs = 0;
	pub->realtime = 1;
	pub->audio_queue = (YangPublishNackBuffer*) calloc(1,sizeof(YangPublishNackBuffer));
	yang_create_pubNackbuffer(pub->audio_queue, 100);

	pub->video_queue = (YangPublishNackBuffer*) calloc(1,sizeof(YangPublishNackBuffer));
	yang_create_pubNackbuffer(pub->video_queue, 1000);

	pub->audioSsrc = audioSsrc;
	pub->videoSsrc = videoSsrc;

	ppush->on_recv_nack = yang_rtcpush_on_recv_nack;
	ppush->cache_nack = yang_rtcpush_cache_nack;
	ppush->on_rtcp_nack = yang_rtcpush_on_rtcp_nack;
	ppush->on_rtcp_xr = yang_rtcpush_on_rtcp_xr;
	ppush->on_rtcp_rr = yang_rtcpush_on_rtcp_rr;
	ppush->on_rtcp = yang_rtcpush_on_rtcp;
	ppush->on_rtcp_ps_feedback = yang_rtcpush_on_rtcp_ps_feedback;
	ppush->check_twcc = yang_rtcpush_check_twcc;
	ppush->check_rrt = yang_rtcpush_check_rrt;

}
void yang_destroy_rtcpush(YangRtcPush *push) {
	if (push == NULL)	return;
	yang_destroy_pubNackbuffer(push->pubStream->audio_queue);
	yang_destroy_pubNackbuffer(push->pubStream->video_queue);
	yang_free(push->pubStream->audio_queue);
	yang_free(push->pubStream->video_queue);
	yang_free(push->pubStream);
}

