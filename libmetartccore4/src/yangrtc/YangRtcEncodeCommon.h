//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGRTC_YANGRTCENCODECOMMON_H_
#define SRC_YANGRTC_YANGRTCENCODECOMMON_H_
#include <yangrtc/YangRtcContext.h>
#include <yangrtp/YangRtp.h>
#include <yangrtp/YangRtpBuffer.h>
#include <yangrtp/YangRtpFUAPayload2.h>
#include <yangrtp/YangRtpPacket.h>
#include <yangrtp/YangRtpRawPayload.h>
#include <yangrtp/YangRtpSTAPPayload.h>
typedef struct {
	uint16_t videoSeq;
	uint16_t audioSeq;

	uint32_t videoSsrc;
	uint32_t audioSsrc;

	char* videoBuf;
	YangRtpBuffer *videoRtpBuffer;
	YangRtpBuffer *audioRtpBuffer;
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
	YangRtpBuffer *videoRtpBuffer;
	YangRtpBuffer *audioRtpBuffer;

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


#endif /* SRC_YANGRTC_YANGRTCENCODECOMMON_H_ */
