//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef TRACE_YangTraceUdp_H_
#define TRACE_YangTraceUdp_H_
#ifdef _WIN32

#include <winsock2.h>


#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <stdint.h>
class YangTraceUdp
{
public:
    YangTraceUdp(void);
    ~YangTraceUdp(void);
    void init(const char *ip,int32_t port);
    int32_t sendUdp(const char* data,int32_t len);
    void setSockSrv(const char *ip,int32_t port);
private:
    int32_t sockSrv;
    struct sockaddr_in m_addrClient;
};

#endif
