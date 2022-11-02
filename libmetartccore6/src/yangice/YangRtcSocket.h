//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGWEBRTC_YANGUDPHANDLEH_H_
#define SRC_YANGWEBRTC_YANGUDPHANDLEH_H_
#include <yangutil/yangtype.h>
#include <stdint.h>
#include <pthread.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#define socklen_t int
#else

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

typedef struct{

	int32_t fd;
	int32_t localPort;
	int32_t remotePort;
	int32_t isStart;
	int32_t isLoop;
	int32_t notRemoteInit;
	YangSocketProtocol socketProtocol;

	pthread_t threadId;

	char* turnBuffer;

	void* user;
	void (*receive)(char *data, int32_t nb_data,void* user);
	void (*startStunTimer)(void* user);

	struct sockaddr_in local_addr;
	struct sockaddr_in remote_addr;
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
