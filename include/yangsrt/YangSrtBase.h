//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGSRTBASE_H_
#define YANGSRTBASE_H_
#include "stdint.h"
#ifdef __WIN32
#include <winsock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#endif
#include <yangutil/yang_unistd.h>
#include "srt/srt.h"
#include "yangutil/YangErrorCode.h"
#include "yangutil/sys/YangLoadLib.h"

#define YangSrtBase_strlen 128
#define YangSrtUnitLen 188
#define Yang_Srt_CacheSize 4096

class YangSrtBase {
public:
	YangSrtBase();
	virtual ~YangSrtBase();
	int32_t init(char *pserver,int32_t pport);
	int32_t publish(char* message,int32_t len);
	int32_t receive(char *p,int32_t *len);

	void startLoop();
	void closeSrt();
	int32_t initConnect(char *streamId);
	int32_t connectServer();
	int32_t getSrtSocketStatus();
#ifdef _WIN32
	int32_t yang_inet_pton(int32_t af, const char * src, void * dst);
#endif
	int64_t get_bitrate();
	int32_t m_errState;
	int32_t m_contextt;
private:
	int32_t m_port;
	int32_t m_sfd;
	int32_t m_eid;
	char m_server[YangSrtBase_strlen];
	int64_t m_dataCount;
	int64_t m_bitRate;
	int64_t m_beginTm;

	YangLoadLib m_lib;
	void loadLib();
	void unloadLib();
	int32_t (*yang_srt_epoll_create)(void);
	int32_t (*yang_srt_epoll_remove_usock)(int32_t eid, SRTSOCKET u);
	int32_t (*yang_srt_epoll_release)(int32_t eid);
	int32_t (*yang_srt_close)        (SRTSOCKET u);
	int32_t (*yang_srt_cleanup)(void);
	 int32_t (*yang_srt_startup)(void);
	 SRT_SOCKSTATUS (*yang_srt_getsockstate)(SRTSOCKET u);
	 void (*yang_srt_setloglevel)(int32_t ll);
	 SRTSOCKET (*yang_srt_create_socket)(void);
	 int32_t (*yang_srt_epoll_set)(int32_t eid, int32_t flags);
	 int32_t (*yang_srt_epoll_add_usock)(int32_t eid, SRTSOCKET u, const int* events);
	 int32_t (*yang_srt_setsockopt)   (SRTSOCKET u, int32_t level /*ignored*/, SRT_SOCKOPT optname, const void* optval, int32_t optlen);
	 int32_t (*yang_srt_setsockflag)  (SRTSOCKET u, SRT_SOCKOPT opt, const void* optval, int32_t optlen);
	 int32_t (*yang_srt_connect )     (SRTSOCKET u, const struct sockaddr* name, int32_t namelen);
	 int32_t (*yang_srt_epoll_wait)(int32_t eid, SRTSOCKET* readfds, int* rnum, SRTSOCKET* writefds, int* wnum, int64_t msTimeOut,
	                            SYSSOCKET* lrfds, int* lrnum, SYSSOCKET* lwfds, int* lwnum);
	 int32_t (*yang_srt_sendmsg) (SRTSOCKET u, const char* buf, int32_t len, int32_t ttl/* = -1*/, int32_t inorder/* = false*/);
	 int32_t (*yang_srt_recvmsg) (SRTSOCKET u, char* buf, int32_t len);
	 int32_t  (*yang_srt_getlasterror)(int* errno_loc);
	 const char* (*yang_srt_getlasterror_str)(void);
};

#endif /* YANGSRTBASE_H_ */
