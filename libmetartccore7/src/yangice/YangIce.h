//
// Copyright (c) 2019-2025 yanggaofeng
//
#ifndef SRC_YANGICE_YANGICE_H_
#define SRC_YANGICE_YANGICE_H_

#include <yangice/YangRtcStun.h>
#include <yangice/YangRtcSocket.h>
#include <yangice/YangTurnConnection.h>

#include <yangutil/yangavinfo.h>


typedef struct{
	int32_t uid;
	int32_t localPort;
	yangbool isTurnAllocated;
	yangbool isTurnReady;
	YangSocketProtocol turnSocketProtocol;
	YangSocketProtocol rtcSocketProtocol;
	YangIceCandidateType candidateType;
	YangIceCandidateState iceState;
	YangTurnConnection* turnconn;
	YangIceServer server;
	YangRtcStun stun;
	YangIceCallback callback;
}YangIceSession;

typedef struct{
	YangIceSession session;
	int32_t (*initIce)(YangIceSession *session);
	int32_t (*iceHandle)(YangIceSession* session,void* rtcSession,yang_turn_receive receive,char* remoteIp,int32_t remotePort);
}YangIce;

void yang_create_ice(YangIce* ice,YangPeerInfo* peerInfo,YangPeerCallback* peerCallback);
void yang_destroy_ice(YangIce* ice);

#endif /* SRC_YANGICE_YANGICE_H_ */
