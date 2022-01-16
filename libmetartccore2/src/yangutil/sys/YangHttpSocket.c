/*
 * YangUserDataConnection.cpp
 *
 *  Created on: 2020年9月1日
 *      Author: yang
 */
#include <yangutil/sys/YangCHttpSocket.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangCString.h>
#include <string.h>
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
#include <errno.h>
#include "stddef.h"
#include <stdlib.h>
#include <yangutil/yang_unistd.h>
#include "fcntl.h"
#define GetSockError()	errno
#define SetSockError(e)	errno = e

#define closesocket(s)	close(s)

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
	int32_t socketfd;
	struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);
	serverAddress.sin_addr.s_addr = inet_addr(ip);
	socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	int32_t on = 1;

	setsockopt(socketfd, IPPROTO_TCP, TCP_NODELAY, (char*) &on, sizeof(on));
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 800000;  // 200 ms
	setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, (const char*) &tv,
			sizeof(struct timeval));

	if (connect(socketfd, (struct sockaddr*) &serverAddress,
			sizeof(struct sockaddr)) == -1) {
		closesocket(socketfd);
		return yang_error_wrap(ERROR_SOCKET, "http connect socket error(%d)",
				GetSockError());
	}
	int err = 1;
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
	int32_t  recvAllLen = 0;
	int32_t recvtimes=0;
	int32_t contentLen=0;
	char contentLenStr[20];
	int32_t contetSize=sizeof(Yang_Http_Content);
	char* p=NULL;
	int32_t ind=-1;
	while (true) {
		memset(buf, 0, Yang_Http_Buffer);
		nBytes = recv(socketfd, (char*) buf, Yang_Http_Buffer, 0);

		if (nBytes > 0) {
			if(recvtimes==0){
				recvtimes++;
				p=strstr(buf, "200 OK");
				if(p){
					p=strstr(buf,Yang_Http_Content);
					int ind=yang_cstr_userfindindex(p,'\r');

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
							if (recvAllLen >= contentLen) {
								err = Yang_Ok;
								break;
							}
							recvtimes++;
						}
						if(strstr(buf,"sessionid")) goto success;
						continue;
					}
				}
				break;
			}else{
				recvAllLen+=nBytes;
				if(recvtimes==1){
					recvtimes++;
					ind = yang_httpsocket_getIndex(buf, nBytes - 1);
					if(ind>-1){
						memcpy(rets + recvLen, buf + ind, nBytes - ind);
						recvLen += nBytes - ind;
						if(strstr(buf,"sessionid")) goto success;
						continue;
					}
				}else{
					recvtimes++;
					memcpy(rets + recvLen, buf , nBytes );
					recvLen += nBytes ;
					if(strstr(buf,"sessionid")) goto success;
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
	closesocket(socketfd);
	yang_free(buf);
	return 1;

	success:
	closesocket(socketfd);
	yang_free(buf);
	return Yang_Ok;

}

