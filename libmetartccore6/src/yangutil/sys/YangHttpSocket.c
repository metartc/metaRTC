//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangutil/sys/YangHttpSocket.h>
#include <yangutil/sys/YangCString.h>
#include <yangutil/sys/YangSocket.h>
#include <yangutil/sys/YangLog.h>

#include <errno.h>

#define Yang_Http_Content (char*)"Content-Length:"
#define Yang_Http_Buffer 1024*12

int32_t yang_httpsocket_getIndex(char *buf, int plen) {
	for (int i = 0; i < plen; i++) {
		if (*(buf + i) == '{')
			return i;
	}
	return -1;
}


int32_t yang_http_post(yangbool isWhip,YangIpFamilyType familyType,char *rets, char *ip, int32_t port, char *api,
		uint8_t *data, int32_t plen) {

	yang_socket_t socketfd=-1;
	YangIpAddress serverAddress;
	yang_addr_set(&serverAddress,ip,port,familyType,Yang_Socket_Protocol_Tcp);
	socketfd = yang_socket_create(familyType,Yang_Socket_Protocol_Tcp);

	if (yang_socket_connect(socketfd, &serverAddress) == -1) {
		yang_socket_close(socketfd);
		return yang_error_wrap(ERROR_SOCKET, "http connect socket error(%d)",
				GetSockError());
	}

	char *buf = (char*) yang_malloc(Yang_Http_Buffer);

	yang_memset(buf, 0, Yang_Http_Buffer);
	const char *s = "POST /%s HTTP/1.1\r\n"
			"Host: %s:%d\r\n"
			"Accept: */*\r\n"
			"Content-Type:application/%s;charset=UTF-8\r\n"
			"Content-Length: %u\r\n"
			"\r\n%s";
	int32_t len = yang_sprintf(buf, s, api, ip, port,isWhip?"sdp":"json", plen, data);

	int32_t nBytes = yang_socket_send(socketfd, buf, len);

	if (nBytes < 1) {
		yang_socket_close(socketfd);
		return yang_error_wrap(ERROR_SOCKET, "http send server fail!");
	}
	int32_t  recvLen = 0;

	int32_t recvtimes=0;
	int32_t contentLen=0;
	char contentLenStr[20];
	int32_t contetSize=sizeof(Yang_Http_Content);
	char* p=NULL;


	int32_t headerLen=0;
	while (yangtrue) {
		yang_memset(buf, 0, Yang_Http_Buffer);
		nBytes = yang_socket_recv(socketfd, (char*) buf, Yang_Http_Buffer, 0);

		if (nBytes > 0) {
			yang_memcpy(rets + recvLen, buf, nBytes);
			recvLen += nBytes;
			if(recvtimes==0&&yang_strstr(buf, "HTTP")==NULL) break;
			recvtimes++;
			if(contentLen==0){
				p=yang_strstr(rets,Yang_Http_Content);
				if(recvtimes>0&&p==NULL) break;
				if(p==NULL) continue;

				int32_t ind=yang_cstr_userfindindex(p,'\r');
				if(ind==0) continue;
				if(ind>contetSize){
					yang_memset(contentLenStr,0,sizeof(contentLenStr));
					yang_memcpy(contentLenStr,p+contetSize,ind-contetSize);
					int numberIndex=yang_cstr_isnumber(contentLenStr,sizeof(contentLenStr));
					if(numberIndex>-1&&numberIndex<sizeof(contentLenStr)){
						contentLen=atoi(contentLenStr+numberIndex);
					}
				}
			}
			if(headerLen==0){
				char* headerp=yang_strstr(rets,"\r\n\r\n");
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
	yang_socket_close(socketfd);
	yang_free(buf);
	return 1;

	success:
	yang_socket_close(socketfd);
	yang_free(buf);
	return Yang_Ok;

}

