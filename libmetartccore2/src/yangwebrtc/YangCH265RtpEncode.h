/*
 * YangCH265RtpEncode.h
 *
 *  Created on: 2022年1月2日
 *      Author: yang
 */

#ifndef SRC_YANGWEBRTC_YANGCH265RTPENCODE_H_
#define SRC_YANGWEBRTC_YANGCH265RTPENCODE_H_


#include <yangwebrtc/YangRtcEncodeCommon.h>
#include <yangwebrtc/YangRtcSession.h>
#if Yang_H265_Encoding

void yang_init_h265RtpEncode(YangH265RtpEncode* rtp,YangRtpBuffer* rtpBuffer);
void yang_destroy_h265RtpEncode(YangH265RtpEncode* rtp);
void yang_init_h265RtpEncode(YangH265RtpEncode* rtp,YangRtpBuffer* rtpBuffer);
void yang_destroy_h265RtpEncode(YangH265RtpEncode* rtp);
int32_t yang_pub_h265_audio(YangRtcSession *session,YangH265RtpEncode* rtp,YangFrame* audioFrame);
int32_t yang_pub_h265_video(YangRtcSession *session,YangH265RtpEncode* rtp,YangFrame* videoFrame);
int32_t yang_pub_h265_package_stap_a(YangRtcSession *session,YangH265RtpEncode* rtp,YangFrame* videoFrame);
#endif



#endif /* SRC_YANGWEBRTC_YANGCH265RTPENCODE_H_ */
