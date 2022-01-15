/*
 * YangUdpHandleH.h
 *
 *  Created on: 2022年1月6日
 *      Author: yang
 */

#ifndef SRC_YANGWEBRTC_YANGUDPHANDLEH_H_
#define SRC_YANGWEBRTC_YANGUDPHANDLEH_H_


#include <pthread.h>



#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


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
}YangUdpHandle;


#endif /* SRC_YANGWEBRTC_YANGUDPHANDLEH_H_ */
