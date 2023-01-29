//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGRTC_YANGRTCSESSION_H_
#define SRC_YANGRTC_YANGRTCSESSION_H_

#include <yangrtc/YangPlay.h>
#include <yangrtc/YangPushH.h>
#include <yangrtc/YangPushH264.h>
#include <yangrtc/YangPushH265.h>
#include <yangrtc/YangPushAudio.h>
#include <yangrtc/YangBandwidth.h>
#include <yangrtc/YangRtcContext.h>
#include <yangrtc/YangPushStream.h>
#include <yangrtc/YangDatachannel.h>

#include <yangice/YangIce.h>
#include <yangutil/sys/YangCTimer.h>
#include <yangrtp/YangRtcpCompound.h>

typedef struct {
	int32_t isSendDtls;
	int32_t startRecv;

	int32_t sendTwccId;
	int32_t isSendStun;
	int32_t activeState;

	yangbool isServer;

	int32_t usingDatachannel;
	int64_t lastStunTime;
	int64_t sessionTimeout;

	YangRtpBuffer *playRtpBuffer;


#if	Yang_Enable_RTC_Audio
	YangRtpBuffer *pushAudioRtpBuffer;
	YangPushAudio *pushAudio;
	YangAudioParam *remote_audio;
#endif

#if	Yang_Enable_RTC_Video
	YangRtpBuffer *pushVideoRtpBuffer;
	YangPushH264 *pushH264;
	#if Yang_Enable_H265_Encoding
	YangPushH265 *pushH265;
	#endif

	#if Yang_Enable_AV1_Encoding
	YangPushAV1 *pushAV1;
	#endif
	#if Yang_Enable_MJpeg_Encoding
	YangPushMjpeg *pushMjpeg;
	#endif
#endif
	//YangTimer* 20ms;
	YangCTimer *tm_1s;
	YangCTimer *tm_100ms;

	YangRtcPush *push;
	YangRtcPlay *play;
#if Yang_Enable_Datachannel
	YangDatachannel *datachannel;
#endif


	YangVideoParam *remote_video;

	YangRtcContext context;
	YangRtpPacket pushPacket;
	YangRtcpCompound rtcp_compound;
	YangVideoCodec codec;
	YangIce ice;

	char remote_fingerprint[128];
	char remoteIcePwd[64];
	char remote_ufrag[32];
	char localIcePwd[64];
	char local_ufrag[16];
} YangRtcSession;

#endif /* SRC_YANGRTC_YANGRTCSESSION_H_ */
