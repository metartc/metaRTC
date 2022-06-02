//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGRTC_YANGRTCAUDIORECVTRACK_H_
#define SRC_YANGRTC_YANGRTCAUDIORECVTRACK_H_
#include <yangrtc/YangRecvTrack.h>

#ifdef __cplusplus
extern "C"{
#endif
void yang_create_recvaudioTrack(YangRtcContext* context,YangRecvTrack* audiorecv);
void yang_destroy_recvaudioTrack(YangRecvTrack* audiorecv);
int32_t yang_recvaudioTrack_on_rtp(YangRtcContext* context,YangRtpPacket *ppkt);
int32_t yang_recvaudioTrack_check_send_nacks(YangRtcContext* context,YangRecvTrack* audiorecv);
#ifdef __cplusplus
}
#endif

#endif /* SRC_YANGRTC_YANGRTCAUDIORECVTRACK_H_ */
