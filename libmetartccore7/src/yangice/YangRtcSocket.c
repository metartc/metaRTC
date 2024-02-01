//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangice/YangRtcSocket.h>

#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangEndian.h>
#include <yangrtp/YangRtpConstant.h>

#include <errno.h>



int32_t yang_rtc_sendData(YangRtcSocketSession *session, char *data, int32_t nb) {
	if (session == NULL || !session->isStart || session->notRemoteInit || data==NULL)	return ERROR_RTC_SOCKET;
	int32_t err = Yang_Ok;
	yang_thread_mutex_lock(&session->sendLock);
	err=yang_socket_sendto(session->fd, data, nb, &session->remote_addr,0) > 0 ? Yang_Ok : ERROR_RTC_SOCKET;
	yang_thread_mutex_unlock(&session->sendLock);
	return err;
}


int32_t yang_rtc_tcp_sendData(YangRtcSocketSession *session,char *data, int32_t nb) {
	if (session == NULL || !session->isStart || session->notRemoteInit || data==NULL)	return ERROR_RTC_SOCKET;
	yang_thread_mutex_lock(&session->sendLock);
	int32_t nbytes=yang_socket_send(session->fd, data, nb);
	yang_thread_mutex_unlock(&session->sendLock);
	if(nbytes==-1) yang_error("rtc tcp send error(%d)",nbytes);
	if(nbytes!=nb) yang_error("rtc tcp send data(%d),should send data(%d)",nbytes,nb);
	return nbytes==nb ? Yang_Ok : ERROR_RTC_SOCKET;
}

#if Yang_Enable_Tcp_Srs
	int32_t yang_rtc_tcp_sendData_srs(YangRtcSocketSession *session,char *data, int32_t nb) {
		if (session == NULL || !session->isStart || session->notRemoteInit || data==NULL || session->fd==-1)	return ERROR_RTC_SOCKET;

		yang_thread_mutex_lock(&session->sendLock);
		int32_t sendBytes=2;

		yang_put_be16(session->buffer,(uint16_t)nb);

		int32_t nbytes=yang_socket_send(session->fd, session->buffer, sendBytes);

		if(nbytes==-1 ||nbytes!=sendBytes) goto fail;

		sendBytes = nb;
		nbytes=yang_socket_send(session->fd, data, sendBytes);// Yang_No_Signal) ;

		if(nbytes==-1 ||nbytes!=sendBytes) goto fail;

		yang_thread_mutex_unlock(&session->sendLock);
		return Yang_Ok;

		fail:
			yang_thread_mutex_unlock(&session->sendLock);
			if(nbytes==-1) return yang_error_wrap(ERROR_RTC_SOCKET,"rtc tcp send error(%d)",nbytes);
			if(nbytes!=sendBytes) return yang_error_wrap(ERROR_RTC_SOCKET,"rtc tcp send data(%d),should send data(%d)",nbytes,sendBytes);
			return Yang_Ok;



	}
#endif

void* yang_run_rtcudp_thread(void *obj) {
    YangRtcSocketSession *sock = (YangRtcSocketSession*) obj;

	sock->isStart = yangtrue;

	if (yang_socket_listen(sock->fd, &sock->local_addr) < 0) {

		yang_error("Udp server bind error");
		exit(1);
	}

	char buffer[2048] = { 0 };
	sock->isLoop = yangtrue;

	int32_t len = 0;
	if (sock->startStunTimer)	sock->startStunTimer(sock->user);

	yang_trace("\nrtc udp(%s) server is starting,localPort=%d",sock->familyType==Yang_IpFamilyType_IPV4?"IPV4":"IPV6", sock->local_addr.port);
	YangIpAddress src;
	yang_memset(&src, 0, sizeof(YangIpAddress));
	src.familyType=sock->familyType;
	src.protocol=Yang_Socket_Protocol_Udp;
	while (sock->isLoop) {
		yang_memset(buffer, 0, 2048);
		if ((len = yang_socket_recvfrom(sock->fd, buffer, 2048,  &src)) > 0) {

			if(sock->notRemoteInit){
				yang_memcpy((char*)&sock->remote_addr,(char*)&src,sizeof(YangIpAddress));
				sock->notRemoteInit=yangfalse;
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
    char buffer[4096] = { 0 };
	sock->isStart = yangtrue;

	if (yang_socket_connect(sock->fd, &sock->remote_addr) == -1) {
		yang_socket_close(sock->fd);
		yang_error_wrap(ERROR_SOCKET, "rtc tcp connect socket error(%d)",GetSockError());
		return NULL;
	}

	if (!sock->notRemoteInit&&sock->startStunTimer)	sock->startStunTimer(sock->user);

	yang_trace("\n rtc tcp socket(%s) is starting",sock->familyType==Yang_IpFamilyType_IPV4?"IPV4":"IPV6");
	sock->isLoop = yangtrue;
	int32_t readLen = 0;

	while (sock->isLoop) {
		yang_memset(buffer, 0, 4096);
        if ((readLen = yang_socket_recv(sock->fd, buffer, 4096, 0)) > 0) {
        		if (sock->receive)	sock->receive(buffer, readLen, sock->user);

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

	sock->isStart = yangfalse;
	yang_socket_close(sock->fd);
	sock->fd = -1;

	return NULL;
}



void* yang_run_rtctcp_thread_srs(void *obj) {

    YangRtcSocketSession *sock = (YangRtcSocketSession*) obj;
    char readBuffer[kRtpPacketSize] = { 0 };

	sock->isStart = yangtrue;

	if (yang_socket_connect(sock->fd, &sock->remote_addr) == -1) {
		yang_socket_close(sock->fd);
		yang_error_wrap(ERROR_SOCKET, "rtc tcp connect socket error(%d)",GetSockError());
		return NULL;
	}

	if (!sock->notRemoteInit&&sock->startStunTimer)	sock->startStunTimer(sock->user);

	yang_trace("\n rtc tcp socket(%s) is starting",sock->familyType==Yang_IpFamilyType_IPV4?"IPV4":"IPV6");
	sock->isLoop = yangtrue;
	int32_t readLen = 0;
	uint16_t msgLen=0;
	int32_t msgReadLen=0;
	int32_t msgReadBufferLen=0;
	yangbool recvData=yangfalse;
	int32_t headerLen=sock->headerLen;

	while (sock->isLoop) {
		if(!recvData){
			if(msgLen==0)
				msgReadLen=headerLen;
			else
				msgReadLen=msgLen;
		}
		if ((readLen = yang_socket_recv(sock->fd, readBuffer+msgReadBufferLen, msgReadLen, 0))> 0) {
			if(readLen!=msgReadLen){
				msgReadLen-=readLen;
				recvData=yangtrue;
				msgReadBufferLen+=readLen;
				continue;
			}
			msgReadBufferLen=0;
			recvData=yangfalse;
			if (msgLen == 0) {
				msgLen = yang_get_be16((uint8_t*)(readBuffer));
				if (msgLen > kRtpPacketSize) {
					yang_error("rtc tcp error pkt size=%hu", msgLen);
					msgLen = 0;
					msgReadLen = 0;
				}
				continue;
			}

			if (sock->receive)
				sock->receive(readBuffer, msgLen, sock->user);

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

	sock->isStart = yangfalse;
	yang_socket_close(sock->fd);
	sock->fd = -1;

	return NULL;
}

void* yang_run_rtctcp_thread_turn(void *obj) {

    YangRtcSocketSession *sock = (YangRtcSocketSession*) obj;
    char readBuffer[kRtpPacketSize] = { 0 };

	sock->isStart = yangtrue;

	if (yang_socket_connect(sock->fd, &sock->remote_addr) == -1) {
		yang_socket_close(sock->fd);
		yang_error_wrap(ERROR_SOCKET, "rtc tcp connect socket error(%d)",GetSockError());
		return NULL;
	}

	if (!sock->notRemoteInit&&sock->startStunTimer)	sock->startStunTimer(sock->user);

	yang_trace("\n rtc tcp socket(%s) is starting",sock->familyType==Yang_IpFamilyType_IPV4?"IPV4":"IPV6");
	sock->isLoop = yangtrue;
	int32_t readLen = 0;
	int32_t msgLen=0;
	int32_t msgReadLen=0;
	int32_t msgReadBufferLen=0;
	yangbool recvData=yangfalse;
	int32_t headerLen=sock->headerLen;
	char* p=readBuffer;

	int32_t blank=0;
	while (sock->isLoop) {
		if(!recvData){
			if(msgLen==0)
				msgReadLen=headerLen;
			else
				msgReadLen=msgLen;
		}
		if ((readLen = yang_socket_recv(sock->fd, p+msgReadBufferLen, msgReadLen, 0))> 0) {
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

			if (sock->receive)
				sock->receive(readBuffer, msgLen+headerLen-blank, sock->user);
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

		if (yang_thread_create(&sock->threadId, 0,
				sock->headerLen==2?yang_run_rtctcp_thread_srs:yang_run_rtctcp_thread_turn, sock))

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

void yang_rtcudp_update_remoteAddr(YangRtcSocketSession *sock,char* remoteIp,int32_t port){

	yang_trace("\nremoteIp=%s,port=%d",remoteIp,port);
	yang_addr_set(&sock->remote_addr,remoteIp,port,sock->familyType,sock->socketProtocol);

	sock->notRemoteInit=(port==0?yangtrue:yangfalse);

}


int32_t yang_create_rtcsocket(YangRtcSocket *psock,YangIpFamilyType familyType, YangSocketProtocol protocol,int32_t plocalPort) {
	if (psock == NULL)		return ERROR_RTC_SOCKET;
	YangRtcSocketSession* sock=&psock->session;

	sock->fd = -1;
	sock->socketProtocol=protocol;
	sock->familyType=familyType;

	sock->headerLen=0;

	yang_addr_setAnyAddr(&sock->local_addr,plocalPort,familyType,protocol);

	sock->fd = yang_socket_create(familyType,protocol);
	if(sock->fd==-1){
		return yang_error_wrap(ERROR_RTC_SOCKET,"create rtc socket fail,familtype=%s,protocol=%s",
				familyType==Yang_IpFamilyType_IPV4?"IPV4":"IPV6",protocol==Yang_Socket_Protocol_Udp?"UDP":"TCP");
	}


	sock->notRemoteInit=yangtrue;

	yang_thread_mutex_init(&sock->sendLock,NULL);

	psock->start=yang_start_rtcudp;
	psock->stop=yang_stop_rtcudp;
	if(protocol == Yang_Socket_Protocol_Udp)
		psock->write=yang_rtc_sendData;
	else
		psock->write=yang_rtc_tcp_sendData;


	psock->updateRemoteAddress=yang_rtcudp_update_remoteAddr;

	return Yang_Ok;

}

#if Yang_Enable_Tcp_Srs

int32_t yang_create_rtcsocket_srs(YangRtcSocket* psock,YangSocketProtocol protocol){
	if(protocol!=Yang_Socket_Protocol_Tcp) return Yang_Ok;
	psock->session.headerLen=2;
	psock->write=yang_rtc_tcp_sendData_srs;

	if(psock->session.buffer==NULL) psock->session.buffer=(char*)yang_calloc(2,1);
	return Yang_Ok;
}

#endif

void yang_destroy_rtcsocket(YangRtcSocket *sock) {
    if (sock == NULL)        return;
    yang_stop_rtcudp(&sock->session);
    if (sock->session.fd > 0) {
    	yang_socket_close(sock->session.fd);
        sock->session.fd = -1;
    }
    yang_free(sock->session.buffer);
    yang_thread_mutex_destroy(&sock->session.sendLock);

}
