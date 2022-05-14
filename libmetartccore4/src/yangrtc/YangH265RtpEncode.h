
//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGRTP_YangH265RtpEncode_H_
#define SRC_YANGRTP_YangH265RtpEncode_H_
#include <yangrtc/YangRtcEncodeCommon.h>
#include <yangrtc/YangRtcSession.h>

#ifdef __cplusplus
extern "C"{
#endif

void yang_create_h265RtpEncode(YangH265RtpEncode* rtp,YangRtpBuffer* audioBuffer,YangRtpBuffer* videoBuffer);
void yang_destroy_h265RtpEncode(YangH265RtpEncode* rtp);

int32_t yang_pub_h265_audio(YangRtcSession *session,YangH265RtpEncode* rtp,YangFrame* audioFrame);
int32_t yang_pub_h265_video(YangRtcSession *session,YangH265RtpEncode* rtp,YangFrame* videoFrame);
int32_t yang_pub_h265_package_stap_a(YangRtcSession *session,YangH265RtpEncode* rtp,YangFrame* videoFrame);
#ifdef __cplusplus
}
#endif
#endif /* SRC_YANGRTP_YANGRTPPACKETWRAP_H_ */
