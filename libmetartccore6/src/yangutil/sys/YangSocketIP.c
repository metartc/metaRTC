//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangutil/sys/YangSocket.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>

#else
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#endif


#include <errno.h>
#include<stdlib.h>
#include<yangutil/yang_unistd.h>
#include <yangutil/sys/YangLog.h>


void yang_getIp( char* domain, char* ip)
{
	struct hostent *host = gethostbyname(domain);
	if (host == NULL){

		strcpy(ip,domain);
		return;
	}
	for (int i = 0; host->h_addr_list[i]; i++)
	{
		//ip=inet_ntoa(*(struct in_addr*)host->h_addr_list[i]);
		strcpy(ip,inet_ntoa(*(struct in_addr*)host->h_addr_list[i]));
		break;
	}

}


#ifdef _WIN32

int32_t yang_getLocalInfo(char* ip){
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(wVersionRequested, &wsaData) != 0)
		return 1;
	char local[255] = {0};
	gethostname(local, sizeof(local));
	struct hostent* ph = gethostbyname(local);
	if (ph == NULL)
		return 1;

	for(int32_t i=0;;i++)
	{
		char* localIP = inet_ntoa(*(IN_ADDR*)ph->h_addr_list[i]);
		if(strstr(localIP,"127.0.0")||memcmp(localIP,"169.",4)==0||strstr(localIP,"192.168.56.")) continue;
		strcpy(ip,localIP);
		if(ph->h_addr_list[i]+ph->h_length >= ph->h_name)                break;
		break;
	}

	WSACleanup();

	return Yang_Ok;

}


int32_t yang_getLocalInfoList(YangStringVector* vecs){
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(wVersionRequested, &wsaData) != 0)
		return 1;
	char local[255] = {0};
	gethostname(local, sizeof(local));
	struct hostent* ph = gethostbyname(local);
	if (ph == NULL)
		return 1;

	for(int32_t i=0;;i++)
	{
		char* localIP = inet_ntoa(*(IN_ADDR*)ph->h_addr_list[i]);
		if(strstr(localIP,"127.0.0")) continue;
		yang_insert_stringVector(vecs,localIP);
		// strcpy(ip,localIP);
		if(ph->h_addr_list[i]+ph->h_length >= ph->h_name)                break;
		//  break;
	}

	WSACleanup();

	return Yang_Ok;

}

#else


int32_t yang_getLocalInfo(char* ip)
{
	int32_t fd;
	int32_t interfaceNum = 0;
	struct ifreq buf[16];
	struct ifconf ifc;
	int32_t res=1;
	sprintf(ip,"127.0.0.1");
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		yang_error("socket error");

		close(fd);

		return res;
	}

	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = (caddr_t)buf;
	if (!ioctl(fd, SIOCGIFCONF, (char *)&ifc))
	{
		interfaceNum = ifc.ifc_len / sizeof(struct ifreq);
		while (interfaceNum-- > 0)
		{
			//ignore the interface that not up or not runing
			//get the IP of this interface

			if (!ioctl(fd, SIOCGIFADDR, (char *)&buf[interfaceNum]))
			{
				char* s=(char *)inet_ntoa(((struct sockaddr_in *)&(buf[interfaceNum].ifr_addr))->sin_addr);

				if(memcmp(s,"127",3)!=0){
					strcpy(ip,s);
				}

			}
			else
			{
				yang_error("ioctl: %s [%s:%d]\n", strerror(errno), __FILE__, __LINE__);
				close(fd);
				return res;
			}
		}
	}
	else
	{
		yang_error("ioctl: %s [%s:%d]\n", strerror(errno), __FILE__, __LINE__);
		close(fd);
		return res;
	}

	close(fd);

	return Yang_Ok;
}


int32_t yang_getLocalInfoList(YangStringVector* vecs){
	int32_t fd;
	int32_t interfaceNum = 0;
	struct ifreq buf[16];
	struct ifconf ifc;
	int32_t res=1;

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		yang_error("socket error");
		close(fd);
		return res;
	}

	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = (caddr_t)buf;
	if (!ioctl(fd, SIOCGIFCONF, (char *)&ifc))
	{
		interfaceNum = ifc.ifc_len / sizeof(struct ifreq);
		while (interfaceNum-- > 0)
		{
			//ignore the interface that not up or not runing
			//get the IP of this interface

			if (!ioctl(fd, SIOCGIFADDR, (char *)&buf[interfaceNum]))
			{
				char* s=(char *)inet_ntoa(((struct sockaddr_in *)&(buf[interfaceNum].ifr_addr))->sin_addr);

				if(memcmp(s,"127",3)!=0){
					yang_insert_stringVector(vecs,s);
					//strcpy(ip,s);
				}

			}
			else
			{
				yang_error("ioctl: %s [%s:%d]\n", strerror(errno), __FILE__, __LINE__);
				close(fd);
				return res;
			}
		}
	}
	else
	{
		yang_error("ioctl: %s [%s:%d]\n", strerror(errno), __FILE__, __LINE__);
		close(fd);
		return res;
	}

	close(fd);

	return Yang_Ok;

}

#endif
