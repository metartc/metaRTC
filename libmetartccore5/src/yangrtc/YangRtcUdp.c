//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtc/YangRtcUdp.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/yang_unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#ifndef _WIN32
#define GetSockError()	errno
#define SetSockError(e)	errno = e

#define closesocket(s)	close(s)
#endif


int32_t yang_rtc_sendData(YangUdpSession *session, char *data, int32_t nb) {
	if (session == NULL || !session->isStart || session->notRemoteInit || data==NULL)	return ERROR_RTC_UDP;
	return sendto(session->fd, data, nb, 0, (struct sockaddr*) &session->remote_addr,sizeof(struct sockaddr)) > 0 ? Yang_Ok : 1;
}

int32_t yang_rtc_sendData2(YangUdpSession *session, struct sockaddr* remote_addr,char *data, int32_t nb) {
	if (session == NULL || !session->isStart || session->notRemoteInit || data==NULL)	return ERROR_RTC_UDP;
	return sendto(session->fd, data, nb, 0, remote_addr,sizeof(struct sockaddr)) > 0 ? Yang_Ok : 1;
}

void* yang_run_rtcudp_thread(void *obj) {
#ifdef _WIN32
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD(2, 2);
    WSAStartup(wVersionRequested, &wsaData);
#endif
    YangUdpSession *udp = (YangUdpSession*) obj;

	udp->isStart = 1;
#ifdef _WIN32
    int32_t timeout=200;
    setsockopt(udp->fd, SOL_SOCKET, SO_RCVTIMEO, (const char*) &timeout,  sizeof(timeout));
#else
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 200000;  // 200 ms
	setsockopt(udp->fd, SOL_SOCKET, SO_RCVTIMEO, (const char*) &tv,	sizeof(struct timeval));
#endif


	if (bind(udp->fd, (struct sockaddr*) &udp->local_addr,
			sizeof(struct sockaddr_in)) < 0) {

		yang_error("Udp server bind error");
		exit(1);
	}

	char buffer[2048] = { 0 };
	udp->isLoop = 1;

	int32_t len = 0;
	if (!udp->notRemoteInit&&udp->startStunTimer)	udp->startStunTimer(udp->user);
	socklen_t src_len = sizeof(struct sockaddr_in);
	yang_trace("\n udp server is starting,localPort=%d", udp->localPort);
	while (udp->isLoop) {
		struct sockaddr_in src;
		memset(&src, 0, src_len);
		memset(buffer, 0, 2048);
		if ((len = recvfrom(udp->fd, buffer, 2048, 0,	(struct sockaddr*) &src, &src_len)) > 0) {
			if(udp->notRemoteInit){

				udp->remotePort = ntohs(src.sin_port);
				udp->remote_addr.sin_port = src.sin_port;//htons(udp->remotePort);

#ifdef _WIN32
				udp->remote_addr.sin_addr.S_un.S_addr=src.sin_addr.S_un.S_addr;
#else
				udp->remote_addr.sin_addr.s_addr = src.sin_addr.s_addr;
#endif
				udp->notRemoteInit=0;
			}
			if (udp->receive)	udp->receive(buffer, len, udp->user);
		}
	}
	udp->isStart = 0;
    closesocket(udp->fd);
	udp->fd = -1;

	return NULL;
}

void yang_start_rtcudp(YangUdpSession *udp) {
	if (udp == NULL)	return;
	if (pthread_create(&udp->threadId, 0, yang_run_rtcudp_thread, udp)) {
		yang_error("YangThread::start could not start thread");
	}
}

void yang_stop_rtcudp(YangUdpSession *udp) {
	if (udp == NULL)	return;
	udp->isLoop = 0;
	while (udp->isStart)
		yang_usleep(1000);
}

void yang_rtcudp_update_remoteAddr(YangUdpSession *udp,char* remoteIp,int32_t port){
	//if(port==0) return;
	yang_trace("\nremoteIp=%s,port=%d",remoteIp,port);
	udp->remote_addr.sin_family = AF_INET;
	udp->remotePort = port;
	udp->remote_addr.sin_port = htons(udp->remotePort);
#ifdef _WIN32
    udp->remote_addr.sin_addr.S_un.S_addr=inet_addr(remoteIp);
#else
	udp->remote_addr.sin_addr.s_addr = inet_addr(remoteIp);
#endif
	udp->notRemoteInit=(port==0?1:0);

}


void yang_rtcudp_update_remoteAddr2(YangUdpSession *udp,uint32_t remoteIp,int32_t port){
	//if(port==0) return;
	yang_trace("\nremoteIp=%s,port=%d",remoteIp,port);
	udp->remote_addr.sin_family = AF_INET;
	udp->remotePort = port;
	udp->remote_addr.sin_port = htons(udp->remotePort);
#ifdef _WIN32
    udp->remote_addr.sin_addr.S_un.S_addr=remoteIp;
#else
	udp->remote_addr.sin_addr.s_addr = remoteIp;
#endif
	udp->notRemoteInit=(port==0?1:0);

}


int32_t yang_create_rtcudp(YangRtcUdp *pudp, int32_t plocalPort) {
	if (pudp == NULL)		return ERROR_RTC_UDP;
	YangUdpSession* udp=&pudp->session;

	udp->localPort = plocalPort;
	udp->fd = -1;


	udp->local_addr.sin_family = AF_INET;
	udp->local_addr.sin_port = htons(udp->localPort);

	udp->local_addr.sin_addr.s_addr = INADDR_ANY;
	udp->fd = socket(AF_INET, SOCK_DGRAM, 0);

	udp->notRemoteInit=1;

	pudp->start=yang_start_rtcudp;
	pudp->stop=yang_stop_rtcudp;
	pudp->sendData=yang_rtc_sendData;
	pudp->updateRemoteAddress=yang_rtcudp_update_remoteAddr;
	pudp->updateRemoteAddress2=yang_rtcudp_update_remoteAddr2;
	return Yang_Ok;

}

void yang_destroy_rtcudp(YangRtcUdp *udp) {
    if (udp == NULL)        return;
    yang_stop_rtcudp(&udp->session);
    if (udp->session.fd > 0) {
        closesocket(udp->session.fd);
        udp->session.fd = -1;
    }

}
