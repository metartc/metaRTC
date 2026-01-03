//
// Copyright (c) 2019-2026 yanggaofeng
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


#include <yangutil/buffer/YangBuffer.h>

typedef struct{
	YangPushStream *pubStream;
	int32_t (*on_video)(YangRtcContext* context,YangPushStream* pub,YangPushData *videoData);
	int32_t (*on_audio)(YangRtcContext* context,YangPushStream* pub,YangPushData *audioData);
	//int32_t (*cache_nack)(YangPushStream* pub,YangRtpPacket *pkt,char* p,int plen);
	int32_t (*send_rtcp_sr)(YangRtcContext *context, YangPushStream* pub);
	int32_t (*on_rtcp_nack)(YangRtcContext* context,YangPushStream* pub,YangRtcpCommon *rtcp);
	int32_t (*on_rtcp_xr)(YangRtcContext* context,YangPushStream* pub,YangRtcpCommon *rtcp) ;
	int32_t (*on_rtcp_rr)(YangRtcContext* context,YangPushStream* pub,YangRtcpCommon *rtcp);
	int32_t (*on_rtcp)(YangRtcContext* context,YangPushStream* pub,YangRtcpCommon *rtcp);
	int32_t (*on_rtcp_ps_feedback)(YangRtcContext* context,YangPushStream* pub,YangRtcpCommon *rtcp) ;
	int32_t (*check_twcc)(YangRtcContext* context,YangPushStream* pub);
	int32_t (*check_bandwidth)(YangRtcContext* context,YangPushStream* pub);

}YangRtcPush;

void yang_create_rtcpush(YangRtcPush* pub,uint32_t audioSsrc, uint32_t videoSsrc);
void yang_destroy_rtcpush(YangRtcPush* pub);

#endif /* YANGWEBRTC_YANGRTCPUBLISHSTREAM_H_ */
