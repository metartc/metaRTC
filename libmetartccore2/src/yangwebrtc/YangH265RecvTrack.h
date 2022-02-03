
#ifndef SRC_YANGWEBRTC_YangH265RecvTrack_H_
#define SRC_YANGWEBRTC_YangH265RecvTrack_H_
#ifdef __cplusplus
extern "C"{
#endif

#include <yangwebrtc/YangRecvTrack.h>
void yang_init_h265recv(YangRtcContext* context,YangH265RecvTrack* recv,YangRtpBuffer *rtpBuffer);
void yang_destroy_h265recv(YangH265RecvTrack* recv);
int32_t yang_h265recv_on_rtp(YangRtcContext* context,YangH265RecvTrack* recv,YangRtpPacket *src);
#ifdef __cplusplus
}
#endif
#endif /* SRC_YANGWEBRTC_YANGRTCVIDEORECVTRACK_H_ */
