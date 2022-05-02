//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGWEBRTC_YangUdpServerH_H_
#define SRC_YANGWEBRTC_YangUdpServerH_H_

#include <stdint.h>
#include <pthread.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
//#define SOCKADDR_IN
#define socklen_t int
#else

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

typedef struct {
	int32_t serverfd;
	int32_t lclPort;
	int32_t isStart;
	int32_t isLoop;
	pthread_t threadId;
	int32_t serverPort;
	//char serverIp[30];
	void* user;
	struct sockaddr_in lcl_addr;
	struct sockaddr_in remote_addr;
	void (*receive)(char *data, int32_t nb_data,void* user);
	void (*startStunTimer)(void* user);
}YangUdpServer;


#endif /* SRC_YANGWEBRTC_YANGUDPHANDLEH_H_ */
