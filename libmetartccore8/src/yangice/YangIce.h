//
// Copyright (c) 2019-2023 yanggaofeng
//
#ifndef SRC_YANGICE_YANGICE_H_
#define SRC_YANGICE_YANGICE_H_
#include <yangice/YangRtcSocket.h>

#include <yangice/YangIceDef.h>
#include <yangice/YangRtcStun.h>
#include <yangice/YangIceAgent.h>
#include <yangice/YangTurnConnection.h>

#include <yangutil/yangavinfo.h>
#include <yangsdp/YangCandidate.h>

typedef struct{
	char *data;
	int32_t nb;
} YangStunData;

typedef void (*yang_turn_start)(void *psession);

typedef struct{
	int32_t uid;
	int32_t turnUid;
	int32_t localPort;

	yangbool sdpWithCandidate;
	yangbool isTurnAllocated;
	yangbool isTurnReady;
	yangbool isPaired;
	YangSocketProtocol turnSocketProtocol;
	YangSocketProtocol rtcSocketProtocol;
	YangIceCandidateType candidateType;
	YangIceCandidateState iceState;
	YangIceGatheringState gatherState;
	YangIceMode remoteIceMode;
	YangIceCandidateType gatherCandidateType;
	YangTurnConnection* turnconn;
	yang_turn_receive receive;
	yang_turn_start turnStart;
	void* rtcSession;

	YangIceAgent agent;
	YangIceServer server;
	YangRtcStun stun;
	YangIceCallback callback;
	YangStunData stunData;
	char local_ufrag[16];
	char remote_ufrag[32];
	char localIcePwd[64];
	char remoteIcePwd[64];
}YangIceSession;

typedef struct{
	YangIceSession session;

	yangbool (*isStunRtp)(YangIceSession* session,char* data,int32_t len);
	int32_t (*onStunRtp)(YangIceSession* session,char* data,int32_t len);
	int32_t (*sendRequestStun)(YangIceSession* session);
	void (*setRequestStunData)(YangIceSession* session,char* data,int32_t nb);
	int32_t (*addIceCandidate)(YangIceSession *session,char* candidateStr);
	int32_t (*addSdpCandidate)(YangIceSession *session,YangCandidate* candidate);
	int32_t (*addIceCandidate2)(YangIceSession *session,YangIpAddress* ip);
	int32_t (*startIceAgent)(YangIceSession *session);
	int32_t (*requestStunServer)(YangIceSession *session);
	void (*onIceStateChange)(YangIceSession* session,YangIceCandidateState iceState);
	int32_t (*onStunResponse)(YangIceSession* session,char* data,int32_t nb);

}YangIce;

void yang_create_ice(YangIce* ice,void* rtcSession,YangPeerInfo* peerInfo,YangPeerCallback* callback,yang_turn_receive receive,yang_turn_start turnStart);
void yang_destroy_ice(YangIce* ice);

#endif /* SRC_YANGICE_YANGICE_H_ */
