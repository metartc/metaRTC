//
// Copyright (c) 2019-2026 yanggaofeng
//
#include <yangrtc/YangPushStream.h>

#include <yangrtc/YangPush.h>
#include <yangrtc/YangRtcRtcp.h>
#include <yangrtc/YangBandwidth.h>

#include <yangrtc/YangPushAudio.h>
#include <yangrtc/YangPushVideo.h>

#include <yangrtp/YangRtpPacket.h>
#include <yangutil/yangrtptype.h>
#include <yangutil/yangrtptype.h>
#include <yangrtp/YangRtcpPsfbCommon.h>

#include <yangutil/sys/YangLog.h>

static int32_t yang_rtcpush_on_rtcp_rr(YangRtcContext *context,
		YangPushStream *pub, YangRtcpCommon *rtcp) {
	int32_t err = Yang_Ok;
	if(rtcp->ssrc==pub->audioSsrc)
		return context->stats.on_recvRR(yangtrue,&context->stats.recvStats,&context->stats.sendStats,rtcp);

	if(rtcp->ssrc==pub->videoSsrc)
		return context->stats.on_recvRR(yangfalse,&context->stats.recvStats,&context->stats.sendStats,rtcp);
	return err;
}

static int32_t yang_rtcpush_on_rtcp_xr(YangRtcContext *context,
		YangPushStream *pub, YangRtcpCommon *rtcp) {
	int32_t err = Yang_Ok;

	return err;
}

static int32_t yang_rtcpush_check_twcc(YangRtcContext *context,YangPushStream* pub){
	int32_t err = Yang_Ok;
#if Yang_Enable_TWCC
	err=context->bandwidth.checkByTwcc(&context->bandwidth.session,&context->twcc.session);
#endif
	return err;
}

static int32_t yang_check_bindwidth(YangRtcContext* context,YangPushStream* pub){

	return context->bandwidth.checkBandWidth(&context->bandwidth.session,&context->stats,context->peerInfo,context->peerCallback,pub->videoSsrc);
}


static int32_t yang_on_recv_nack(YangRtcContext *context,
		YangPushStream *pub,yangbool isVideo,YangRtcpNack *nack) {

	uint16_t seq = 0;
	uint32_t ssrc,i;
	int32_t err = Yang_Ok;

	uint8_t* buffer;
	YangPacket *pktData;
	YangPacketBuffer* que;

	if (context == NULL || pub == NULL)
		return ERROR_RTC_PUBLISH;

	que=isVideo?pub->video_queue:pub->audio_queue;
	ssrc=isVideo?pub->videoSsrc:pub->audioSsrc;

	buffer=pub->nackBuffer;

	if(que==NULL)
		return 1;

	if(nack->vsize==0)
		return Yang_Ok;


	for (i = 0; i < nack->vsize; ++i) {
		seq = nack->nacks[i];

		pktData = yang_pkt_getPacketBySeq(que,seq);
		if (pktData == NULL) {
			continue;
		}

		yang_memcpy(buffer,pktData->payload,pktData->length);

		if(isVideo)
			yang_push_videoPacket(context,ssrc,buffer,pktData->length);
		else
			yang_push_audioPacket(context,ssrc,buffer);

		context->stats.sendStats.nackCount++;
		if ((err = yang_send_nackpacket(context, (char*)buffer, pktData->length))!= Yang_Ok) {
			return yang_error_wrap(err, "raw send");
		}
	}

	return err;
}

static int32_t yang_rtcpush_on_rtcp_nack(YangRtcContext *context,
		YangPushStream *pub, YangRtcpCommon *rtcp) {

	int32_t err = Yang_Ok;

		yangbool isVideo=yangtrue;
	    uint32_t ssrc = rtcp->nack->mediaSsrc;

		if (context == NULL || pub == NULL||rtcp==NULL)
			return ERROR_RTC_PUBLISH;


	#if Yang_Enable_RTC_Audio
		if (ssrc == pub->audioSsrc)		isVideo = yangfalse;
	#endif
	#if Yang_Enable_RTC_Video
		if (ssrc == pub->videoSsrc)		isVideo = yangtrue;
	#endif
//	yang_trace("\npublish find lost ssrc=%hu:", ssrc);
	if (!ssrc)	return yang_error_wrap(err, "track response nack  ssrc=%u", ssrc);
	if ((err = yang_on_recv_nack(context, pub, isVideo,rtcp->nack)) != Yang_Ok) {
		return yang_error_wrap(err, "track response nack.  ssrc=%u", ssrc);
	}
	return err;
}

static int32_t yang_rtcpush_do_request_keyframe(YangRtcContext *context,
		uint32_t ssrc) {
	int32_t err = Yang_Ok;

	if (context == NULL)
		return ERROR_RTC_PUBLISH;

	yang_do_request_keyframe(context, ssrc);
	return err;
}

static int32_t yang_rtcpush_on_rtcp_ps_feedback(YangRtcContext *context,
		YangPushStream *pub, YangRtcpCommon *rtcp) {

	uint8_t fmt = rtcp->header.rc;
	uint32_t ssrc;
	int32_t err = Yang_Ok;

	if (context == NULL || pub == NULL)
		return ERROR_RTC_PUBLISH;

	switch (fmt) {
	case kPLI: {
		ssrc = pub->videoSsrc;
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
	case kFIR: {
		ssrc = pub->videoSsrc;
		if (ssrc) {
			yang_rtcpush_do_request_keyframe(context, ssrc);
		}
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

static int32_t yang_rtcpush_on_rtcp(YangRtcContext *context,
		YangPushStream *pub, YangRtcpCommon *rtcp) {

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


static int32_t yang_rtcpush_send_rtcp_sr(YangRtcContext *context, YangPushStream* pub){

	if(yang_send_rtcp_sr(context,yangtrue,pub->audioSsrc)!=Yang_Ok){
		return yang_error_wrap(1,"send audio sr fail");
	}
	if(yang_send_rtcp_sr(context,yangfalse,pub->videoSsrc)!=Yang_Ok){
		return yang_error_wrap(1,"send video sr fail");
	}
	return Yang_Ok;
}

static int32_t yang_on_video(YangRtcContext* context,YangPushStream* pub,YangPushData *videoData){
	YangPushDataSession* session=(YangPushDataSession*)videoData->pushData;
	if(pub->video_queue == NULL)
		pub->video_queue = session->pushDataBuffer;
	return yang_push_encodeVideo(context,pub->videoSsrc,session);
}

static int32_t yang_on_audio(YangRtcContext* context,YangPushStream* pub,YangPushData *audioData){
	 YangPushDataSession* session=(YangPushDataSession*)audioData->pushData;
	if(pub->audio_queue == NULL)
		pub->audio_queue = session->pushDataBuffer;
	return yang_push_encodeAudio(context,pub->audioSsrc,session,session->buffer);
}



void yang_create_rtcpush(YangRtcPush *push, uint32_t audioSsrc,
		uint32_t videoSsrc) {
	YangPushStream *pushStream;

	if (push == NULL)
		return;

	pushStream=(YangPushStream*) yang_calloc(1,sizeof(YangPushStream));
	push->pubStream=pushStream;
	pushStream->mw_msgs = 0;
#if Yang_Enable_RTC_Audio
	pushStream->audio_queue = NULL;
#endif
#if Yang_Enable_RTC_Video
	pushStream->video_queue = NULL;
#endif
	pushStream->audioSsrc = audioSsrc;
	pushStream->videoSsrc = videoSsrc;
	pushStream->nackBuffer=(uint8_t*)yang_malloc(kRtpPacketSize);

	push->on_rtcp_nack = yang_rtcpush_on_rtcp_nack;
	push->on_rtcp_xr = yang_rtcpush_on_rtcp_xr;
	push->on_rtcp_rr = yang_rtcpush_on_rtcp_rr;
	push->on_rtcp = yang_rtcpush_on_rtcp;
	push->on_rtcp_ps_feedback = yang_rtcpush_on_rtcp_ps_feedback;
	push->check_twcc = yang_rtcpush_check_twcc;
	push->check_bandwidth = yang_check_bindwidth;
	push->send_rtcp_sr=yang_rtcpush_send_rtcp_sr;
	push->on_video = yang_on_video;
	push->on_audio = yang_on_audio;
}
void yang_destroy_rtcpush(YangRtcPush *push) {
	if (push == NULL)
		return;

	yang_free(push->pubStream->nackBuffer);
	yang_free(push->pubStream);
}

