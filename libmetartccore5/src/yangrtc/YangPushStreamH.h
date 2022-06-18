//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGRTC_YANGPUSHSTREAMH_H_
#define SRC_YANGRTC_YANGPUSHSTREAMH_H_
#include <yangrtc/YangRtcContextH.h>
#include <yangrtc/YangRtcStream.h>
typedef struct{
	YangRtcPushStream *pubStream;
	int32_t (*on_recv_nack)(YangRtcContext* context,YangRtcPushStream* pub,YangPublishNackBuffer *que,YangRtcpNack* nack);
	int32_t (*cache_nack)(YangRtcPushStream* pub,YangRtpPacket *pkt,char* p,int plen);
	int32_t (*on_rtcp_nack)(YangRtcContext* context,YangRtcPushStream* pub,YangRtcpCommon *rtcp);
	int32_t (*on_rtcp_xr)(YangRtcContext* context,YangRtcPushStream* pub,YangRtcpCommon *rtcp) ;
	int32_t (*on_rtcp_rr)(YangRtcContext* context,YangRtcPushStream* pub,YangRtcpCommon *rtcp);
	int32_t (*on_rtcp)(YangRtcContext* context,YangRtcPushStream* pub,YangRtcpCommon *rtcp);
	int32_t (*on_rtcp_ps_feedback)(YangRtcContext* context,YangRtcPushStream* pub,YangRtcpCommon *rtcp) ;
	int32_t (*check_twcc)(YangRtcContext* context,YangRtcPushStream* pub);
	int32_t (*check_rrt)(YangRtcContext* context,YangRtcPushStream* pub);
}YangRtcPush;

#endif /* SRC_YANGRTC_YANGPUSHSTREAMH_H_ */
