//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGICE_YANGTURNCONNECTION_H_
#define SRC_YANGICE_YANGTURNCONNECTION_H_
#include <yangice/YangRtcTurn.h>

typedef struct{
	YangTurnSession session;
	yangbool (*isReady)(YangTurnSession* session);
	yangbool (*isAllocated)(YangTurnSession* session);
	int32_t (*sendData)(YangTurnSession *psession, int32_t uid,char *data, int32_t nb);
	int32_t (*addPeer)(YangTurnSession *session,int32_t uid,void* rtcSession,yang_turn_receive receive,char* remoteIp,uint16_t remotePort);
	int32_t (*removePeer)(YangTurnSession *session,int32_t uid);
	int32_t (*start)(YangTurnSession* session);
	void (*stop)(YangTurnSession* session);
}YangTurnConnection;

void yang_create_turnConnection(YangTurnConnection* conn,YangIceServer* server,YangRtcUdp* udp,int32_t localport);
void yang_destroy_turnConnection(YangTurnConnection* conn);
#endif /* SRC_YANGICE_YANGTURNCONNECTION_H_ */
