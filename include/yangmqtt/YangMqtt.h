//
// Copyright (c) 2019-2023 yanggaofeng
//

#ifndef INCLUDE_YANGMQTT_YANGMQTT_H_
#define INCLUDE_YANGMQTT_YANGMQTT_H_
#include <yangutil/yangtype.h>
#include <yangutil/yangavinfotype.h>

typedef struct{
	void* session;
	void (*mqtt_receive)(void* session,char* topic,char* msg);
}YangMqttCallback;

typedef struct{
	void* session;
	int32_t (*connect)(void* session,char *remoteIp,int32_t remotePort,char* username,char* password);
	int32_t (*close)(void* session);
	int32_t (*publish)(void* session,char* topic,char* msg);
	int32_t (*subscribe)(void* session,char* topic);
	int32_t (*unsubscribe)(void* session,char* topic);
}YangMqtt;

#ifdef __cplusplus
extern "C"{
#endif
	int32_t yang_create_mqtt(YangMqtt* mqtt,YangAVInfo* avinfo,YangMqttCallback* mqttcb);
	void yang_destroy_mqtt(YangMqtt* mqtt);
#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_YANGMQTT_YANGMQTT_H_ */
