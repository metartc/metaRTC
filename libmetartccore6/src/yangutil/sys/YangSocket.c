//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangutil/sys/YangSocket.h>
#include <yangutil/sys/YangLog.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>

#define yang_poll WSAPoll
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

#endif

#define yang_sockaddr(x) x->familyType==Yang_IpFamilyType_IPV4?(const struct sockaddr*)(&x->addr4):(const struct sockaddr*)(&x->addr6)
#define yang_sockaddr2(x) x->familyType==Yang_IpFamilyType_IPV4?(struct sockaddr*)(&x->addr4):(struct sockaddr*)(&x->addr6)
#define yang_sockaddr_len(x)  x->familyType==Yang_IpFamilyType_IPV4?sizeof(yang_socket_addr4):sizeof(yang_socket_addr6);

void yang_addr_set(YangIpAddress* addr,char* ip,int32_t port,YangIpFamilyType familyType, YangSocketProtocol protocol){
	if(addr==NULL) return;
	addr->port=port;
	addr->familyType=familyType;
	addr->protocol=protocol;
	yang_memcpy(addr->address,ip,yang_strlen(ip));
	if(familyType==Yang_IpFamilyType_IPV4){
		addr->addr4.sin_family = AF_INET;
		addr->addr4.sin_port = htons(addr->port);
#ifdef _WIN32
		addr->addr4.sin_addr.S_un.S_addr=inet_addr(ip);
#else
		addr->addr4.sin_addr.s_addr = inet_addr(ip);
#endif
	}else{
		addr->addr6.sin6_family = AF_INET6;
		addr->addr6.sin6_port = htons(addr->port);
		inet_pton(AF_INET6, ip, &addr->addr6.sin6_addr);

	}
}

void yang_addr_setAnyAddr(YangIpAddress* addr,int32_t port,YangIpFamilyType familyType, YangSocketProtocol protocol){
	if(familyType==Yang_IpFamilyType_IPV6){
		uint8_t ip[16]={0};
		yang_addr_setIPV6(addr,ip,port,protocol);
		return;
	}

		yang_addr_setIPV4(addr,0,port,protocol);

}

void yang_addr_setIPV4(YangIpAddress* addr,int32_t ip,int32_t port,YangSocketProtocol protocol){
	if(addr==NULL) return;
	addr->port=port;
	addr->familyType=Yang_IpFamilyType_IPV4;
	addr->protocol=protocol;


		addr->addr4.sin_family = AF_INET;
		addr->addr4.sin_port = htons(addr->port);
#ifdef _WIN32
		addr->addr4.sin_addr.S_un.S_addr=ip;
#else
		addr->addr4.sin_addr.s_addr = ip;
#endif

}


void yang_addr_setIPV6(YangIpAddress* addr,uint8_t ip[16],int32_t port,YangSocketProtocol protocol){
	if(addr==NULL) return;
	addr->port=port;
	addr->familyType=Yang_IpFamilyType_IPV6;
	addr->protocol=protocol;
	addr->addr6.sin6_family = AF_INET6;
	addr->addr6.sin6_port = htons(addr->port);
	yang_memcpy(&addr->addr6.sin6_addr,ip,16);

}

void yang_addr_getIPStr(YangIpAddress* addr,char* addrstr,int32_t strLen){
	if(addr->familyType == Yang_IpFamilyType_IPV4 )
		inet_ntop(AF_INET,&addr->addr4.sin_addr.s_addr, addrstr, strLen);
	else
        inet_ntop(AF_INET6,&addr->addr6.sin6_addr, addrstr, strLen);
}

uint32_t yang_addr_getIP(YangIpAddress* addr){
#ifdef _WIN32
	return addr->addr4.sin_addr.S_un.S_addr;
#else
	return addr->addr4.sin_addr.s_addr;
#endif
	//return addr->familyType == Yang_IpFamilyType_IPV4 ?addr->addr4.sin_addr.s_addr:addr->addr6.sin6_addr.__in6_u;
}

uint16_t yang_addr_getPort(YangIpAddress* addr){
	return htons(addr->familyType == Yang_IpFamilyType_IPV4?addr->addr4.sin_port:addr->addr6.sin6_port);
}

uint16_t yang_addr_getSinPort(YangIpAddress* addr){
	return addr->familyType == Yang_IpFamilyType_IPV4?addr->addr4.sin_port:addr->addr6.sin6_port;
}

yang_socket_t yang_socket_create(YangIpFamilyType familyType, YangSocketProtocol protocol){
#ifdef _WIN32
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD(2, 2);
    WSAStartup(wVersionRequested, &wsaData);
#endif

    int32_t fd=-1;
	fd = socket(familyType == Yang_IpFamilyType_IPV4 ? AF_INET : AF_INET6, protocol==Yang_Socket_Protocol_Tcp?SOCK_STREAM:SOCK_DGRAM, 0);

	if(fd==-1){
		yang_error("create socket error: %d ", GetSockError());
	}

	int32_t timeoutMs=800;
#ifdef _WIN32
    int32_t timeout=timeoutMs;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char*) &timeout,  sizeof(timeout));
#else
	struct timeval tv;
	tv.tv_sec = 0;
    tv.tv_usec = timeoutMs*1000;  //  ms
	setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char*) &tv,	sizeof(struct timeval));
#endif

	//int value = 1;
	//setsockopt(fd, SOL_SOCKET, Yang_No_Signal, &value, sizeof(value));


	if(protocol==Yang_Socket_Protocol_Tcp){
		//int sendBufSize=32*1024;
		//setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &sendBufSize, sizeof(sendBufSize));

		int32_t on = 1;
		setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*) &on, sizeof(on));
	}

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

int32_t yang_socket_listen(yang_socket_t fd,YangIpAddress* addr){
	int32_t err=Yang_Ok;
	socklen_t addrLen=yang_sockaddr_len(addr);
	if((err=bind(fd, yang_sockaddr(addr),addrLen))!=Yang_Ok){
		return yang_error_wrap(err,"socket bind error");
	}
	if(addr->protocol==Yang_Socket_Protocol_Tcp){
		return listen(fd,5);
	}
	return Yang_Ok;

}



int32_t yang_socket_connect(yang_socket_t fd,YangIpAddress* remoteAddr){
	socklen_t addrLen=yang_sockaddr_len(remoteAddr);
	return connect(fd,yang_sockaddr(remoteAddr),addrLen);
}

int32_t yang_socket_recvfrom(yang_socket_t fd,char* buffer,int32_t bufferLen,YangIpAddress* addr){
	socklen_t srcLen=yang_sockaddr_len(addr);
	return recvfrom(fd, buffer, bufferLen, 0,	yang_sockaddr2(addr), &srcLen);
}

int32_t yang_socket_sendto(yang_socket_t fd,char* data,int32_t nb,YangIpAddress* remote_addr,int32_t flag){
	socklen_t addrLen=yang_sockaddr_len(remote_addr);
	return sendto(fd, data, nb, flag, yang_sockaddr(remote_addr),addrLen);
}

int32_t yang_socket_send(yang_socket_t fd,char* data,int32_t nb,int32_t flag){
	return send(fd, data, nb, flag);
}

int32_t yang_socket_recv(yang_socket_t fd,char* data,int32_t nb,int32_t flag){
	return recv(fd, data, nb, flag);
}

int32_t yang_socket_accept(yang_socket_t fd,YangIpAddress* addr){
	socklen_t addrLen=yang_sockaddr_len(addr);
	return accept(fd, yang_sockaddr2(addr), &addrLen);
}

