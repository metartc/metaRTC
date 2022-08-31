//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangsrt/YangSrtBase.h>

#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x602
#endif
#include <WS2tcpip.h>
//#pragma comment(lib,"Ws2_32.lib")
#else
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "string.h"

#include "yangutil/sys/YangLog.h"

#define POLLING_TIME 1 /// Time in milliseconds between interrupt check
#define TS_UDP_LEN 188*7

using namespace std;

#define HAVE_GETTIMEOFDAY 1

int64_t sls_gettime(void) //rturn micro-second
		{
#ifndef _WIN32
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (int64_t) tv.tv_sec * 1000000 + tv.tv_usec;
#else
	FILETIME ft;
	int64_t t;
	GetSystemTimeAsFileTime(&ft);
	t = (int64_t) ft.dwHighDateTime << 32 | ft.dwLowDateTime;
	return t / 10 - 11644473600000000; // Jan 1, 1601

	//return -1;
#endif
}
int64_t sls_gettime_ms(void) //rturn millisecond
		{
	return sls_gettime() / 1000;
}

void YangSrtBase::loadLib() {
	yang_srt_startup = (int32_t (*)(void)) m_lib.loadFunction("srt_startup");
	;
	yang_srt_setloglevel = (void (*)(int32_t ll)) m_lib.loadFunction(
			"srt_setloglevel");

	yang_srt_create_socket = (SRTSOCKET (*)(void)) m_lib.loadFunction(
			"srt_create_socket");
	yang_srt_setsockopt = (int32_t (*)(SRTSOCKET u, int32_t level, SRT_SOCKOPT optname,
			const void *optval, int32_t optlen)) m_lib.loadFunction(
			"srt_setsockopt");
	yang_srt_setsockflag = (int32_t (*)(SRTSOCKET u, SRT_SOCKOPT opt,
			const void *optval, int32_t optlen)) m_lib.loadFunction(
			"srt_setsockflag");
	yang_srt_connect = (int32_t (*)(SRTSOCKET u, const struct sockaddr *name,
			int32_t namelen)) m_lib.loadFunction("srt_connect");
	yang_srt_epoll_create = (int32_t (*)(void)) m_lib.loadFunction(
			"srt_epoll_create");
	yang_srt_epoll_set =
			(int32_t (*)(int32_t eid, int32_t flags)) m_lib.loadFunction(
					"srt_epoll_set");
	yang_srt_epoll_add_usock =
			(int32_t (*)(int32_t eid, SRTSOCKET u, const int32_t *events)) m_lib.loadFunction(
					"srt_epoll_add_usock");
	yang_srt_epoll_remove_usock =
			(int32_t (*)(int32_t eid, SRTSOCKET u)) m_lib.loadFunction(
					"srt_epoll_remove_usock");
	yang_srt_epoll_release = (int32_t (*)(int32_t eid)) m_lib.loadFunction(
			"srt_epoll_release");
	yang_srt_close = (int32_t (*)(SRTSOCKET u)) m_lib.loadFunction("srt_close");
	yang_srt_getsockstate =
			(SRT_SOCKSTATUS (*)(SRTSOCKET u)) m_lib.loadFunction(
					"srt_getsockstate");
	yang_srt_cleanup = (int32_t (*)(void)) m_lib.loadFunction("srt_cleanup");
	yang_srt_epoll_wait = (int32_t (*)(int32_t eid, SRTSOCKET *readfds, int32_t *rnum,
			SRTSOCKET *writefds, int32_t *wnum, int64_t msTimeOut, SYSSOCKET *lrfds,
			int32_t *lrnum, SYSSOCKET *lwfds, int32_t *lwnum)) m_lib.loadFunction(
			"srt_epoll_wait");
	yang_srt_sendmsg = (int32_t (*)(SRTSOCKET u, const char *buf, int32_t len,
			int32_t ttl/* = -1*/, int32_t inorder/* = false*/)) m_lib.loadFunction(
			"srt_sendmsg");
	yang_srt_recvmsg =
			(int32_t (*)(SRTSOCKET u, char *buf, int32_t len)) m_lib.loadFunction(
					"srt_recvmsg");
	yang_srt_getlasterror = (int32_t (*)(int32_t *errno_loc)) m_lib.loadFunction(
			"srt_getlasterror");
	yang_srt_getlasterror_str = (const char* (*)(void)) m_lib.loadFunction(
			"srt_getlasterror_str");
}

void YangSrtBase::unloadLib() { //srt_cleanup
	yang_srt_startup = NULL;
	yang_srt_getsockstate = NULL;
	yang_srt_setloglevel = NULL;
	yang_srt_create_socket = NULL;
	yang_srt_setsockopt = NULL;
	yang_srt_setsockflag = NULL;
	yang_srt_connect = NULL;
	yang_srt_epoll_create = NULL;
	yang_srt_epoll_set = NULL;
	yang_srt_epoll_add_usock = NULL;
	yang_srt_epoll_remove_usock = NULL;
	yang_srt_epoll_release = NULL;
	yang_srt_close = NULL;
	yang_srt_cleanup = NULL;
	yang_srt_epoll_wait = NULL;
	yang_srt_sendmsg = NULL;
	yang_srt_recvmsg = NULL;
	yang_srt_getlasterror = NULL;
	yang_srt_getlasterror_str = NULL;

}
YangSrtBase::YangSrtBase() {
	m_port = 9000;
	m_sfd = -1;
	m_eid = -1;
	m_dataCount = 0;
	m_beginTm = 0;
	m_bitRate = 0;
	m_errState = 0;
	m_contextt = 0;
	memset(m_server, 0, YangSrtBase_strlen);
	unloadLib();
}

YangSrtBase::~YangSrtBase() {
	closeSrt();
	yang_srt_cleanup();
	unloadLib();
	m_lib.unloadObject();
}

void YangSrtBase::closeSrt() {
	if (!m_contextt)
		return;
	int32_t st = 0;

	if (m_eid > 0) {
		st = yang_srt_epoll_remove_usock(m_eid, m_sfd);
		yang_srt_epoll_release(m_eid);

	}
	//srt_close
	if (m_sfd > 0) {
		st = yang_srt_close(m_sfd);
	}
	if (st == SRT_ERROR) {
		yang_error("srt_close: %s", yang_srt_getlasterror_str());
		return;
	}
	m_sfd = 0;
	//srt_cleanup();

}
int32_t YangSrtBase::getSrtSocketStatus() {
	return yang_srt_getsockstate(m_sfd);
}
int32_t YangSrtBase::init(char *pserver, int32_t pport) {
	if (m_contextt)
		return Yang_Ok;
#ifdef _WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD(2, 2); //create 16bit data
	if (WSAStartup(wVersionRequested, &wsaData) != 0) {
		printf("Load WinSock Failed!");
		exit(1);
		return ERROR_SOCKET;
	}
#endif

	m_lib.loadObject("libsrt");
	loadLib();
	strcpy(m_server, pserver);
	m_port = pport;
	yang_srt_startup();
	yang_srt_setloglevel(srt_logging::LogLevel::debug);

	m_errState = Yang_Ok;
	m_contextt = 1;
	return Yang_Ok;
}
#ifdef _WIN32
int32_t YangSrtBase::yang_inet_pton(int32_t af, const char *src, void *dst) {
	struct sockaddr_storage ss;
	int32_t ssSize = sizeof(ss);
	char srcCopy[INET6_ADDRSTRLEN + 1];

	ZeroMemory(&ss, sizeof(ss));

	// work around non-const API
	strncpy(srcCopy, src, INET6_ADDRSTRLEN + 1);
	srcCopy[INET6_ADDRSTRLEN] = '\0';

	if (WSAStringToAddressA(srcCopy, af, NULL, (struct sockaddr*) &ss, &ssSize)
			!= 0) {
		return 0;
	}

	switch (af) {
	case AF_INET: {
		*(struct in_addr*) dst = ((struct sockaddr_in*) &ss)->sin_addr;
		return 1;
	}
	case AF_INET6: {
		*(struct in6_addr*) dst = ((struct sockaddr_in6*) &ss)->sin6_addr;
		return 1;
	}
	default: {
		// No-Op
	}
	}

	return 0;
}
#endif
int32_t YangSrtBase::initConnect(char *streamid) {
	m_sfd = yang_srt_create_socket();
	if (m_sfd == SRT_ERROR) {
		yang_error("srt_socket: %s", yang_srt_getlasterror_str());
		return ERROR_SRT_SocketConnectCreate;
	}
	int32_t no = 0; //,m_is_write=1;
	// srt_setsockflag(m_sfd, SRTO_SENDER, &m_is_write, sizeof m_is_write);
#ifdef _WIN32
	//int32_t mss = 1052;
	//yang_srt_setsockopt(m_sfd, 0, SRTO_MSS, &mss, sizeof(int));
#endif
	yang_srt_setsockopt(m_sfd, 0, SRTO_SNDSYN, &no, sizeof no); // for async write
	yang_srt_setsockopt(m_sfd, 0, SRTO_RCVSYN, &no, sizeof no);
	if (yang_srt_setsockopt(m_sfd, 0, SRTO_STREAMID, streamid, strlen(streamid))
			< 0) {
		yang_error(
				"[%p]CSLSRelay::open, srt_setsockopt SRTO_STREAMID failure. err=%s.",
				this, yang_srt_getlasterror_str());
		return ERROR_SRT_StreamIdSetFailure;
	}

	//	int32_t minversion = SRT_VERSION_FEAT_HSv5;
	//	srt_setsockflag(m_sfd, SRTO_MINVERSION, &minversion, sizeof minversion);

	// Require also non-live message mode.
	int32_t file_mode = SRTT_LIVE;   //SRTT_FILE;
	//	int32_t yes = 1;
	yang_srt_setsockflag(m_sfd, SRTO_TRANSTYPE, &file_mode, sizeof file_mode);
	//	srt_setsockflag(m_sfd, SRTO_MESSAGEAPI, &yes, sizeof yes);

	addrinfo hints, *res;
	char portstr[10] = { 0 };
	snprintf(portstr, sizeof(portstr), "%d", m_port);
	memset(&hints, 0, sizeof(addrinfo));

	hints.ai_socktype = SOCK_DGRAM; //SOCK_STREAM;
	hints.ai_family = AF_INET;
	struct sockaddr_in sa;
	sa.sin_port = htons(m_port);
	sa.sin_family = AF_INET; //AF_UNSPEC;
	getaddrinfo(m_server, portstr, &hints, &res);
#ifdef _WIN32
	if (yang_inet_pton(AF_INET, (const char*) m_server, &sa.sin_addr) != 1) {
		return ERROR_SRT_SocketConnect;
	}
#else
	if (inet_pton(AF_INET, (const char*) m_server, &sa.sin_addr) != 1) {
		return ERROR_SRT_SocketConnect;
	}
#endif
	//srt_connect
	int32_t st = yang_srt_connect(m_sfd, (struct sockaddr*) &sa, sizeof sa);
	SRT_SOCKSTATUS status = yang_srt_getsockstate(m_sfd);
	yang_info("srt connect status===%d", status);
	if (st == SRT_ERROR) {
		yang_error("srt_connect: %s", yang_srt_getlasterror_str());
		return ERROR_SRT_SocketConnect;
	}
//#ifndef _WIN32
	m_eid = yang_srt_epoll_create();
	if (m_eid < 0) {
		yang_error("work, srt_epoll_create failed.");
		return ERROR_SRT_EpollCreateFailure;
		// return CSLSSrt::libsrt_neterrno();
	}
	//compatible with srt v1.4.0 when container is empty.
	yang_srt_epoll_set(m_eid, SRT_EPOLL_ENABLE_EMPTY);
	int32_t modes = SRT_EPOLL_IN | SRT_EPOLL_OUT | SRT_EPOLL_ERR;

	int32_t ret = yang_srt_epoll_add_usock(m_eid, m_sfd, &modes);
	if (ret < 0) {
		yang_error(
				"srt_add_to_epoll, srt_epoll_add_usock failed, m_eid=%d, fd=%d, modes=%d.",
				m_eid, m_sfd, modes);
		return ERROR_SRT_EpollSetFailure;	//libsrt_neterrno();
	}
//#endif
	return Yang_Ok;
}
int32_t YangSrtBase::connectServer() {
	int32_t srtRet = 0;
	for (int32_t i = 0; i < 500; i++) {
		srtRet = getSrtSocketStatus();
		if (srtRet == SRTS_CONNECTED) {
			return Yang_Ok;
		}
		yang_usleep(1000);
	}

	srtRet += Yang_SRTS_SocketBase;
	if (srtRet == Yang_SRTS_CONNECTING)		srtRet = Yang_SRTS_NONSRTSERVER;
	return yang_error_wrap(srtRet,"srt connect error");

}
int32_t YangSrtBase::receive(char *szData, int32_t *plen) {
	if (yang_srt_getsockstate(m_sfd) != SRTS_CONNECTED)
		return ERROR_SRT_NotInit;
	//char szData[TS_UDP_LEN];
	*plen = 0;
	int32_t ret = 0;
//#ifndef _WIN32
	SRTSOCKET read_socks[1];
	SRTSOCKET write_socks[1];

	int32_t read_len = 1;
	int32_t write_len = 0;
	ret = yang_srt_epoll_wait(m_eid, read_socks, &read_len, write_socks,
			&write_len, POLLING_TIME, 0, 0, 0, 0);

	if (ret < 2) {
		//yang_error("srt_epoll failure, n=%s.", yang_srt_getlasterror_str());
		return Yang_Ok;	//ERROR_SRT_EpollSelectFailure;
	}
	if (0 >= read_socks[0]) {
		//yang_error("srt_reader failure, n=%s.", yang_srt_getlasterror_str());
		return Yang_Ok;	//ERROR_SRT_ReadSocket;
	}
//#endif
	//read data
	ret = yang_srt_recvmsg(m_sfd, szData, TS_UDP_LEN);

	// if(ret>0) printf("a%d,",*plen);

	if (ret == SRT_ERROR) {
		// int32_t err_no = srt_getlasterror(NULL);
		if (getSrtSocketStatus() == SRTS_CONNECTED)
			return Yang_Ok;
		yang_error("read_data_handler, srt_read failure, errno=%d...err=%s",
				yang_srt_getlasterror(NULL), yang_srt_getlasterror_str());
		return ERROR_SRT_PullFailure;
	}
	*plen = ret;

	m_dataCount += ret;
	int64_t cur_tm = sls_gettime_ms();
	int32_t d = cur_tm - m_beginTm;
	if (d >= 500) {
		m_bitRate = m_dataCount * 8 / d;
		m_dataCount = 0;
		m_beginTm = sls_gettime_ms();
	}
	return Yang_Ok;

}

int32_t YangSrtBase::publish(char *message, int32_t len) {
//#ifndef _WIN32
	if (yang_srt_getsockstate(m_sfd) != SRTS_CONNECTED)
		return ERROR_SRT_NotInit;
	SRTSOCKET read_socks[1];
	SRTSOCKET write_socks[1];
	int32_t read_len = 0;
	int32_t write_len = 1;

	int32_t ret = yang_srt_epoll_wait(m_eid, read_socks, &read_len, write_socks,
			&write_len, POLLING_TIME, 0, 0, 0, 0);
	if (0 > ret) {

		return Yang_Ok;
	}
	if (0 >= write_socks[0]) {
		//yang_error("srt_write failure, n=%s.", yang_srt_getlasterror_str());
		return Yang_Ok;
	}
//#endif
	//write data
	int32_t n = yang_srt_sendmsg(m_sfd, message, len, -1, 0);
	if (n == SRT_ERROR) {
		yang_error("srt_write failure, n=%d.", n);

		return ERROR_SRT_PushFailure;

	}


	return Yang_Ok;
}
