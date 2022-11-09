//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangutil/sys/YangSocket.h>


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

int yang_socket_close(yang_socket_t fd){
#ifdef _WIN32
	closesocket(fd);
#else
	close(fd);
#endif

	return Yang_Ok;
}

int yang_socket_listen(yang_socket_t fd,int32_t nc){
	return listen(fd,nc);
}

int yang_socket_bind(yang_socket_t fd,yang_socket_addr* addr){
	return bind(fd, (const struct sockaddr*) addr,sizeof(struct sockaddr_in));
}

int yang_socket_connect(yang_socket_t fd,yang_socket_addr* remoteAddr){
	return connect(fd,(struct sockaddr*) remoteAddr,sizeof(struct sockaddr));
}
int yang_socket_recvfrom(yang_socket_t fd,char* buffer,int32_t bufferLen,yang_socket_addr* addr,socklen_t* addrLen){
	return recvfrom(fd, buffer, bufferLen, 0,	(struct sockaddr*) addr, addrLen);
}

int yang_socket_sendto(yang_socket_t fd,char* data,int32_t nb,yang_socket_addr* remote_addr){
	return sendto(fd, data, nb, 0, (struct sockaddr*) remote_addr,sizeof(struct sockaddr));
}

int yang_socket_send(yang_socket_t fd,char* data,int32_t nb,int32_t flag){
	return send(fd, data, nb, flag);
}

int yang_socket_recv(yang_socket_t fd,char* data,int32_t nb,int32_t flag){
	return recv(fd, data, nb, flag);
}

int yang_socket_accept(yang_socket_t fd,yang_socket_addr* addr,socklen_t* addrLen){
	return accept(fd, (struct sockaddr*)addr, addrLen);
}

int yang_socket_setsockopt_tcpdelay(yang_socket_t fd,yangbool isDelay){
	int32_t on = isDelay;

	return setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*) &on, sizeof(on));
}


int yang_socket_setsockopt_timeout(yang_socket_t fd,int ptimeoutMs){
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




