//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGRTC_YANGRTCSESSIONH_H_
#define SRC_YANGRTC_YANGRTCSESSIONH_H_
#include <yangrtc/YangRtcContext.h>
#include <yangrtc/YangRtcEncodeCommon.h>
#include <yangutil/sys/YangCTimer.h>

typedef struct {
	YangRtpBuffer *rtpBuffer;
	//YangTimer* 20ms;
	YangCTimer *tm_1s;
	YangCTimer *tm_100ms;
	YangH264RtpEncode *h264Encoder;
#if Yang_H265_Encoding
	YangH265RtpEncode *h265Encoder;
#endif

	YangRtpExtensionTypes extension_types_;
	int32_t isSendDtls;
	int32_t startRecv;
	int32_t twcc_id_;
	int32_t isSendStun;
	int32_t activeState;
	YangAudioParam *remote_audio;
	YangVideoParam *remote_video;
	YangRtcPublishStream *pubStream;
	YangRtcPlayStream *playStream;
	YangRtcContext context;
	YangRtpPacket pushPacket;
	//YangStunPacket stun;
	YangVideoCodec codec;
	char remoteIcePwd[128];
	char ufrag[16];

} YangRtcSession;

#endif /* SRC_YANGRTC_YANGRTCSESSIONH_H_ */
