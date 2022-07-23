//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGICE_YANGTURNCONNECTION_H_
#define SRC_YANGICE_YANGTURNCONNECTION_H_
#include <yangice/YangRtcTurn.h>
#include <yangutil/yangtype.h>
#include <yangutil/yangavctype.h>
#include <yangrtc/YangRtcUdp.h>
typedef struct{
	YangRtcTurn* turn;
	YangIceServer* server;
	YangRtcUdp* udp;
	char* icePwd;
	uint32_t ip;
	int32_t port;
	yangbool response;
	void* receiveUser;
	void (*receive)(void *psession, char *data, int32_t size);

}YangTurnSession;

typedef struct{
	YangTurnSession session;
	int32_t (*bindingTurnServer)(YangTurnSession* session);
	int32_t (*start)(YangTurnSession* session);
}YangTurnConnection;

void yang_create_turnConnection(YangTurnConnection* conn,YangIceServer* server,YangRtcUdp* udp,int32_t localport);
void yang_destroy_turnConnection(YangTurnConnection* conn);
#endif /* SRC_YANGICE_YANGTURNCONNECTION_H_ */
