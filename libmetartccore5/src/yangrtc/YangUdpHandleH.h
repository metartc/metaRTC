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

typedef struct {
	int32_t fd;
	int32_t localPort;
	int32_t remotePort;
	int32_t isStart;
	int32_t isLoop;
	int32_t notRemoteInit;
	pthread_t threadId;

	void* user;
	struct sockaddr_in local_addr;
	struct sockaddr_in remote_addr;
	void (*receive)(char *data, int32_t nb_data,void* user);
	void (*startStunTimer)(void* user);
}YangUdpHandle;


#endif /* SRC_YANGWEBRTC_YANGUDPHANDLEH_H_ */
