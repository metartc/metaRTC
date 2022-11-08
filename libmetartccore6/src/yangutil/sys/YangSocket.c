//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangutil/sys/YangSocket.h>

//typedef struct{

//}YangSocket;


yang_socket_t yang_socket_create(YangSocketProtocol protocol){
#ifdef _WIN32
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD(2, 2);
    WSAStartup(wVersionRequested, &wsaData);
#endif
	int fd=-1;
	if(protocol==Yang_Socket_Protocol_Udp)
			fd = socket(AF_INET, SOCK_DGRAM, 0);
		else if(protocol==Yang_Socket_Protocol_Tcp)
			fd=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	return fd;
}

int yang_socket_close(int32_t fd){
#ifdef _WIN32
	closesocket(fd);
#else
	close(fd);
#endif

	return Yang_Ok;
}

int yang_socket_setsockopt_delay(int fd,yangbool isDelay){
	int32_t on = isDelay;

	return setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*) &on, sizeof(on));
}


int yang_socket_setsockopt_timeout(int fd,int ptimeoutMs){
#ifdef _WIN32
    int32_t timeout=ptimeoutMs;
    return setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char*) &timeout,  sizeof(timeout));
#else
	struct timeval tv;
	tv.tv_sec = 0;
    tv.tv_usec = ptimeoutMs*1000;  //  ms
	return setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char*) &tv,
			sizeof(struct timeval));
#endif
}




