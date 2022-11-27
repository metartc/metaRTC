//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangice/YangRtcSocket.h>
#include <yangutil/yang_unistd.h>
#include <yangutil/sys/YangLog.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


int32_t yang_rtc_sendData(YangRtcSocketSession *session, char *data, int32_t nb) {
	if (session == NULL || !session->isStart || session->notRemoteInit || data==NULL)	return ERROR_RTC_UDP;
	return yang_socket_sendto(session->fd, data, nb, &session->remote_addr) > 0 ? Yang_Ok : 1;
}

int32_t yang_rtc_sendData2(YangRtcSocketSession *session, yang_socket_addr* remote_addr,char *data, int32_t nb) {
	if (session == NULL || !session->isStart || session->notRemoteInit || data==NULL)	return ERROR_RTC_UDP;
	return yang_socket_sendto(session->fd, data, nb,remote_addr) > 0 ? Yang_Ok : 1;
}

void* yang_run_rtcudp_thread(void *obj) {
    YangRtcSocketSession *sock = (YangRtcSocketSession*) obj;

	sock->isStart = yangtrue;
	yang_socket_setsockopt_timeout(sock->fd,800);

	if (yang_socket_bind(sock->fd, &sock->local_addr) < 0) {

		yang_error("Udp server bind error");
		exit(1);
	}

	char buffer[2048] = { 0 };
	sock->isLoop = yangtrue;

	int32_t len = 0;
	if (!sock->notRemoteInit&&sock->startStunTimer)	sock->startStunTimer(sock->user);
	socklen_t src_len = sizeof(yang_socket_addr);
	yang_trace("\n udp server is starting,localPort=%d", sock->localPort);
	while (sock->isLoop) {
		yang_socket_addr src;
		memset(&src, 0, src_len);
		memset(buffer, 0, 2048);
		if ((len = yang_socket_recvfrom(sock->fd, buffer, 2048,  &src, &src_len)) > 0) {

			if(sock->notRemoteInit){

				sock->remotePort = ntohs(src.sin_port);
				sock->remote_addr.sin_port = src.sin_port;//htons(udp->remotePort);

#ifdef _WIN32
				sock->remote_addr.sin_addr.S_un.S_addr=src.sin_addr.S_un.S_addr;
#else
				sock->remote_addr.sin_addr.s_addr = src.sin_addr.s_addr;
#endif
				sock->notRemoteInit=0;
			}
			if (sock->receive)	sock->receive(buffer, len, sock->user);
		}
	}

	sock->isStart = yangfalse;
	yang_socket_close(sock->fd);
	sock->fd = -1;

	return NULL;
}


void* yang_run_rtctcp_thread(void *obj) {

    YangRtcSocketSession *sock = (YangRtcSocketSession*) obj;

	sock->isStart = yangtrue;
	yang_socket_setsockopt_timeout(sock->fd,800);

	yang_trace("\nsock->remote_addr>>>>>>>>>%u,port=%hu",sock->remote_addr.sin_addr.s_addr,ntohs(sock->remote_addr.sin_port));
	if (yang_socket_connect(sock->fd, &sock->remote_addr) == -1) {
		yang_socket_close(sock->fd);
		yang_error_wrap(ERROR_SOCKET, "rtc tcp connect socket error(%d)",GetSockError());
		return NULL;
	}

	char buffer[4096] = { 0 };
	sock->isLoop = yangtrue;

	int32_t len = 0;
	if (!sock->notRemoteInit&&sock->startStunTimer)	sock->startStunTimer(sock->user);

	socklen_t src_len = sizeof(yang_socket_addr);
	yang_trace("\n udp server is starting,localPort=%d", sock->localPort);

	while (sock->isLoop) {
		yang_socket_addr src;
		memset(&src, 0, src_len);
		memset(buffer, 0, 4096);
        if ((len = yang_socket_recv(sock->fd, buffer, 4096, 0) > 0)) {
			if (sock->receive&&len>0)	sock->receive(buffer, len, sock->user);
		}
	}

	sock->isStart = yangfalse;
	yang_socket_close(sock->fd);
	sock->fd = -1;

	return NULL;
}

void yang_start_rtcudp(YangRtcSocketSession *sock) {
	if (sock == NULL || sock->isStart)	return;
	if(sock->socketProtocol==Yang_Socket_Protocol_Udp){
		if (yang_thread_create(&sock->threadId, 0, yang_run_rtcudp_thread, sock)) {
			yang_error("YangThread::start could not start thread");
		}
	}else if(sock->socketProtocol==Yang_Socket_Protocol_Tcp){
		if (yang_thread_create(&sock->threadId, 0, yang_run_rtctcp_thread, sock)) {
					yang_error("YangThread::start could not start thread");
		}
	}
}

void yang_stop_rtcudp(YangRtcSocketSession *sock) {
	if (sock == NULL)	return;
	sock->isLoop = yangfalse;
	while (sock->isStart)	yang_usleep(1000);
}

void yang_rtcudp_update_remoteAddr(YangRtcSocketSession *sock,char* remoteIp,int32_t port){

	yang_trace("\nremoteIp=%s,port=%d",remoteIp,port);
	sock->remote_addr.sin_family = AF_INET;
	sock->remotePort = port;
	sock->remote_addr.sin_port = htons(sock->remotePort);
#ifdef _WIN32
    sock->remote_addr.sin_addr.S_un.S_addr=inet_addr(remoteIp);
#else
	sock->remote_addr.sin_addr.s_addr = inet_addr(remoteIp);
#endif
	sock->notRemoteInit=(port==0?yangtrue:yangfalse);

}


void yang_rtcudp_update_remoteAddr2(YangRtcSocketSession *sock,uint32_t remoteIp,int32_t port){

	yang_trace("\nremoteIp=%s,port=%d",remoteIp,port);
	sock->remote_addr.sin_family = AF_INET;
	sock->remotePort = port;
	sock->remote_addr.sin_port = htons(sock->remotePort);
#ifdef _WIN32
    sock->remote_addr.sin_addr.S_un.S_addr=remoteIp;
#else
	sock->remote_addr.sin_addr.s_addr = remoteIp;
#endif
	sock->notRemoteInit=(port==0?yangtrue:yangfalse);

}


int32_t yang_create_rtcudp(YangRtcSocket *psock, YangSocketProtocol protocol,int32_t plocalPort) {
	if (psock == NULL)		return ERROR_RTC_UDP;
	YangRtcSocketSession* sock=&psock->session;

	sock->localPort = plocalPort;
	sock->fd = -1;
	sock->socketProtocol=protocol;

	sock->local_addr.sin_family = AF_INET;
	sock->local_addr.sin_port = htons(sock->localPort);

	sock->local_addr.sin_addr.s_addr = INADDR_ANY;

	sock->fd = yang_socket_create(protocol);


	sock->notRemoteInit=yangtrue;

	psock->start=yang_start_rtcudp;
	psock->stop=yang_stop_rtcudp;
	psock->sendData=yang_rtc_sendData;
	psock->updateRemoteAddress=yang_rtcudp_update_remoteAddr;
	psock->updateRemoteAddress2=yang_rtcudp_update_remoteAddr2;
	return Yang_Ok;

}

void yang_destroy_rtcudp(YangRtcSocket *sock) {
    if (sock == NULL)        return;
    yang_stop_rtcudp(&sock->session);
    if (sock->session.fd > 0) {
    	yang_socket_close(sock->session.fd);
        sock->session.fd = -1;
    }
    yang_free(sock->session.turnBuffer);

}
