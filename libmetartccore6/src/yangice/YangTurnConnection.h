//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGICE_YANGTURNCONNECTION_H_
#define SRC_YANGICE_YANGTURNCONNECTION_H_

#include <yangice/YangRtcStun.h>
#include <yangice/YangRtcSocket.h>

#include <yangutil/yangtype.h>
#include <yangutil/yangavctype.h>
#include <yangutil/sys/YangVector.h>

#define   Yang_Turn_Error_BadRequest       400
#define   Yang_Turn_Error_Unauthoorized    401
#define   Yang_Turn_Error_UnknowAttribute  420
#define   Yang_Turn_Error_Nonce            438
#define   Yang_Turn_Transport_Udp          0x11
#define   Yang_Turn_Transport_Tcp          0x06
#define   Yang_Turn_ChannelNumBase         0x4000
#define   Yang_Turn_Default_Lifetime       600
#define   Yang_Turn_Permission_Lifetime    300

typedef enum {
	YangTurnNew,
	YangTurnCredential,
	YangTurnAllocate,
	YangTurnPermission,
	YangTurnBindChannel,
	YangTurnReady,
	YangTurnClose
} YangTurnState;

typedef enum{
	YangTurnNoneError,
	YangTurnAllocateError,
	YangTurnPermissionError,
	YangTurnBindChannelError,
	YangTurnRefreshError,
	YangTurnKwnownError
}YangTurnErrorCode;


typedef struct{
	uint16_t errorCode;
	uint16_t mappedPort;
	uint32_t mappedAddress;
	int32_t lifetime;
	uint32_t nonceLen;
	uint8_t nonce[128];

}YangTurnResponseData;

typedef void (*yang_turn_receive)(void *psession, char *data, int32_t size);

typedef struct {
    uint16_t channelNumber;
    uint16_t lifetime;
    yangbool isSending;
	int32_t uid;
	uint32_t lifetimeOuttime;
	YangTurnState state;
    void* receiveUser;
    yang_turn_receive receive;
	YangIpAddress address;
	YangIpAddress xorAddress;
} YangTurnPeer;

yang_vector_declare2(YangTurnPeer)

typedef struct{
	uint16_t channelNum;
	uint16_t relayPort;
	uint16_t nonceLen;
	uint16_t permissionCount;
	uint32_t relayIp;
	int32_t isStart;
	int32_t isLoop;
	int32_t waitTime;
	int32_t lifetime;
	uint32_t allocateOuttime;

	YangTurnState state;
	YangTurnErrorCode errorCode;
	yang_thread_t threadId;
	yang_thread_mutex_t turnLock;
	yangbool hasRelayAddress;
	YangTurnResponseData response;

	YangIceServer* server;
	YangRtcSocket* sock;

	char* icePwd;
	YangBuffer* stream;

	char realm[129];
	uint8_t nonce[128];
	YangStunLib stun;
	YangTurnPeerVector2 peers;

}YangTurnSession;

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

void yang_create_turnConnection(YangTurnConnection* conn,YangIceServer* server,YangRtcSocket* sock,int32_t localport);
void yang_destroy_turnConnection(YangTurnConnection* conn);
#endif /* SRC_YANGICE_YANGTURNCONNECTION_H_ */
