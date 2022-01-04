/*
 * YangCH264RtpEncode.h
 *
 *  Created on: 2022年1月2日
 *      Author: yang
 */

#ifndef SRC_YANGWEBRTC_YANGCH264RTPENCODE_H_
#define SRC_YANGWEBRTC_YANGCH264RTPENCODE_H_



typedef struct YangH264RtpEncode{
	uint16_t videoSeq;
	uint16_t audioSeq;

	uint32_t videoSsrc;
	uint32_t audioSsrc;

	char* videoBuf;
	YangRtpBuffer *rtpBuffer;

	YangFua2H264Data videoFua2Data;
	YangRtpRawData videoRawData;
	YangRtpRawData audioRawData;
	YangRtpSTAPData stapData;
    YangBuffer buf;

	YangRtpPacket videoFuaPacket;
	YangRtpPacket videoRawPacket;
	YangRtpPacket videoStapPacket;
	YangRtpPacket audioRawPacket;
}YangH264RtpEncode;

void yang_init_h264RtpEncode(YangH264RtpEncode* rtp,YangRtpBuffer* rtpBuffer);
void yang_destroy_h264RtpEncode(YangH264RtpEncode* rtp);
int32_t yang_pub_h264_video(YangRtcContext* context,YangH264RtpEncode* rtp,YangFrame* videoFrame);
int32_t yang_pub_h264_audio(YangRtcContext* context,YangH264RtpEncode* rtp,YangFrame* audioFrame);
int32_t yang_pub_h264_package_stap_a(YangRtcContext* context,YangH264RtpEncode* rtp,YangFrame* videoFrame);




#endif /* SRC_YANGWEBRTC_YANGCH264RTPENCODE_H_ */
