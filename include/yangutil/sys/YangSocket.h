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
typedef struct sockaddr_in yang_socket_addr;

#ifdef __cplusplus
extern "C"{
#endif

yang_socket_t yang_socket_create(YangSocketProtocol protocol);
int yang_socket_close(yang_socket_t fd);

int yang_socket_setsockopt_timeout(yang_socket_t fd,int ptimeoutMs);
int yang_socket_setsockopt_tcpdelay(yang_socket_t fd,yangbool isDelay);

int yang_socket_bind(yang_socket_t fd,yang_socket_addr* addr);
int yang_socket_connect(yang_socket_t fd,yang_socket_addr* remoteAddr);
int yang_socket_recvfrom(yang_socket_t fd,char* buffer,int32_t bufferLen,yang_socket_addr* addr,socklen_t* addrLen);
int yang_socket_sendto(yang_socket_t fd,char* data,int32_t nb,yang_socket_addr* remote_addr);
int yang_socket_send(yang_socket_t fd,char* data,int32_t nb,int32_t flag);
int yang_socket_recv(yang_socket_t fd,char* data,int32_t nb,int32_t flag);
int yang_socket_accept(yang_socket_t fd,yang_socket_addr* addr,socklen_t* addrLen);


int32_t yang_getLocalInfo(char* p);
int32_t yang_getLocalInfoList(YangStringVector* p);
void yang_getIp( char* domain, char* ip);

#ifdef __cplusplus
}
#endif
#endif /* YANGSOCKETUTIL_H_ */
