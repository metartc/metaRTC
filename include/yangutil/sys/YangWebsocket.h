#ifndef YANGRTP_YANGWEBSOCKET_H_
#define YANGRTP_YANGWEBSOCKET_H_
#include <yangutil/yangtype.h>
#include <yangutil/yangavtype.h>
#include <yangutil/sys/YangUrl.h>


typedef struct YangWebsocketData{
	uint8_t payload[1024];
	int nb;
}YangWebsocketData;

typedef struct YangWebsocketCallback{
	void* context;
	int (*receive)(void* context,YangSample* data);
}YangWebsocketCallback;

typedef struct{
	void* context;
	int32_t (*connectServer)(void* context,char* url);
	int32_t (*disconnectServer)(void* context);
	int32_t (*sendData)(void* context,uint8_t* p,int nb);
}YangWebsocket;
#ifdef __cplusplus
extern "C"{
#endif
void yang_create_websocket(YangIpFamilyType familyType,YangWebsocket* web,YangWebsocketCallback* callback);
void yang_destroy_websocket(YangWebsocket* web);
#ifdef __cplusplus
}
#endif

#endif /* YANGRTP_YANGWEBSOCKET_H_ */
