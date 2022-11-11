
//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGWEBRTC_YANGUDPHANDLEH_H_
#define SRC_YANGWEBRTC_YANGUDPHANDLEH_H_
#include <yangutil/yangtype.h>
#include <stdint.h>
#include <yangutil/sys/YangThread.h>
#include <yangutil/sys/YangSocket.h>

typedef struct{
	yang_socket_t fd;

	int32_t localPort;
	int32_t remotePort;

	yangbool isStart;
	yangbool isLoop;
	yangbool notRemoteInit;

	YangSocketProtocol socketProtocol;

	yang_thread_t threadId;

	char* turnBuffer;

	void* user;
	void (*receive)(char *data, int32_t nb_data,void* user);
	void (*startStunTimer)(void* user);

	yang_socket_addr local_addr;
	yang_socket_addr remote_addr;
}YangRtcSocketSession;

typedef struct {
	YangRtcSocketSession session;
	void (*updateRemoteAddress)(YangRtcSocketSession *sock,char* remoteIp,int32_t port);
	void (*updateRemoteAddress2)(YangRtcSocketSession *sock,uint32_t remoteIp,int32_t port);
	void (*start)(YangRtcSocketSession* sock);
	void (*stop)(YangRtcSocketSession* sock);
	int32_t (*sendData)(YangRtcSocketSession* sock,char* p,int32_t plen);
}YangRtcSocket;

int32_t yang_create_rtcudp(YangRtcSocket* sock,YangSocketProtocol protocol,int32_t plocalPort);
void yang_destroy_rtcudp(YangRtcSocket* sock);


#endif /* SRC_YANGWEBRTC_YANGUDPHANDLEH_H_ */
