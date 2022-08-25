//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef INCLUDE_YANGP2P_YANGP2PSERVERH_H_
#define INCLUDE_YANGP2P_YANGP2PSERVERH_H_

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

typedef struct {
	int32_t serverfd;
	int32_t connFd;
	//int32_t lclPort;
	int32_t isStart;
	int32_t isLoop;
	pthread_t threadId;
	int32_t serverPort;
	void* user;
	char remoteIp[32];

	struct sockaddr_in local_addr;
	struct sockaddr_in remote_addr;
	void (*receive)(char *data, int32_t nb_data,char* response,char* remoteIp,void* user);
	void (*startStunTimer)(void* user);
}YangP2pServer;





#endif /* INCLUDE_YANGP2P_YANGP2PSERVERH_H_ */
