/*
 * YangCH265RtpEncode.h
 *
 *  Created on: 2022年1月2日
 *      Author: yang
 */

#ifndef SRC_YANGWEBRTC_YANGCH265RTPENCODE_H_
#define SRC_YANGWEBRTC_YANGCH265RTPENCODE_H_


#include <yangrtp/YangRtp.h>
#include <yangrtp/YangRtpBuffer.h>
#include <yangrtp/YangRtpPacket.h>
#include <yangrtp/YangRtpRawPayload.h>
#include <yangrtp/YangRtpSTAPPayload.h>
#include <yangrtp/YangRtpFUAPayload2.h>
#include <yangwebrtc/YangRtcContext.h>

#if Yang_H265_Encoding
typedef struct YangH265RtpEncode{
	uint32_t videoSsrc;
		uint32_t audioSsrc;
		uint16_t videoSeq;
		uint16_t audioSeq;
		char* videoBuf;
		YangRtpBuffer *rtpBuffer;

		YangFua2H265Data videoFua2Data;
		YangRtpRawData videoRawData;
		YangRtpRawData audioRawData;
		YangRtpSTAPData stapData;
	    YangBuffer buf;


		YangRtpPacket videoFuaPacket;
		YangRtpPacket videoRawPacket;
		YangRtpPacket videoStapPacket;
		YangRtpPacket audioRawPacket;
}YangH265RtpEncode;
void yang_init_h265RtpEncode(YangH265RtpEncode* rtp,YangRtpBuffer* rtpBuffer);
void yang_destroy_h265RtpEncode(YangH265RtpEncode* rtp);
void yang_init_h265RtpEncode(YangH265RtpEncode* rtp,YangRtpBuffer* rtpBuffer);
void yang_destroy_h265RtpEncode(YangH265RtpEncode* rtp);
int32_t yang_pub_h265_audio(YangRtcContext* context,YangH265RtpEncode* rtp,YangFrame* audioFrame);
int32_t yang_pub_h265_video(YangRtcContext* context,YangH265RtpEncode* rtp,YangFrame* videoFrame);
int32_t yang_pub_h265_package_stap_a(YangRtcContext* context,YangH265RtpEncode* rtp,YangFrame* videoFrame);
#endif



#endif /* SRC_YANGWEBRTC_YANGCH265RTPENCODE_H_ */
