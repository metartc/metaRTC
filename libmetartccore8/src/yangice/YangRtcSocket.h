
//
// Copyright (c) 2019-2026 yanggaofeng
//
#ifndef SRC_YANGWEBRTC_YANGUDPHANDLEH_H_
#define SRC_YANGWEBRTC_YANGUDPHANDLEH_H_

#include <yangutil/sys/YangThread.h>
#include <yangutil/sys/YangSocket.h>

typedef void* (*yang_rtcsocket_thread_app)(void *obj);

typedef struct{
	yang_socket_t fd;

	yangbool isStart;
	yangbool isLoop;
	yangbool notRemoteInit;
	yangbool stunRequesting;
	yangbool binded;

	YangIpFamilyType   familyType;
	YangSocketProtocol socketProtocol;

	int32_t headerLen;

	yang_thread_t threadId;
	yang_thread_mutex_t sendLock;

	char* buffer;

	void* user;
	void (*receive)(char *data, int32_t nb_data,void* user);
	void (*startStunTimer)(void* user);
	void (*updateAddress)(void* user,YangIpAddress* address);

	void* rtcSession;
	YangIpAddress* income_addr;
	YangIpAddress local_addr;
	YangIpAddress remote_addr;

}YangRtcSocketSession;

typedef struct {
	YangRtcSocketSession session;
	void (*updateRemoteAddress)(YangRtcSocketSession *sock,char* remoteIp,int32_t port);
	int32_t (*listen)(YangRtcSocketSession* sock);
	void (*start)(YangRtcSocketSession* sock);
    void (*stop)(YangRtcSocketSession* sock);
    int32_t (*connect)(YangRtcSocketSession* sock);
	int32_t (*write)(YangRtcSocketSession* sock,char* buffer,int32_t bufferLen);
	int32_t (*write2)(YangRtcSocketSession *session, YangIpAddress* remote_addr,char *data, int32_t nb);
	int32_t (*read)(YangRtcSocketSession* sock,char* buffer,int32_t bufferLen,int32_t* nbytes);
}YangRtcSocket;

int32_t yang_create_rtcsocket(YangRtcSocket* sock,YangIpFamilyType familyType,YangSocketProtocol protocol,int32_t plocalPort);
void yang_destroy_rtcsocket(YangRtcSocket* sock);


#endif /* SRC_YANGWEBRTC_YANGUDPHANDLEH_H_ */
