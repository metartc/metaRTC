//
// Copyright (c) 2019-2026 yanggaofeng
//
#ifndef SRC_YANGRTC_YANGRTCSESSION_H_
#define SRC_YANGRTC_YANGRTCSESSION_H_
#include <yangrtc/YangRtcContext.h>

#include <yangrtc/YangPull.h>
#include <yangrtc/YangPushH.h>
#include <yangrtc/YangBandwidth.h>
#include <yangrtc/YangDatachannel.h>


#include <yangrtc/YangPushAudio.h>

#include <yangrtc/YangPushStream.h>

#include <yangice/YangIce.h>
#include <yangutil/sys/YangCTimer.h>
#include <yangrtp/YangRtcpCompound.h>

typedef struct {
	yangbool isSendDtls;
	yangbool isControlled;
	yangbool iceInited;

	yangbool isInited;
	yangbool activeState;
	yangbool enableDatachannel;

	int32_t startRecv;


	int64_t lastStunTime;
	int64_t sessionTimeout;

	YangRtpBuffer *playRtpBuffer;


	//YangTimer* 20ms;
	YangCTimer *tm_1s;
	YangCTimer *tm_100ms;

	YangRtcPush *push;
	YangRtcPull *play;
#if Yang_Enable_Datachannel
	YangDatachannel *datachannel;
#endif

	YangRtcContext context;
	YangRtpPacket pushPacket;
	YangRtcpCompound rtcp_compound;

	YangIce ice;

	char remote_fingerprint[128];

} YangRtcSession;

#endif /* SRC_YANGRTC_YANGRTCSESSION_H_ */
