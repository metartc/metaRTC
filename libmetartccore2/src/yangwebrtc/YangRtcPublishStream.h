#ifndef YANGWEBRTC_YANGRTCPUBLISHSTREAM_H_
#define YANGWEBRTC_YANGRTCPUBLISHSTREAM_H_

#include <yangrtp/YangPublishNackBuffer.h>
#include <yangrtp/YangRtcpNack.h>
#include <yangrtp/YangRtcpPsfbCommon.h>
#include <yangrtp/YangRtcpRR.h>
#include <yangrtp/YangRtcpXr.h>
#include <yangrtp/YangRtpPacket.h>
#include <yangutil/buffer/YangBuffer.h>
#include <yangrtp/YangRtcpXr.h>
#include <yangwebrtc/YangRtcContext.h>
#ifdef __cplusplus
extern "C"{
#endif


void yang_init_rtcpublish(YangRtcPublishStream* pub,uint32_t audioSsrc, uint32_t videoSsrc);
void yang_destroy_rtcpublish(YangRtcPublishStream* pub);
int32_t yang_rtcpublish_on_recv_nack(YangRtcContext* context,YangRtcPublishStream* pub,YangPublishNackBuffer *que,YangRtcpNack* nack);
int32_t yang_rtcpublish_cache_nack(YangRtcPublishStream* pub,YangRtpPacket *pkt,char* p,int plen);
int32_t yang_rtcpublish_on_rtcp_nack(YangRtcContext* context,YangRtcPublishStream* pub,YangRtcpCommon *rtcp);
int32_t yang_rtcpublish_on_rtcp_xr(YangRtcContext* context,YangRtcPublishStream* pub,YangRtcpCommon *rtcp) ;
int32_t yang_rtcpublish_on_rtcp_rr(YangRtcContext* context,YangRtcPublishStream* pub,YangRtcpCommon *rtcp);
int32_t yang_rtcpublish_on_rtcp(YangRtcContext* context,YangRtcPublishStream* pub,YangRtcpCommon *rtcp);
int32_t yang_rtcpublish_on_rtcp_ps_feedback(YangRtcContext* context,YangRtcPublishStream* pub,YangRtcpCommon *rtcp) ;

#ifdef __cplusplus
}
#endif

#endif /* YANGWEBRTC_YANGRTCPUBLISHSTREAM_H_ */
