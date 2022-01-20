/*
 * YangCH264RtpEncode.h
 *
 *  Created on: 2022年1月2日
 *      Author: yang
 */

#ifndef SRC_YANGWEBRTC_YANGCH264RTPENCODE_H_
#define SRC_YANGWEBRTC_YANGCH264RTPENCODE_H_

#include <yangwebrtc/YangRtcEncodeCommon.h>
#include <yangwebrtc/YangRtcSessionH.h>
void yang_init_h264RtpEncode(YangH264RtpEncode* rtp,YangRtpBuffer* rtpBuffer);
void yang_destroy_h264RtpEncode(YangH264RtpEncode* rtp);
int32_t yang_pub_h264_video(YangRtcSession *session,YangH264RtpEncode* rtp,YangFrame* videoFrame);
int32_t yang_pub_h264_audio(YangRtcSession *session,YangH264RtpEncode* rtp,YangFrame* audioFrame);
int32_t yang_pub_h264_package_stap_a(YangRtcSession *session,YangH264RtpEncode* rtp,YangFrame* videoFrame);




#endif /* SRC_YANGWEBRTC_YANGCH264RTPENCODE_H_ */
