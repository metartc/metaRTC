//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef SRC_YANGICE_YANGRTCTURN_H_
#define SRC_YANGICE_YANGRTCTURN_H_
#include <yangice/YangRtcStun.h>
#include <yangutil/yangtype.h>
#include <yangutil/yangavctype.h>
#include <yangrtc/YangRtcUdp.h>
#include <yangutil/sys/YangVector.h>
#define   Yang_Turn_Error_BadRequest       400
#define   Yang_Turn_Error_Unauthoorized    401
#define   Yang_Turn_Error_UnknowAttribute  420
#define   Yang_Turn_Error_Nonce            438
#define   Yang_Turn_Transport_Udp          0x11
#define   Yang_Turn_Transport_Tcp          0x06
#define   Yang_Turn_ChaneelBase            0x4000

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
}YangTurnResponseData;

typedef void (*yang_turn_receive)(void *psession, char *data, int32_t size);

typedef struct {
    uint16_t channelNumber;
    yangbool isSending;
	int32_t uid;
	YangTurnState state;
    void* receiveUser;
    yang_turn_receive receive;
	YangIpAddress address;
	YangIpAddress xorAddress;
} YangTurnPeer;

yang_vector_declare2(YangTurnPeer)

typedef struct{
	uint32_t relayIp;
	uint16_t relayPort;

	int32_t isStart;
	int32_t isLoop;
	int32_t lifetime;
	uint16_t channelNum;
	uint64_t refreshTime;
	int32_t peerCount;
	YangTurnState state;
	YangTurnErrorCode errorCode;
	pthread_t threadId;
	pthread_mutex_t turnLock;
	yangbool hasRelayAddress;
	YangTurnResponseData response;


	YangIceServer* server;
	YangRtcUdp* udp;
	//YangTurnPeer* peer;
	char* icePwd;
	YangBuffer* stream;


	char realm[129];
	uint8_t nonce[128];
	uint16_t nonceLen;

	YangTurnPeerVector2 peers;



}YangTurnSession;




#endif /* SRC_YANGICE_YANGRTCTURN_H_ */
