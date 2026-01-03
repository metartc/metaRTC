//
// Copyright (c) 2019-2023 yanggaofeng
//

#ifndef SRC_YANGICE_YANGICEAGENT_H_
#define SRC_YANGICE_YANGICEAGENT_H_
#include <yangutil/yangtype.h>
#include <yangutil/yangavinfo.h>
#include <yangutil/sys/YangVector.h>
#include <yangice/YangRtcStun.h>
#include <yangsdp/YangCandidate.h>

typedef struct{
	void* session;
	int32_t (*addRemoteCandidate)(void* session,char* candidateStr);
	int32_t (*addRemoteSdpCandidate)(void* session,YangCandidate* candidate);
	int32_t (*addRemoteCandidate2)(void* session,YangIpAddress* ip);
	YangCandidate* (*getRemoteCandidateByTurn)(void* session);
	int32_t (*startGather)(void* session);
	void (*stopGather)(void* session);
	int32_t (*startConnectivity)(void* session);
	void (*stopConnectivity)(void* session);
	int32_t (*on_stun)(void* session,YangStunPacket* response);
	int32_t (*on_server_stun)(void* psession,YangStunPacket* request);
}YangIceAgent;

void yang_create_iceAgent(YangIceAgent* agent,void* iceSession);
void yang_destroy_iceAgent(YangIceAgent* agent);

#endif /* SRC_YANGICE_YANGICEAGENT_H_ */
