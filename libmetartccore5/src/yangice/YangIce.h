//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGICE_YANGICE_H_
#define SRC_YANGICE_YANGICE_H_
#include <yangice/YangRtcStun.h>
#include <yangice/YangTurnConnection.h>
#include <yangrtc/YangRtcUdp.h>
#include <yangutil/yangavtype.h>
#include <yangutil/yangavinfotype.h>

typedef struct{
	int32_t uid;
	int32_t localPort;
	yangbool isTurnAllocated;
	yangbool isTurnReady;
	YangIceCandidateType candidateType;
	YangTurnConnection* turnconn;
	YangIceServer server;
	YangRtcStun stun;
}YangIceSession;

typedef struct{
	YangIceSession session;
	void (*initIce)(YangIceSession *session);
	YangIceCandidateType (*iceHandle)(YangIceSession* session,void* rtcSession,yang_turn_receive receive,char* remoteIp,int32_t remotePort);
}YangIce;

void yang_create_ice(YangIce* ice,YangStreamConfig* config,YangAVInfo* avinfo);
void yang_destroy_ice(YangIce* ice);

#endif /* SRC_YANGICE_YANGICE_H_ */
