//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGWEBRTC_YangH264RecvTrack_H_
#define SRC_YANGWEBRTC_YangH264RecvTrack_H_


#include <yangwebrtc/YangRecvTrack.h>
#ifdef __cplusplus
extern "C"{
#endif

void yang_init_h264recv(YangRtcContext* context,YangH264RecvTrack* recv,YangRtpBuffer *rtpBuffer);
void yang_destroy_h264recv(YangH264RecvTrack* recv);
int32_t yang_h264recv_on_rtp(YangRtcContext* context,YangH264RecvTrack* recv,YangRtpPacket *src);
#ifdef __cplusplus
}
#endif

#endif /* SRC_YANGWEBRTC_YANGRTCVIDEORECVTRACK_H_ */
