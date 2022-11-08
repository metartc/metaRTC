//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGSOCKETUTIL_H_
#define YANGSOCKETUTIL_H_
#include <stdint.h>
#include <yangutil/sys/YangVector.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>

#define socklen_t int
#define GetSockError()	WSAGetLastError()
#define SetSockError(e)	WSASetLastError(e)
#define setsockopt(a,b,c,d,e)	(setsockopt)(a,b,c,(const char *)d,(int)e)
#else

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#define GetSockError()	errno
#define SetSockError(e)	errno = e
#endif

#define yang_socket_t int
#define yang_socket_recv recv
#define yang_socket_recvfrom recvfrom
#define yang_socket_bind bind
#define yang_socket_accept accept
#define yang_socket_connect connect
#define yang_socket_send send
#define yang_socket_sendto sendto


#ifdef __cplusplus
extern "C"{
#endif

yang_socket_t yang_socket_create(YangSocketProtocol protocol);
int yang_socket_close(int32_t fd);
int yang_socket_setsockopt_timeout(int fd,int ptimeoutMs);
int yang_socket_setsockopt_delay(int fd,yangbool isDelay);

int32_t yang_getLocalInfo(char* p);
int32_t yang_getLocalInfoList(YangStringVector* p);
void yang_getIp( char* domain, char* ip);

#ifdef __cplusplus
}
#endif
#endif /* YANGSOCKETUTIL_H_ */
