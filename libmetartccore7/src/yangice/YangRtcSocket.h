
//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGWEBRTC_YANGUDPHANDLEH_H_
#define SRC_YANGWEBRTC_YANGUDPHANDLEH_H_

#include <yangutil/sys/YangThread.h>
#include <yangutil/sys/YangSocket.h>

typedef void* (*yang_rtcsocket_thread_app)(void *obj);



typedef struct{
	yang_socket_t fd;
	yangbool isControlled;
	yangbool isStart;
	yangbool isLoop;
	yangbool notRemoteInit;

	YangIpFamilyType   familyType;
	YangSocketProtocol socketProtocol;

	int32_t headerLen;

	yang_thread_t threadId;
	yang_thread_mutex_t sendLock;

	char* buffer;

	void* user;
	void (*receive)(char *data, int32_t nb_data,void* user);
	void (*startStunTimer)(void* user);

	YangIpAddress local_addr;
	YangIpAddress remote_addr;

}YangRtcSocketSession;

typedef struct {
	YangRtcSocketSession session;
	void (*updateRemoteAddress)(YangRtcSocketSession *sock,char* remoteIp,int32_t port);
	void (*start)(YangRtcSocketSession* sock);
	void (*stop)(YangRtcSocketSession* sock);
	int32_t (*write)(YangRtcSocketSession* sock,char* buffer,int32_t bufferLen);
}YangRtcSocket;

int32_t yang_create_rtcsocket(YangRtcSocket* sock,YangIpFamilyType familyType,YangSocketProtocol protocol,int32_t plocalPort);

#if Yang_Enable_Tcp_Srs
int32_t yang_create_rtcsocket_srs(YangRtcSocket* sock,YangSocketProtocol protocol);
#endif

void yang_destroy_rtcsocket(YangRtcSocket* sock);


#endif /* SRC_YANGWEBRTC_YANGUDPHANDLEH_H_ */
