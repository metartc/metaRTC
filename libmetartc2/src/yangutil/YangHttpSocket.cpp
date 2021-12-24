/*
 * YangUserDataConnection.cpp
 *
 *  Created on: 2020年9月1日
 *      Author: yang
 */
#include <yangutil/sys/YangHttpSocket.h>
#include "yangutil/sys/YangLog.h"
#include <string>
#include <string.h>

#ifdef _WIN32

#include <winsock2.h>
#include <ws2tcpip.h>

#define GetSockError()	WSAGetLastError()
#define SetSockError(e)	WSASetLastError(e)
#define setsockopt(a,b,c,d,e)	(setsockopt)(a,b,c,(const char *)d,(int)e)

#define sleep(n)	Sleep(n*1000)
#define msleep(n)	Sleep(n)
#define SET_RCVTIMEO(tv,s)	int32_t tv = s*1000
#else /* !_WIN32 */
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
#define GetSockError()	errno
#define SetSockError(e)	errno = e
#undef closesocket
#define closesocket(s)	close(s)
#define msleep(n)	yang_usleep(n*1000)
#define SET_RCVTIMEO(tv,s)	struct timeval tv = {s,0}
#endif
#include <yangutil/yang_unistd.h>
#include "fcntl.h"

YangHttpSocket::YangHttpSocket() {

	m_socket = 0;

}

YangHttpSocket::~YangHttpSocket() {

}

int32_t YangHttpSocket::init(char *serverip, int32_t pport) {

#ifdef _WIN32
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD(2, 2);
    WSAStartup(wVersionRequested, &wsaData);
#endif

	m_serverAddress.sin_family = AF_INET;
	m_serverAddress.sin_port = htons(pport);
#ifdef _WIN32
    m_serverAddress.sin_addr.S_un.S_addr=inet_addr(serverip);
#else
	m_serverAddress.sin_addr.s_addr = inet_addr(serverip);

#endif
	return Yang_Ok;
}

int32_t YangHttpSocket::connectServer() {
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	int32_t on = 1;

	setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, (char*) &on, sizeof(on));
#ifdef _WIN32
    struct timeval tv = { 1, 0 };
    setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (char*) &tv, sizeof(tv));
#else
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 800000;  // 200 ms
	setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*) &tv,
			sizeof(struct timeval));
#endif

	if (connect(m_socket, (sockaddr*) &m_serverAddress, sizeof(sockaddr))
			== -1) {
		closesocket(m_socket);
		return yang_error_wrap(ERROR_SOCKET, "http connect socket error(%d)",
				GetSockError());
	}

	return Yang_Ok;
}

int32_t YangHttpSocket::getIndex(char *buf, int plen) {
	for (int i = 0; i < plen; i++) {
		if (*(buf + i) == '{')
			return i;
	}
	return -1;
}
#define Yang_Http_Buffer 1024*5
int32_t YangHttpSocket::post(std::string &rets, char *ip, int32_t port,
		char *api, uint8_t *p, int32_t plen) {
	init(ip, port);
	if (connectServer()) {
		closesocket(m_socket);
		return yang_error_wrap(ERROR_SOCKET, "http connect server fail!");
	}
	int err = 1;
	char *buf = new char[Yang_Http_Buffer];
	YangAutoFreeA(char, buf);
	memset(buf, 0, Yang_Http_Buffer);
	const char *s = "POST /%s HTTP/1.1\r\n"
			"Host: %s:%d\r\n"
			"Accept: */*\r\n"
			"Content-Type:application/json;charset=UTF-8\r\n"
			"Content-Length: %u\r\n"
			"\r\n%s";
	int len = sprintf(buf, s, api, ip, port, plen, p);


	int32_t nBytes = send(m_socket, buf, len, 0);
	if (nBytes < 1)	{
		closesocket(m_socket);
		return yang_error_wrap(ERROR_SOCKET, "http send server fail!");
	}

	while (true) {
		memset(buf, 0, Yang_Http_Buffer);
		nBytes = recv(m_socket, (char*) buf, Yang_Http_Buffer, 0);
		if (nBytes > 0) {

			int ind = getIndex(buf, nBytes-1);
			if (ind > -1) {
				rets.append(buf + ind, nBytes - ind);
				err = Yang_Ok;
			}else{
				std::string str=buf;
				if(str.find("OK")!=str.npos) continue;
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
	closesocket(m_socket);

	return err;
}

