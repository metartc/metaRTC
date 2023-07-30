//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef SRC_YANGRTC_YANGRTCCONNECTION_H_
#define SRC_YANGRTC_YANGRTCCONNECTION_H_

#include <yangrtc/YangRtcContext.h>
#include <yangrtc/YangRtcSession.h>

#include <yangrtp/YangRtpPacket.h>
#include <yangrtp/YangRtpRecvNack.h>
#include <yangrtp/YangReceiveNackBuffer.h>

#include <yangutil/sys/YangTime.h>

int32_t yang_send_rtcppacket(YangRtcContext *context, char *data, int32_t nb);
void yang_do_request_keyframe(YangRtcContext* context,uint32_t ssrc);
int32_t yang_send_rtcp_fb_pli(YangRtcContext* context,uint32_t ssrc);
int32_t yang_send_rtcp_fb_twcc(YangRtcContext *context, YangRecvTWCC* twcc,int32_t twcc_fb_count);
int32_t yang_send_rtcp_rr(YangRtcContext* context,yangbool isAudio,uint32_t ssrc, YangReceiveNackBuffer *rtp_queue,
		const uint64_t last_send_systime,  YangNtp* last_send_ntp);
int32_t yang_send_rtcp_sr(YangRtcContext *context, yangbool isAudio,uint32_t ssrc);
int32_t yang_send_rtcp_xr_rrtr(YangRtcContext* context,uint32_t ssrc);
int32_t yang_check_send_nacks(YangRtcContext* context,YangRtpRecvNack *nack, uint32_t ssrc,
		uint32_t *sent_nacks, uint32_t *timeout_nacks) ;

#endif /* SRC_YANGRTC_YANGRTCCONNECTION_H_ */
