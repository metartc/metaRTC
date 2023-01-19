//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGRTC_YANGPUSHH_H_
#define SRC_YANGRTC_YANGPUSHH_H_
#include <yangrtc/YangRtcContext.h>

#include <yangrtp/YangRtp.h>
#include <yangrtp/YangRtpBuffer.h>
#include <yangrtp/YangRtpPacket.h>
#include <yangrtp/YangRtpRawPayload.h>
#include <yangrtp/YangRtpSTAPPayload.h>
#include <yangrtp/YangRtpFUAPayload2.h>

typedef struct {
	uint16_t audioSeq;
	uint32_t audioSsrc;
	char* audioBuffer;
	YangRtpBuffer *audioRtpBuffer;
	YangRtpRawData audioRawData;
    YangBuffer buf;
	YangRtpPacket audioRawPacket;
}YangPushAudioRtp;

typedef struct {
	uint16_t videoSeq;
	uint16_t twccSeq;
	uint32_t videoSsrc;

	char* videoBuf;
	YangRtpBuffer *videoRtpBuffer;
	YangFua2H264Data videoFua2Data;
	YangRtpRawData videoRawData;

	YangRtpSTAPData stapData;
    YangBuffer buf;
#if Yang_Enable_TWCC
    YangRtpExtensions rtpExtension;
#endif
	YangRtpPacket videoFuaPacket;
	YangRtpPacket videoRawPacket;
	YangRtpPacket videoStapPacket;

}YangPushH264Rtp;

typedef struct {
	uint16_t videoSeq;
	uint16_t twccSeq;
	uint32_t videoSsrc;

	char* videoBuf;
	YangRtpBuffer *videoRtpBuffer;

	YangFua2H265Data videoFua2Data;
	YangRtpRawData videoRawData;
	YangRtpSTAPData stapData;
	YangBuffer buf;
#if Yang_Enable_TWCC
	YangRtpExtensions rtpExtension;
#endif
	YangRtpPacket videoFuaPacket;
	YangRtpPacket videoRawPacket;
	YangRtpPacket videoStapPacket;
}YangPushH265Rtp;


typedef struct {
	uint16_t videoSeq;
	uint16_t twccSeq;
	uint32_t videoSsrc;

	char* videoBuf;
	YangRtpBuffer *videoRtpBuffer;
	YangFua2H264Data videoFua2Data;
	YangRtpRawData videoRawData;

	YangRtpSTAPData stapData;
    YangBuffer buf;
#if Yang_Enable_TWCC
    YangRtpExtensions rtpExtension;
#endif
	YangRtpPacket videoFuaPacket;
	YangRtpPacket videoRawPacket;
	YangRtpPacket videoStapPacket;

}YangPushAV1Rtp;

typedef struct {
	uint16_t videoSeq;
	uint16_t twccSeq;
	uint32_t videoSsrc;
	char* videoBuf;
	YangRtpBuffer *videoRtpBuffer;

	YangFua2H264Data videoFua2Data;
	YangRtpRawData videoRawData;

    YangBuffer buf;
#if Yang_Enable_TWCC
    YangRtpExtensions rtpExtension;
#endif
	YangRtpPacket videoFuaPacket;
	YangRtpPacket videoRawPacket;

}YangPushMjpegRtp;


#endif /* SRC_YANGRTC_YANGPUSHH_H_ */
