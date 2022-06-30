//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGWEBRTC_YANGUDPHANDLEH_H_
#define SRC_YANGWEBRTC_YANGUDPHANDLEH_H_

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
	pthread_t threadId;

	void* user;
	void (*receive)(char *data, int32_t nb_data,void* user);
	void (*startStunTimer)(void* user);

	struct sockaddr_in local_addr;
	struct sockaddr_in remote_addr;
}YangUdpSession;

typedef struct {
	YangUdpSession session;
	void (*updateRemoteAddress)(YangUdpSession *udp,char* remoteIp,int32_t port);
	void (*updateRemoteAddress2)(YangUdpSession *udp,uint32_t remoteIp,int32_t port);
	void (*start)(YangUdpSession* udp);
	void (*stop)(YangUdpSession* udp);
	int32_t (*sendData)(YangUdpSession* udp,char* p,int32_t plen);
}YangRtcUdp;

int32_t yang_create_rtcudp(YangRtcUdp* udp,int32_t plocalPort);
void yang_destroy_rtcudp(YangRtcUdp* udp);


#endif /* SRC_YANGWEBRTC_YANGUDPHANDLEH_H_ */
