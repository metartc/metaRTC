//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGRTC_YANGRTCSESSION_H_
#define SRC_YANGRTC_YANGRTCSESSION_H_
#include <yangrtc/YangRtcContext.h>
#include <yangrtc/YangDatachannel.h>
#include <yangrtc/YangBandwidth.h>
#include <yangrtc/YangPlay.h>
#include <yangrtc/YangPushH.h>

#include <yangrtc/YangPushH264.h>
#include <yangrtc/YangPushH265.h>
#include <yangrtc/YangPushStream.h>

#include <yangice/YangIce.h>
#include <yangutil/sys/YangCTimer.h>

typedef struct {
	int32_t isSendDtls;
	int32_t startRecv;

	int32_t sendTwccId;
	int32_t isSendStun;
	int32_t activeState;
	int32_t listenPort;
	int32_t isServer;
	int32_t usingDatachannel;
	int64_t lastStunTime;
	int64_t sessionTimeout;

	YangRtpBuffer *playRtpBuffer;
	YangRtpBuffer *pushVideoRtpBuffer;
	YangRtpBuffer *pushAudioRtpBuffer;
	//YangTimer* 20ms;
	YangCTimer *tm_1s;
	YangCTimer *tm_100ms;

	YangPushH264 *pushH264;
#if Yang_H265_Encoding
	YangPushH265 *pushH265;
#endif

	YangRtcPush *push;
	YangRtcPlay *play;
#if Yang_HaveDatachannel
	YangDatachannel *datachannel;
#endif

	YangAudioParam *remote_audio;
	YangVideoParam *remote_video;

	YangRtcContext context;
	YangRtpPacket pushPacket;
	YangVideoCodec codec;
	YangIce ice;



	char remote_fingerprint[128];
	char remoteIcePwd[64];
	char remote_ufrag[32];
	char localIcePwd[64];
	char local_ufrag[16];
} YangRtcSession;

#endif /* SRC_YANGRTC_YANGRTCSESSION_H_ */
