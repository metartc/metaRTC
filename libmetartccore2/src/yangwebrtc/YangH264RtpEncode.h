//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGRTP_YANGRTPPACKETWRAP_H_
#define SRC_YANGRTP_YANGRTPPACKETWRAP_H_

#include <yangwebrtc/YangRtcEncodeCommon.h>
#include <yangwebrtc/YangRtcSessionH.h>
#ifdef __cplusplus
extern "C"{
#endif

void yang_init_h264RtpEncode(YangH264RtpEncode* rtp,YangRtpBuffer* rtpBuffer);
void yang_destroy_h264RtpEncode(YangH264RtpEncode* rtp);
int32_t yang_pub_h264_video(YangRtcSession *session,YangH264RtpEncode* rtp,YangFrame* videoFrame);
int32_t yang_pub_h264_audio(YangRtcSession *session,YangH264RtpEncode* rtp,YangFrame* audioFrame);
int32_t yang_pub_h264_package_stap_a(YangRtcSession *session,YangH264RtpEncode* rtp,YangFrame* videoFrame);
#ifdef __cplusplus
}
#endif
#endif /* SRC_YANGRTP_YANGRTPPACKETWRAP_H_ */
