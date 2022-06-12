//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGRTC_YANGRECVTRACK_H_
#define SRC_YANGRTC_YANGRECVTRACK_H_

#include <stdint.h>
#include <yangrtc/YangRtcContext.h>
#include <yangrtp/YangRtpPacket.h>

#include <yangstream/YangStreamType.h>
#include <yangutil/sys/YangSsrc.h>

//

#define yang_cache_index(x) 	 x&YANG_RTC_RECV_BUFFER_COUNT2

#ifdef __cplusplus
extern "C"{
#endif

void yang_create_recvTrack(YangRtcContext* context,YangRecvTrack* recv,int is_audio);
void yang_destroy_recvTrack(YangRecvTrack *recv);
int32_t yang_recvtrack_do_check_send_nacks(YangRtcContext *context,
		YangRecvTrack *recv, uint32_t *timeout_nacks);
int32_t yang_recvtrack_on_nack(YangRtcContext* context,YangRecvTrack* recv,YangRtpPacket *pkt);
int32_t yang_recvtrack_send_rtcp_rr(YangRtcContext* context,YangRecvTrack* recv);
int32_t yang_recvtrack_send_rtcp_xr_rrtr(YangRtcContext* context,YangRecvTrack* recv);
int32_t yang_recvtrack_has_ssrc(YangRtcContext* context,YangRecvTrack* recv,uint32_t ssrc);
void yang_recvtrack_update_rtt(YangRecvTrack* recv,int32_t rtt);

void yang_recvtrack_update_send_report_time(YangRecvTrack* recv, YangNtp* ntp,	uint32_t rtp_time);
void yang_recvtrack_receiveVideo(YangRtcContext *context,YangFrame *videoFrame) ;
#ifdef __cplusplus
}
#endif


#endif /* SRC_YANGRTC_YANGRECVTRACK_H_ */
