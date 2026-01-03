//
// Copyright (c) 2019-2026 yanggaofeng
//

#include <yangice/YangRtcSocket.h>

#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangEndian.h>
#include <yangutil/yangrtptype.h>
#include <yangrtc/YangRtcSession.h>

#include <errno.h>

int32_t yang_rtcsock_sendData(YangRtcSocketSession *session, char *data, int32_t nb) {
	int32_t err = Yang_Ok;
	if (session == NULL || session->fd == -1 || session->notRemoteInit || data==NULL)
		return ERROR_RTC_SOCKET;

	yang_thread_mutex_lock(&session->sendLock);
	err=yang_socket_sendto(session->fd, data, nb, &session->remote_addr,0) > 0 ? Yang_Ok : ERROR_RTC_SOCKET;
	yang_thread_mutex_unlock(&session->sendLock);

	return err;
}
int32_t yang_rtcsock_sendData2(YangRtcSocketSession *session, YangIpAddress* remote_addr,char *data, int32_t nb) {
	int32_t err = Yang_Ok;

	if (session == NULL || session->fd == -1 || data==NULL)
		return ERROR_RTC_SOCKET;

	yang_thread_mutex_lock(&session->sendLock);
	err=yang_socket_sendto(session->fd, data, nb, remote_addr,0) > 0 ? Yang_Ok : ERROR_RTC_SOCKET;
	yang_thread_mutex_unlock(&session->sendLock);

	return err;
}

int32_t yang_rtcsock_tcp_sendData(YangRtcSocketSession *session,char *data, int32_t nb) {
	int32_t nbytes = 0;

	if (session == NULL || session->fd == -1  || session->notRemoteInit || data==NULL)
		return ERROR_RTC_SOCKET;

	yang_thread_mutex_lock(&session->sendLock);
	nbytes=yang_socket_send(session->fd, data, nb);
	yang_thread_mutex_unlock(&session->sendLock);
	if(nbytes==-1) yang_error("rtc tcp send error(%d)",nbytes);
	if(nbytes!=nb) yang_error("rtc tcp send data(%d),should send data(%d)",nbytes,nb);
	return nbytes==nb ? Yang_Ok : ERROR_RTC_SOCKET;
}

 int32_t yang_rtcsock_tcp_read(YangRtcSocketSession *session, char *buffer, int32_t bufferLen,int32_t* nbytes){
	 int32_t readLen=0;
	 while(yangtrue){
		 readLen = yang_socket_recv(session->fd, buffer, bufferLen, 0);

		 if (readLen > 0) {
			 *nbytes=readLen;
			 return Yang_Ok;
		 }else if (readLen == -1) {
			 int32_t sockerr = GetSockError();
			 if (sockerr == EINTR || sockerr == EWOULDBLOCK || sockerr == EAGAIN)	continue;

			 yang_error("%s, recv returned %d. GetSockError(): %d (%s)",
					 __FUNCTION__, readLen, sockerr, strerror(sockerr));

			 return ERROR_RTC_SOCKET;
		 }
	 }
	 return Yang_Ok;
 }

 int32_t yang_rtcsock_udp_read(YangRtcSocketSession *session, char *buffer, int32_t bufferLen,int32_t* nbytes){
	 int32_t readLen=0;
	 if ((readLen = yang_socket_recvfrom(session->fd, buffer, bufferLen, 0)) > 0) {
		 *nbytes=readLen;
		 return Yang_Ok;
	 		}else if (readLen == -1) {
	 			int32_t sockerr = GetSockError();
	 			if (sockerr == EINTR)
	 				return ERROR_RTC_SOCKET;
	 			if (sockerr == EWOULDBLOCK || sockerr == EAGAIN) {
	 				readLen = 0;
	 				return ERROR_RTC_SOCKET;
	 			}
	 			yang_error("%s, recv returned %d. GetSockError(): %d (%s)",
	 					__FUNCTION__, readLen, sockerr, strerror(sockerr));

	 			return ERROR_RTC_SOCKET;
	 		}
	 return Yang_Ok;
}

void* yang_run_rtcudp_thread(void *obj) {
    YangRtcSocketSession *session = (YangRtcSocketSession*) obj;
    YangRtcSession* rtcSession=(YangRtcSession*)session->rtcSession;
	int32_t nbytes = 0;
	char buffer[2048] = { 0 };
	//char ip[64]={0};
	YangIpAddress iceServer={0};
	YangIpAddress src;
	session->income_addr = &src;
	if (!session->binded&&yang_socket_listen(session->fd, &session->local_addr) < 0) {

		yang_error("Udp server bind error");
		return NULL;
	}

	session->isStart = yangtrue;
	session->isLoop = yangtrue;

	if (!session->notRemoteInit&&session->startStunTimer)
		session->startStunTimer(session->user);

	yang_trace("\nrtc udp(%s) server is starting,localPort=%d",session->familyType==Yang_IpFamilyType_IPV4?"IPV4":"IPV6", yang_addr_getPort(&session->local_addr));

	yang_memset(&src, 0, sizeof(YangIpAddress));
	src.familyType=session->familyType;

	while (session->isLoop) {
		yang_memset(buffer, 0, 2048);
		if ((nbytes = yang_socket_recvfrom(session->fd, buffer, 2048,  &src)) > 0) {

			if(buffer[0]==0x01&&buffer[1]==0x01&&session->rtcSession){

				if(session->stunRequesting){
					yang_memset(&iceServer,0,sizeof(YangIpAddress));
					yang_addr_set(&iceServer,rtcSession->ice.session.server.serverIp,rtcSession->ice.session.server.serverPort,session->familyType,session->socketProtocol);
					if(yang_addr_cmp(&src,&iceServer)){
						rtcSession->ice.onStunResponse(&rtcSession->ice.session,buffer,nbytes);
						continue;
					}
				}else{
					if(rtcSession->ice.session.server.serverPort==yang_addr_getPort(&src))
							continue;
				}
			}

			if(session->notRemoteInit){
				if(!rtcSession->ice.isStunRtp(&rtcSession->ice.session,buffer, nbytes))
					continue;
				yang_addr_copy(&src,&session->remote_addr,session->familyType);
				session->notRemoteInit = yangfalse;
				if(session->updateAddress) session->updateAddress(session->user,&src);

			}

			if (session->receive)	session->receive(buffer, nbytes, session->user);
		}
	}

	session->isStart = yangfalse;
	yang_socket_close(session->fd);
	session->fd = -1;

	return NULL;
}


void* yang_run_rtctcp_thread(void *obj) {

    YangRtcSocketSession *session = (YangRtcSocketSession*) obj;
    int32_t readLen = 0;
    char buffer[4096] = { 0 };
	session->isStart = yangtrue;

	if (yang_socket_connect(session->fd, &session->remote_addr) == -1) {
		yang_socket_close(session->fd);
		yang_error_wrap(ERROR_SOCKET, "rtc tcp connect socket error(%d)",GetSockError());
		return NULL;
	}

	if (!session->notRemoteInit&&session->startStunTimer)	session->startStunTimer(session->user);

	yang_trace("\n rtc tcp socket(%s) is starting",session->familyType==Yang_IpFamilyType_IPV4?"IPV4":"IPV6");
	session->isLoop = yangtrue;

	while (session->isLoop) {
		yang_memset(buffer, 0, 4096);
        if ((readLen = yang_socket_recv(session->fd, buffer, 4096, 0)) > 0) {
        		if (session->receive)	session->receive(buffer, readLen, session->user);

		}else if (readLen == -1) {
			int32_t sockerr = GetSockError();
			if (sockerr == EINTR)
				continue;
			if (sockerr == EWOULDBLOCK || sockerr == EAGAIN) {
				readLen = 0;
				continue;
			}
			yang_error("%s, recv returned %d. GetSockError(): %d (%s)",
					__FUNCTION__, readLen, sockerr, strerror(sockerr));

			break;
		}
	}

	session->isStart = yangfalse;
	yang_socket_close(session->fd);
	session->fd = -1;

	return NULL;
}

void* yang_run_rtctcp_thread_turn(void *obj) {

    YangRtcSocketSession *session = (YangRtcSocketSession*) obj;
	int32_t readLen = 0;
	int32_t msgLen=0;
	int32_t msgReadLen=0;
	int32_t msgReadBufferLen=0;
	yangbool recvData=yangfalse;
	int32_t headerLen=session->headerLen;
    char readBuffer[kRtpPacketSize] = { 0 };
    char* p=readBuffer;
    int32_t blank=0;

	session->isStart = yangtrue;

	if (yang_socket_connect(session->fd, &session->remote_addr) == -1) {
		yang_socket_close(session->fd);
		yang_error_wrap(ERROR_SOCKET, "rtc tcp connect socket error(%d)",GetSockError());
		return NULL;
	}

	if (!session->notRemoteInit&&session->startStunTimer)	session->startStunTimer(session->user);

	yang_trace("\n rtc tcp socket(%s) is starting",session->familyType==Yang_IpFamilyType_IPV4?"IPV4":"IPV6");
	session->isLoop = yangtrue;


	while (session->isLoop) {
		if(!recvData){
			if(msgLen==0)
				msgReadLen=headerLen;
			else
				msgReadLen=msgLen;
		}
		if ((readLen = yang_socket_recv(session->fd, p+msgReadBufferLen, msgReadLen, 0))> 0) {
			if(readLen!=msgReadLen){
				msgReadLen-=readLen;
				recvData=yangtrue;
				msgReadBufferLen+=readLen;
				continue;
			}
			msgReadBufferLen=0;
			recvData=yangfalse;
			if (msgLen == 0) {
				if(yang_get_be32((uint8_t*) readBuffer + 4) == 0x2112A442){
					msgLen = yang_get_be16((uint8_t*)readBuffer+2)+12;
					blank=0;
				}else{
					msgLen = (int32_t)yang_get_be16((uint8_t*)readBuffer+2);
					msgLen-=4;
					blank=4-msgLen%4;
					if(blank<4) msgLen+=blank;
				}


				if (msgLen > kRtpPacketSize) {
					yang_error("rtc tcp error pkt size=%d", msgLen);
					msgLen = 0;
					msgReadLen = 0;
					p=readBuffer;
					continue;
				}
				p=readBuffer+headerLen;
				continue;
			}

			if (session->receive)
				session->receive(readBuffer, msgLen+headerLen-blank, session->user);
			p=readBuffer;
			msgLen = 0;
		}else if (readLen == -1) {
			int32_t sockerr = GetSockError();
			if (sockerr == EINTR)
				continue;
			if (sockerr == EWOULDBLOCK || sockerr == EAGAIN) {
				readLen = 0;
				continue;
			}
			yang_error("%s, recv returned %d. GetSockError(): %d (%s)",
					__FUNCTION__, readLen, sockerr, strerror(sockerr));

			break;
		}
	}

	session->isStart = yangfalse;
	yang_socket_close(session->fd);
	session->fd = -1;

	return NULL;
}

void yang_start_rtcudp(YangRtcSocketSession *sock) {
	if (sock == NULL || sock->isStart)
		return;

	if(sock->socketProtocol==Yang_Socket_Protocol_Udp){
		if (yang_thread_create(&sock->threadId, 0, yang_run_rtcudp_thread, sock)) {
			yang_error("YangThread::start could not start thread");
		}
	}else if(sock->socketProtocol==Yang_Socket_Protocol_Tcp){

		if (yang_thread_create(&sock->threadId, 0,yang_run_rtctcp_thread_turn, sock))

		{
				yang_error("YangThread::start could not start thread");
		}


	}
}


void yang_stop_rtcudp(YangRtcSocketSession *sock) {
	if (sock == NULL)	return;
	sock->isLoop = yangfalse;
	while (sock->isStart)	yang_usleep(1000);
}

void yang_rtcsock_update_remoteAddr(YangRtcSocketSession *sock,char* remoteIp,int32_t port){

	yang_trace("\nremoteIp=%s,port=%d",remoteIp,port);
	yang_addr_set(&sock->remote_addr,remoteIp,port,sock->familyType,sock->socketProtocol);

	sock->notRemoteInit=(port==0?yangtrue:yangfalse);

}

int32_t yang_rtcsock_connect(YangRtcSocketSession* sock){
	return yang_socket_connect(sock->fd, &sock->remote_addr);
}

int32_t yang_rtcsock_listen(YangRtcSocketSession* sock){
	if(sock==NULL)
		return ERROR_RTC_SOCKET;

	if (yang_socket_listen(sock->fd, &sock->local_addr) < 0) {
		char ip[64]={0};
		yang_addr_getIPStr(&sock->local_addr,ip,sizeof(ip));
		yang_error("Udp server (%s:%hu) bind error",ip,yang_addr_getPort(&sock->local_addr));
		return ERROR_RTC_SOCKET;
	}
	sock->binded=yangtrue;
	return Yang_Ok;
}

int32_t yang_create_rtcsocket(YangRtcSocket *sock,YangIpFamilyType familyType, YangSocketProtocol protocol,int32_t plocalPort) {
	YangRtcSocketSession* session = NULL;
	if (sock == NULL)
		return ERROR_RTC_SOCKET;
	session=&sock->session;

	session->fd = -1;
	session->socketProtocol=protocol;
	session->familyType=familyType;

	session->headerLen=0;

	session->binded=yangfalse;
	session->income_addr = NULL;
	yang_addr_setAnyAddr(&session->local_addr,plocalPort,familyType,protocol);

	session->fd = yang_socket_create(familyType,protocol);
	if(session->fd==-1){
		return yang_error_wrap(ERROR_RTC_SOCKET,"create rtc socket fail,familtype=%s,protocol=%s",
				familyType==Yang_IpFamilyType_IPV4?"IPV4":"IPV6",protocol==Yang_Socket_Protocol_Udp?"UDP":"TCP");
	}

	session->notRemoteInit=yangtrue;

	yang_thread_mutex_init(&session->sendLock,NULL);


	if(protocol == Yang_Socket_Protocol_Udp){
		sock->write=yang_rtcsock_sendData;
		sock->write2 = yang_rtcsock_sendData2;
		sock->read=yang_rtcsock_udp_read;
	}else{
		sock->write=yang_rtcsock_tcp_sendData;
		sock->read=yang_rtcsock_tcp_read;
	}
	sock->start=yang_start_rtcudp;
	sock->stop=yang_stop_rtcudp;
	sock->updateRemoteAddress=yang_rtcsock_update_remoteAddr;
	sock->connect=yang_rtcsock_connect;
	sock->listen=yang_rtcsock_listen;


	return Yang_Ok;

}

void yang_destroy_rtcsocket(YangRtcSocket *sock) {
    if (sock == NULL)
    	return;

    yang_stop_rtcudp(&sock->session);
    if (sock->session.fd > 0) {
    	yang_socket_close(sock->session.fd);
        sock->session.fd = -1;
    }
    yang_free(sock->session.buffer);
    yang_thread_mutex_destroy(&sock->session.sendLock);

}
