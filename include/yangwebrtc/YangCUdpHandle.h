/*
 * YangCUdpHandle.h
 *
 *  Created on: 2021年12月28日
 *      Author: yang
 */

#ifndef SRC_YANGWEBRTC_YANGCUDPHANDLE_H_
#define SRC_YANGWEBRTC_YANGCUDPHANDLE_H_

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
int32_t yang_init_rtcudp(YangUdpHandle* udp,char* pserverIp,int32_t plocalPort);
void yang_destroy_rtcudp(YangUdpHandle* udp);
void yang_start_rtcudp(YangUdpHandle* udp);
void yang_stop_rtcudp(YangUdpHandle* udp);
int32_t yang_rtc_sendData(YangUdpHandle* udp,char* p,int32_t plen);



#endif /* SRC_YANGWEBRTC_YANGCUDPHANDLE_H_ */
