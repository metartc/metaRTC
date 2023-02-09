//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangp2p/YangP2pServer.h>

#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangSocket.h>
#include <yangutil/sys/YangCString.h>

#include <errno.h>

#define Yang_Http_Content (char*)"Content-Length:"
#define Yang_Http_Buffer 1024*12
//#define Yang_Http_Option "HTTP/1.1 200 OK\r\nAllow: OPTIONS, GET, HEAD, POST\r\nAccess-Control-Allow-Origin: *\r\nAccess-Control-Allow-Headers: content-type,x-requested-with,Content-Length\r\nServer: metaRTC\r\nContent-Length: 0\r\n\r\n"



int32_t yang_create_p2pserver(YangP2pServer *p2p, int32_t listenPort) {
	if (p2p == NULL)	return ERROR_P2P_SERVER;

	p2p->serverfd = -1;
	p2p->serverPort = listenPort;
	yang_addr_setAnyAddr(&p2p->local_addr,listenPort,p2p->familyType,Yang_Socket_Protocol_Tcp);

	return Yang_Ok;

}
void yang_destroy_p2pserver(YangP2pServer *p2p) {
	if (p2p == NULL)
		return;
	yang_stop_p2pserver(p2p);
	if (p2p->serverfd > 0) {
		yang_socket_close(p2p->serverfd);
		p2p->serverfd = -1;
	}
}

void* yang_run_http_thread(void *obj){
	char* buf=(char*)yang_calloc(Yang_Http_Buffer,1);
	char* rets=(char*)yang_calloc(1024*16,1);
	char* answer=(char*)yang_calloc(1024*16,1);

	char* tmp=NULL;

	YangP2pServer* p2p=(YangP2pServer*)obj;
	int connfd=p2p->connFd;
	char remoteIp[32]={0};
	yang_strcpy(remoteIp,p2p->remoteIp);

	int32_t nBytes =0;

	int32_t  recvLen = 0;

	int32_t recvtimes=0;
	int32_t contentLen=0;
	char contentLenStr[20];
	int32_t contetSize=sizeof(Yang_Http_Content);
	char* p=NULL;
	int32_t headerLen=0;
	while (yangtrue) {
		yang_memset(buf, 0, Yang_Http_Buffer);
		nBytes = yang_socket_recv(connfd, (char*) buf, Yang_Http_Buffer, 0);

		if (nBytes > 0) {
			yang_memcpy(rets + recvLen, buf, nBytes);
			recvLen += nBytes;
			if(recvtimes==0&&yang_strstr(buf, "HTTP")==NULL) break;
			recvtimes++;
			if(headerLen==0){
				char* headerp=yang_strstr(rets,"\r\n\r\n");
				if(headerp==NULL) continue;
				if (headerp) {
					int32_t contentPos = headerp - rets;
					if (contentPos > 0) 	headerLen = contentPos + 4;
				}
				p=yang_strstr(rets,Yang_Http_Content);
				if(p&&headerp-p>0){
					int32_t ind=yang_cstr_userfindindex(p,'\r');
					if(ind>contetSize){
						yang_memset(contentLenStr,0,sizeof(contentLenStr));
						yang_memcpy(contentLenStr,p+contetSize,ind-contetSize);
						int numberIndex=yang_cstr_isnumber(contentLenStr,sizeof(contentLenStr));
						if(numberIndex>-1&&numberIndex<sizeof(contentLenStr)){
							contentLen=atoi(contentLenStr+numberIndex);
						}
					}
				}
			}

			if(contentLen>0&&recvLen >= headerLen+contentLen) 		goto success;

			if(contentLen==0&&headerLen>0&&yang_strstr(buf,"}"))		goto success;

			continue;

		} else if (nBytes == -1) {
			int32_t sockerr = GetSockError();
			if (sockerr == EINTR)
				continue;
			if (sockerr == EWOULDBLOCK || sockerr == EAGAIN) {
				nBytes = 0;
				continue;
			}
			yang_error("%s, recv returned %d. GetSockError(): %d (%s)",
					__FUNCTION__, nBytes, sockerr, strerror(sockerr));

			break;
		} else if (nBytes == 0) {

			break;
		}
		break;
	}
	yang_socket_close(connfd);
	yang_free(buf);
	yang_free(rets);
	yang_free(answer);
	return NULL;
	success:
	if(tmp==NULL) tmp=(char*)yang_calloc(1024*16,1);
	yang_cstr_replace(rets+headerLen, tmp, "\r\n","\n");
	yang_memset(rets,0,1024*16);
	yang_cstr_replace(tmp,rets,"}","");
	p2p->receive(rets, yang_strlen(rets)+1, answer,remoteIp,p2p->user);
	nBytes = yang_socket_send(connfd, answer, yang_strlen(answer));
	yang_sleep(1);
	yang_socket_close(connfd);
	yang_free(tmp);
	yang_free(buf);
	yang_free(rets);
	yang_free(answer);
	return NULL;
}

void* yang_run_p2pserver_thread(void *obj) {

	YangP2pServer *p2p = (YangP2pServer*) obj;
	p2p->isStart = yangtrue;
	p2p->serverfd = yang_socket_create(p2p->familyType,Yang_Socket_Protocol_Tcp);


	yang_trace("\nhttp tcp server is starting,listenPort==%d", p2p->serverPort);

	if (yang_socket_listen(p2p->serverfd,  &p2p->local_addr) < 0) {
		yang_error("http server bind error(%d)",GetSockError());
		exit(1);
	}


	p2p->isLoop = yangtrue;

	while (p2p->isLoop) {
		YangIpAddress src;
		yang_memset(&src, 0, sizeof(YangIpAddress));
		src.familyType=p2p->familyType;
		src.protocol=Yang_Socket_Protocol_Tcp;
		int connfd = yang_socket_accept(p2p->serverfd,  &src);
		if(connfd>-1){
			yang_thread_t th;
			p2p->connFd=connfd;
			yang_memset(p2p->remoteIp,0,sizeof(p2p->remoteIp));
			yang_addr_getIPStr(&src,p2p->remoteIp, sizeof(p2p->remoteIp));

			yang_thread_create(&th, 0, yang_run_http_thread, p2p);
		}
	}
	p2p->isStart = yangfalse;
	yang_socket_close(p2p->serverfd);
	p2p->serverfd = -1;
	return NULL;
}
void yang_start_p2pserver(YangP2pServer *p2p) {
	if (p2p == NULL)	return;
	if (yang_thread_create(&p2p->threadId, 0, yang_run_p2pserver_thread, p2p)) {
		yang_error("YangThread::start could not start thread");
	}
}
void yang_stop_p2pserver(YangP2pServer *p2p) {
	if (p2p == NULL)
		return;
	p2p->isLoop = yangfalse;
	if(p2p->serverfd>0)    yang_socket_close(p2p->serverfd);
	while (p2p->isStart)
		yang_usleep(1000);
}
