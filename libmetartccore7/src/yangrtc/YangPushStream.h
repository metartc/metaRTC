//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGWEBRTC_YANGRTCPUBLISHSTREAM_H_
#define YANGWEBRTC_YANGRTCPUBLISHSTREAM_H_

#include <yangrtc/YangRtcStream.h>
#include <yangrtc/YangRtcContext.h>

#include <yangrtp/YangRtcpXr.h>
#include <yangrtp/YangRtcpRR.h>
#include <yangrtp/YangRtcpXr.h>
#include <yangrtp/YangRtcpNack.h>
#include <yangrtp/YangRtpPacket.h>
#include <yangrtp/YangRtcpPsfbCommon.h>
#include <yangrtp/YangPublishNackBuffer.h>

#include <yangutil/buffer/YangBuffer.h>

typedef struct{
	YangRtcPushStream *pubStream;
	int32_t (*on_recv_nack)(YangRtcContext* context,YangRtcPushStream* pub,YangPublishNackBuffer *que,YangRtcpNack* nack);
	int32_t (*cache_nack)(YangRtcPushStream* pub,YangRtpPacket *pkt,char* p,int plen);
	int32_t (*send_rtcp_sr)(YangRtcContext *context, YangRtcPushStream* pub);
	int32_t (*on_rtcp_nack)(YangRtcContext* context,YangRtcPushStream* pub,YangRtcpCommon *rtcp);
	int32_t (*on_rtcp_xr)(YangRtcContext* context,YangRtcPushStream* pub,YangRtcpCommon *rtcp) ;
	int32_t (*on_rtcp_rr)(YangRtcContext* context,YangRtcPushStream* pub,YangRtcpCommon *rtcp);
	int32_t (*on_rtcp)(YangRtcContext* context,YangRtcPushStream* pub,YangRtcpCommon *rtcp);
	int32_t (*on_rtcp_ps_feedback)(YangRtcContext* context,YangRtcPushStream* pub,YangRtcpCommon *rtcp) ;
	int32_t (*check_twcc)(YangRtcContext* context,YangRtcPushStream* pub);
	int32_t (*check_bandwidth)(YangRtcContext* context,YangRtcPushStream* pub);

}YangRtcPush;

void yang_create_rtcpush(YangRtcPush* pub,uint32_t audioSsrc, uint32_t videoSsrc);
void yang_destroy_rtcpush(YangRtcPush* pub);

#endif /* YANGWEBRTC_YANGRTCPUBLISHSTREAM_H_ */
