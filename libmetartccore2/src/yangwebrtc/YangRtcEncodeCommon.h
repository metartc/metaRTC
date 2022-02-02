/*
 * YangRtcEncodeCommon.h
 *
 *  Created on: 2022年1月5日
 *      Author: yang
 */

#ifndef SRC_YANGWEBRTC_YANGRTCENCODECOMMON_H_
#define SRC_YANGWEBRTC_YANGRTCENCODECOMMON_H_
#include <yangrtp/YangRtp.h>
#include <yangrtp/YangRtpBuffer.h>
#include <yangrtp/YangRtpFUAPayload2.h>
#include <yangrtp/YangRtpPacket.h>
#include <yangrtp/YangRtpRawPayload.h>
#include <yangrtp/YangRtpSTAPPayload.h>
#include <yangwebrtc/YangRtcContext.h>
typedef struct {
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
typedef struct {
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


#endif /* SRC_YANGWEBRTC_YANGRTCENCODECOMMON_H_ */
