//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef INCLUDE_YANGP2P_YANGP2PSERVERH_H_
#define INCLUDE_YANGP2P_YANGP2PSERVERH_H_

#include <stdint.h>
#include <yangutil/sys/YangThread.h>
#include <yangutil/sys/YangSocket.h>

typedef struct {

	yang_socket_t serverfd;
	yang_socket_t connFd;

	yangbool isStart;
	yangbool isLoop;

	int32_t serverPort;

	yang_thread_t threadId;

	void* user;
	char remoteIp[32];

	yang_socket_addr local_addr;
	yang_socket_addr remote_addr;
	void (*receive)(char *data, int32_t nb_data,char* response,char* remoteIp,void* user);
	void (*startStunTimer)(void* user);

}YangP2pServer;





#endif /* INCLUDE_YANGP2P_YANGP2PSERVERH_H_ */
