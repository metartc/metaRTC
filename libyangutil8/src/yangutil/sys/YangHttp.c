//
// Copyright (c) 2019-2026 yanggaofeng
//
#include <yangutil/sys/YangCString.h>
#include <yangutil/sys/YangSocket.h>
#include <yangutil/sys/YangLog.h>

#include <errno.h>
#include <yangutil/sys/YangHttp.h>

#define Yang_Http_Content (char*)"Content-Length:"
#define Yang_Http_Buffer 1024*12

static int32_t yang_http_content_length(char *buf) {
	int32_t numberIndex;
	int32_t contentLength=0;
	char line[64];

	if(yang_get_line(buf,line,sizeof(line))!=Yang_Ok)
		return contentLength;

	numberIndex=yang_cstr_isnumber(line,sizeof(line));
	if(numberIndex>-1&&numberIndex<sizeof(line)){
		contentLength=yang_atoi(line+numberIndex);
	}

	return contentLength;
}


int32_t yang_http_post(yangbool isWhip,YangIpFamilyType familyType,char *rets, char *ip, int32_t port, char *api,
		uint8_t *data, int32_t plen) {
	int32_t err=1;
	int32_t len,nBytes,recvLen,contentPos;
	int32_t recvtimes,contentLen,headerLen,sockerr;

	yang_socket_t socketfd=-1;

	char *buf;
	char* p=NULL;
	char* headerp;
	const char *s = "POST /%s HTTP/1.1\r\n"
			"Host: %s:%d\r\n"
			"Accept: */*\r\n"
			"Content-Type: application/%s\r\n"
			"Content-Length: %u\r\n"
			"\r\n%s";
	char line[32];
	YangIpAddress serverAddress;

	yang_addr_set(&serverAddress,ip,port,familyType,Yang_Socket_Protocol_Tcp);
	socketfd = yang_socket_create(familyType,Yang_Socket_Protocol_Tcp);

	if (yang_socket_connect(socketfd, &serverAddress) == -1) {
		yang_socket_close(socketfd);
		return yang_error_wrap(ERROR_SOCKET, "http connect socket error(%d)",
				GetSockError());
	}

	buf = (char*) yang_malloc(Yang_Http_Buffer);

	yang_memset(buf, 0, Yang_Http_Buffer);

	len = yang_sprintf(buf, s, api, ip, port,isWhip?"sdp":"json;charset=UTF-8", plen, data);

	nBytes = yang_socket_send(socketfd, buf, len);

	if (nBytes < 1) {
		yang_socket_close(socketfd);
		return yang_error_wrap(ERROR_SOCKET, "http send server fail!");
	}

	recvLen = 0;
	recvtimes=0;
	contentLen=0;
	headerLen=0;

	while (yangtrue) {
		yang_memset(buf, 0, Yang_Http_Buffer);
		nBytes = yang_socket_recv(socketfd, (char*) buf, Yang_Http_Buffer, 0);

		if (nBytes > 0) {
			yang_memcpy(rets + recvLen, buf, nBytes);
			recvLen += nBytes;
			if(recvtimes==0){
				yang_memset(line,0,sizeof(line));

				if(yang_get_line(buf,line,sizeof(line))!=Yang_Ok)
					break;

				if(yang_strstr(line, "HTTP")==NULL)
					break;

				if(yang_strstr(line,"200")||yang_strstr(line,"201"))
					err=Yang_Ok;
			}

			recvtimes++;

			if(contentLen==0){
				p=yang_strstr(rets,Yang_Http_Content);
				if(p)
					contentLen=yang_http_content_length(p);
			}

			if(headerLen==0){
				headerp=yang_strstr(rets,"\r\n\r\n");
				if(headerp==NULL)
					continue;

				if (headerp) {
					contentPos = headerp - rets;
					if (contentPos > 0)
						headerLen = contentPos + 4;
				}

				if(yang_strstr(headerp+4,"\r\n\r\n"))
					goto cleanup;

			}else{
				if(yang_strstr(buf,"\r\n\r\n"))
					goto cleanup;

			}

			if(contentLen&&recvLen >= headerLen+contentLen)
				goto cleanup;

			continue;

		} else if (nBytes == -1) {
			sockerr = GetSockError();
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
	cleanup:
	yang_socket_close(socketfd);
	yang_free(buf);
	return err;

}

