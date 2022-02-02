#ifndef SRC_YANGWEBRTC_YANGRTCAUDIORECVTRACK_H_
#define SRC_YANGWEBRTC_YANGRTCAUDIORECVTRACK_H_
#include <yangwebrtc/YangRecvTrack.h>

#ifdef __cplusplus
extern "C"{
#endif
void yang_init_recvaudioTrack(YangRtcContext* context,YangRecvTrack* audiorecv,YangRtpBuffer *rtpBuffer);
void yang_destroy_recvaudioTrack(YangRecvTrack* audiorecv);
int32_t yang_recvaudioTrack_on_rtp(YangRtcContext* context,YangRtpPacket *ppkt);
int32_t yang_recvaudioTrack_check_send_nacks(YangRtcContext* context,YangRecvTrack* audiorecv);
#ifdef __cplusplus
}
#endif

#endif /* SRC_YANGWEBRTC_YANGRTCAUDIORECVTRACK_H_ */
