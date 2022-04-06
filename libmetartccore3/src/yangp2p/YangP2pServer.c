//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangp2p/YangP2pServer.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/yang_unistd.h>
#include <yangutil/sys/YangCString.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define Yang_Http_Content (char*)"Content-Length:"
#define Yang_Http_Buffer 1024*12
#define Yang_Http_Option "HTTP/1.1 200 OK\r\nAllow: OPTIONS, GET, HEAD, POST\r\nAccess-Control-Allow-Origin: *\r\nAccess-Control-Allow-Headers: content-type,x-requested-with,Content-Length\r\nServer: metaRTC\r\nContent-Length: 0\r\n\r\n"
#ifndef _WIN32
#include <stddef.h>
#define GetSockError()	errno
#define SetSockError(e)	errno = e

#define closesocket(s)	close(s)
#else
#define GetSockError()	WSAGetLastError()
#define SetSockError(e)	WSASetLastError(e)
#define setsockopt(a,b,c,d,e)	(setsockopt)(a,b,c,(const char *)d,(int)e)

#endif
extern int32_t yang_httpsocket_getIndex(char *buf, int plen);

int32_t yang_create_p2pserver(YangP2pServer *p2p, int32_t listenPort) {
	if (p2p == NULL)
		return 1;

	p2p->serverfd = -1;
	p2p->serverPort = listenPort;
	p2p->local_addr.sin_family = AF_INET;
	p2p->local_addr.sin_port = htons(listenPort);
#ifdef _WIN32
	p2p->local_addr.sin_addr.S_un.S_addr=INADDR_ANY;
#else
	p2p->local_addr.sin_addr.s_addr = INADDR_ANY;
#endif
	return Yang_Ok;

}
void yang_destroy_p2pserver(YangP2pServer *p2p) {
    if (p2p == NULL)
        return;
    yang_stop_p2pserver(p2p);
    if (p2p->serverfd > 0) {
        closesocket(p2p->serverfd);
        p2p->serverfd = -1;
    }
}

void* yang_run_http_thread(void *obj){
	char* buf=(char*)calloc(1024*16,1);
	char* rets=(char*)calloc(1024*16,1);
	char* answer=(char*)calloc(1024*16,1);
	const char* sdp_prefix="\"sdp\":\"";
	char* tmp=NULL;
	char* tmp2=NULL;
	YangP2pServer* p2p=(YangP2pServer*)obj;
	int connfd=p2p->connFd;
	char remoteIp[32]={0};
	strcpy(remoteIp,p2p->remoteIp);

	int32_t nBytes =0;

	int32_t  recvLen = 0;
	int32_t  recvAllLen = 0;
	int32_t recvtimes=0;
	int32_t contentLen=0;
	char contentLenStr[20];
	int32_t contetSize=sizeof(Yang_Http_Content);
	char* p=NULL;
	int32_t ind=-1;

	int32_t headerLen=0;
	int32_t hasContentLen=1;
	while (true) {
		memset(buf, 0, Yang_Http_Buffer);
		nBytes = recv(connfd, (char*) buf, Yang_Http_Buffer, 0);

		if (nBytes > 0) {
			recvAllLen+=nBytes;
			if(recvtimes==0){
				recvtimes++;
				p=strstr(buf, "HTTP");
				if(p){
					p=strstr(buf,Yang_Http_Content);
					if(p==NULL) hasContentLen=0;
					if(hasContentLen){
						int32_t ind=yang_cstr_userfindindex(p,'\r');
						char* headerp=strstr(buf,"\r\n\r\n");
						if (headerp) {
							int32_t contentPos = headerp - buf;
							if (contentPos > 0) 	headerLen = contentPos + 4;
						}
						if(ind>contetSize){
							memset(contentLenStr,0,sizeof(contentLenStr));
							memcpy(contentLenStr,p+contetSize,ind-contetSize);
							int numberIndex=yang_cstr_isnumber(contentLenStr,sizeof(contentLenStr));
							if(numberIndex>-1&&numberIndex<sizeof(contentLenStr)){
								contentLen=atoi(contentLenStr+numberIndex);
							}
							ind = yang_httpsocket_getIndex(buf, nBytes - 1);
							if(ind>-1){
								memcpy(rets + recvLen, buf + ind, nBytes - ind);
								recvLen += nBytes - ind;
								recvtimes++;
							}

							if (recvAllLen >= headerLen+contentLen) goto success;
							continue;
						}
					}else{
						memcpy(rets , buf , nBytes );
						recvLen += nBytes;
                        if (strstr(buf,"}")) goto success;
                        recvtimes++;
						continue;
					}
				}
				break;
			}else{

				if(recvtimes==1){
					recvtimes++;
					ind = yang_httpsocket_getIndex(buf, nBytes - 1);
					if(ind>-1){
						memcpy(rets + recvLen, buf + ind, nBytes - ind);
						recvLen += nBytes - ind;

						if (hasContentLen) {
							if(recvAllLen >= headerLen+contentLen) goto success;
						}else{
							if (strstr(buf,"}")) goto success;
						}
						continue;
					}
				}else{
					recvtimes++;
					memcpy(rets + recvLen, buf , nBytes );
					recvLen += nBytes ;
					if (hasContentLen) {
						if(recvAllLen >= headerLen+contentLen) goto success;
					}else{
						if (strstr(buf,"}")) goto success;
					}
					continue;
				}

			}

			break;
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
	yang_free(buf);
	yang_free(rets);
	yang_free(answer);
	return NULL;
	/*option:
	p=Yang_Http_Option;
	nBytes = send(connfd, p, strlen(p), 0);
	yang_sleep(1);
	yang_free(buf);
	yang_free(rets);
	yang_free(answer);
	yang_trace("\n send option http response.............");
	return NULL;*/
	success:
	tmp=strstr(rets,sdp_prefix);
	tmp2=strstr(tmp,"\"}");
	memset(buf, 0, Yang_Http_Buffer);
	memcpy(buf,tmp+strlen(sdp_prefix),tmp2-tmp-strlen(sdp_prefix));
	memset(rets, 0, Yang_Http_Buffer);
	yang_cstr_replace(buf,rets, "\\r\\n", "\n");
	p2p->receive(rets, recvLen, answer,remoteIp,p2p->user);
	nBytes = send(connfd, answer, strlen(answer), 0);
    yang_sleep(1);
	closesocket(connfd);
	yang_free(buf);
	yang_free(rets);
	yang_free(answer);
	return NULL;
}

void* yang_run_p2pserver_thread(void *obj) {
#ifdef _WIN32
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD(2, 2);
    WSAStartup(wVersionRequested, &wsaData);
#endif
	YangP2pServer *p2p = (YangP2pServer*) obj;
	p2p->isStart = 1;
	p2p->serverfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#ifdef _WIN32
    int32_t timeout=200;
    setsockopt(p2p->serverfd, SOL_SOCKET, SO_RCVTIMEO, (const char*) &timeout,  sizeof(timeout));
#else
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 200000;  // 200 ms
	setsockopt(p2p->serverfd, SOL_SOCKET, SO_RCVTIMEO, (const char*) &tv,
			sizeof(struct timeval));
#endif
	yang_trace("\n tcp server is starting..listenPort==%d", p2p->serverPort);

	if (bind(p2p->serverfd, (struct sockaddr*) &p2p->local_addr,sizeof(struct sockaddr_in)) < 0) {
		yang_error("http server bind error(%d)",GetSockError());
		exit(1);
	}
	listen(p2p->serverfd, 5);

	p2p->isLoop = 1;
	socklen_t src_len = sizeof(struct sockaddr_in);


	while (p2p->isLoop) {
		struct sockaddr_in src;
		memset(&src, 0, src_len);
		int connfd = accept(p2p->serverfd, (struct sockaddr*) &src, &src_len);
		if(connfd>-1){
			pthread_t th;
			p2p->connFd=connfd;
			memset(p2p->remoteIp,0,sizeof(p2p->remoteIp));
			inet_ntop(AF_INET,&src.sin_addr, p2p->remoteIp, sizeof(p2p->remoteIp));
			pthread_create(&th, 0, yang_run_http_thread, p2p);
		}
	}
	p2p->isStart = 0;
    closesocket(p2p->serverfd);
	p2p->serverfd = -1;

	return NULL;
}
void yang_start_p2pserver(YangP2pServer *p2p) {
	if (p2p == NULL)	return;
	if (pthread_create(&p2p->threadId, 0, yang_run_p2pserver_thread, p2p)) {
		yang_error("YangThread::start could not start thread");
	}
}
void yang_stop_p2pserver(YangP2pServer *p2p) {
	if (p2p == NULL)
		return;
	p2p->isLoop = 0;
	if(p2p->serverfd>0)    closesocket(p2p->serverfd);
	while (p2p->isStart)
		yang_usleep(1000);
}
