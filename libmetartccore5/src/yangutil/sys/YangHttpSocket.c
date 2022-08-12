//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangutil/sys/YangHttpSocket.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangCString.h>
#include <string.h>
#include <errno.h>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>

#define socklen_t int
#define GetSockError()	WSAGetLastError()
#define SetSockError(e)	WSASetLastError(e)
#define setsockopt(a,b,c,d,e)	(setsockopt)(a,b,c,(const char *)d,(int)e)

#define sleep(n)	Sleep(n*1000)
#define msleep(n)	Sleep(n)
#define SET_RCVTIMEO(tv,s)	int32_t tv = s*1000
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/times.h>
#include <netdb.h>
#include <yangutil/yang_unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <stddef.h>
#include <stdlib.h>
#define GetSockError()	errno
#define SetSockError(e)	errno = e

#define closesocket(s)	close(s)
#endif
#include <yangutil/yang_unistd.h>

#include <fcntl.h>


#define Yang_Http_Content (char*)"Content-Length:"
#define Yang_Http_Buffer 1024*12

int32_t yang_httpsocket_getIndex(char *buf, int plen) {
	for (int i = 0; i < plen; i++) {
		if (*(buf + i) == '{')
			return i;
	}
	return -1;
}


int32_t yang_http_post(char *rets, char *ip, int32_t port, char *api,
		uint8_t *data, int32_t plen) {
#ifdef _WIN32
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD(2, 2);
    WSAStartup(wVersionRequested, &wsaData);
#endif
	int32_t socketfd;
	struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);
#ifdef _WIN32
    serverAddress.sin_addr.S_un.S_addr=inet_addr(ip);
#else
	serverAddress.sin_addr.s_addr = inet_addr(ip);
#endif
	socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	int32_t on = 1;

	setsockopt(socketfd, IPPROTO_TCP, TCP_NODELAY, (char*) &on, sizeof(on));
#ifdef _WIN32
    int32_t timeout=800;
    setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, (const char*) &timeout,  sizeof(timeout));
#else
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 800000;  // 200 ms
	setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, (const char*) &tv,
			sizeof(struct timeval));
#endif
	if (connect(socketfd, (struct sockaddr*) &serverAddress,
			sizeof(struct sockaddr)) == -1) {
		closesocket(socketfd);
		return yang_error_wrap(ERROR_SOCKET, "http connect socket error(%d)",
				GetSockError());
	}

	char *buf = (char*) malloc(Yang_Http_Buffer);

	memset(buf, 0, Yang_Http_Buffer);
	const char *s = "POST /%s HTTP/1.1\r\n"
			"Host: %s:%d\r\n"
			"Accept: */*\r\n"
			"Content-Type:application/json;charset=UTF-8\r\n"
			"Content-Length: %u\r\n"
			"\r\n%s";
	int len = sprintf(buf, s, api, ip, port, plen, data);

	int32_t nBytes = send(socketfd, buf, len, 0);
	if (nBytes < 1) {
		close(socketfd);
		return yang_error_wrap(ERROR_SOCKET, "http send server fail!");
	}
	int32_t  recvLen = 0;

	int32_t recvtimes=0;
	int32_t contentLen=0;
	char contentLenStr[20];
	int32_t contetSize=sizeof(Yang_Http_Content);
	char* p=NULL;


	int32_t headerLen=0;
	while (true) {
		memset(buf, 0, Yang_Http_Buffer);
		nBytes = recv(socketfd, (char*) buf, Yang_Http_Buffer, 0);

		if (nBytes > 0) {
			memcpy(rets + recvLen, buf, nBytes);
			recvLen += nBytes;
			if(recvtimes==0&&strstr(buf, "HTTP")==NULL) break;
			recvtimes++;
			if(contentLen==0){
				p=strstr(rets,Yang_Http_Content);
				if(recvtimes>0&&p==NULL) break;
				if(p==NULL) continue;

				int32_t ind=yang_cstr_userfindindex(p,'\r');
				if(ind==0) continue;
				if(ind>contetSize){
					memset(contentLenStr,0,sizeof(contentLenStr));
					memcpy(contentLenStr,p+contetSize,ind-contetSize);
					int numberIndex=yang_cstr_isnumber(contentLenStr,sizeof(contentLenStr));
					if(numberIndex>-1&&numberIndex<sizeof(contentLenStr)){
						contentLen=atoi(contentLenStr+numberIndex);
					}
				}
			}
			if(headerLen==0){
				char* headerp=strstr(rets,"\r\n\r\n");
				if(headerp==NULL) continue;
				if (headerp) {
					int32_t contentPos = headerp - rets;
					if (contentPos > 0) 	headerLen = contentPos + 4;
				}
			}

			if(recvLen >= headerLen+contentLen) goto success;
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
	closesocket(socketfd);
	yang_free(buf);
	return 1;

	success:
	closesocket(socketfd);
	yang_free(buf);
	return Yang_Ok;

}

