//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangutil/sys/YangSocket.h>
#include <yangutil/sys/YangLog.h>
#include <errno.h>

#if Yang_OS_WIN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Iphlpapi.h>
#pragma comment(lib,"Iphlpapi")

#else
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <ifaddrs.h>

#endif

yangbool yang_socket_filterIp(char* ip){
    if(yang_memcmp(ip,"127.0.0",7)==0 || yang_memcmp(ip,"169.",4)==0 || yang_memcmp(ip,"192.168.56.",11)==0)
        return yangfalse;

    return yangtrue;
}


int32_t yang_getIp( YangIpFamilyType familyType,char* domain, char* ip)
{

    struct addrinfo *addinfo=NULL,*addr=NULL;

    if(getaddrinfo(domain, NULL, NULL, &addinfo)!=0) {
        yang_strcpy(ip,domain);
        goto cleanup;
    }

    for (addr = addinfo; addr != NULL; addr = addr->ai_next) {
        if (addr->ai_family == AF_INET) {
            if(familyType==Yang_IpFamilyType_IPV4)
                inet_ntop(AF_INET, &((struct sockaddr_in*) addr->ai_addr)->sin_addr, ip, INET_ADDRSTRLEN);

            goto cleanup;
        } else if (addr->ai_family == AF_INET6) {

            if(familyType==Yang_IpFamilyType_IPV6)
                inet_ntop(AF_INET6, &((struct sockaddr_in6*) addr->ai_addr)->sin6_addr, ip, INET6_ADDRSTRLEN);

           goto cleanup;
        }
    }
    cleanup:
    if(addinfo) freeaddrinfo(addinfo);
    return Yang_Ok;
}


int32_t yang_getLocalInfoList(YangIpFamilyType familyType,YangStringVector* vecs)
{

    char ip[128];
#if Yang_OS_WIN
    DWORD ret, outBufLen;
    IP_ADAPTER_ADDRESSES *adapterAddresses=NULL, *adress  = NULL;
    PIP_ADAPTER_UNICAST_ADDRESS uaddress = NULL;
    ret=GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, NULL, &outBufLen);
    adapterAddresses=(IP_ADAPTER_ADDRESSES*) yang_calloc(outBufLen,1);
    ret= GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, adapterAddresses, &outBufLen);

    for (adress = adapterAddresses; adress != NULL ; adress = adress->Next) {

        for (uaddress = adress->FirstUnicastAddress; uaddress != NULL; uaddress = uaddress->Next) {

            yang_memset(ip,0,sizeof(ip));
            if (uaddress->Address.lpSockaddr->sa_family == AF_INET) {
                if(familyType==Yang_IpFamilyType_IPV4){
                    inet_ntop(AF_INET, &((struct sockaddr_in*) (uaddress->Address.lpSockaddr))->sin_addr, ip, INET_ADDRSTRLEN);
                    yang_insert_stringVector(vecs,ip);
                }

            } else {
                struct sockaddr_in6* ip6Addr = ((struct sockaddr_in6*) (uaddress->Address.lpSockaddr));
                if (IN6_IS_ADDR_UNSPECIFIED(&ip6Addr->sin6_addr) || IN6_IS_ADDR_LINKLOCAL(&ip6Addr->sin6_addr) ||
                        IN6_IS_ADDR_SITELOCAL(&ip6Addr->sin6_addr)) {
                    continue;
                }
                if(familyType==Yang_IpFamilyType_IPV6){
                    inet_ntop(AF_INET6, &ip6Addr->sin6_addr, ip, INET6_ADDRSTRLEN);
                    yang_insert_stringVector(vecs,ip);

                }

            }
        }
    }

    yang_free(adapterAddresses);

#else
    struct ifaddrs * address=NULL;
    struct ifaddrs * ifAddr=NULL;
    getifaddrs(&address);
    ifAddr=address;
    while (ifAddr!=NULL)
    {
    	if(ifAddr->ifa_addr==NULL){
        		ifAddr=ifAddr->ifa_next;
        		continue;
        	}
        if((ifAddr->ifa_flags & IFF_LOOPBACK) == 0&&(ifAddr->ifa_flags & IFF_RUNNING) > 0){
            yang_memset(ip,0,sizeof(ip));
            if (ifAddr->ifa_addr->sa_family==AF_INET) { //ipv4
                if(familyType==Yang_IpFamilyType_IPV4){

                    inet_ntop(AF_INET, &((struct sockaddr_in*)ifAddr->ifa_addr)->sin_addr, ip, INET_ADDRSTRLEN);
                    yang_insert_stringVector(vecs,ip);
                }
            } else if (ifAddr->ifa_addr->sa_family==AF_INET6) { // ipv6
                if(familyType==Yang_IpFamilyType_IPV6){

                    inet_ntop(AF_INET6, &((struct sockaddr_in*)ifAddr->ifa_addr)->sin_addr, ip, INET6_ADDRSTRLEN);
                    yang_insert_stringVector(vecs,ip);
                }
            }

        }
        ifAddr=ifAddr->ifa_next;
    }
    if (address != NULL) {
        freeifaddrs(address);
    }
#endif
    return 0;

}

#if Yang_OS_WIN
int32_t yang_getLocalInfo(YangIpFamilyType familyType,char* ipAddress){
    int32_t err = ERROR_SOCKET;
    char ip[128];
    DWORD ret, outBufLen;
    IP_ADAPTER_ADDRESSES *adapterAddresses=NULL, *adress  = NULL;
    PIP_ADAPTER_UNICAST_ADDRESS uaddress = NULL;
    ret=GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, NULL, &outBufLen);
    adapterAddresses=(IP_ADAPTER_ADDRESSES*) yang_calloc(outBufLen,1);
    ret= GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, adapterAddresses, &outBufLen);
    yangbool isLoop=yangtrue;
    for (adress = adapterAddresses; adress != NULL&&isLoop ; adress = adress->Next) {

        for (uaddress = adress->FirstUnicastAddress; uaddress != NULL; uaddress = uaddress->Next) {
            yang_memset(ip,0,sizeof(ip));

            if (uaddress->Address.lpSockaddr->sa_family == AF_INET) {
                if(familyType==Yang_IpFamilyType_IPV4){
                    inet_ntop(AF_INET,  &((struct sockaddr_in*) (uaddress->Address.lpSockaddr))->sin_addr, ip, INET_ADDRSTRLEN);
                    if(yang_socket_filterIp(ip)){
                        yang_strcpy(ipAddress,ip);
                        err=Yang_Ok;
                        isLoop=yangfalse;
                        break;
                    }
                }

            } else {
                struct sockaddr_in6* ip6Addr = ((struct sockaddr_in6*) (uaddress->Address.lpSockaddr));
                if (IN6_IS_ADDR_UNSPECIFIED(&ip6Addr->sin6_addr) || IN6_IS_ADDR_LINKLOCAL(&ip6Addr->sin6_addr) ||
                        IN6_IS_ADDR_SITELOCAL(&ip6Addr->sin6_addr)) {
                    continue;
                }

                if(familyType==Yang_IpFamilyType_IPV6){
                    inet_ntop(AF_INET6, &ip6Addr->sin6_addr, ip, INET6_ADDRSTRLEN);
                    yang_strcpy(ipAddress,ip);
                    err=Yang_Ok;
                    isLoop=yangfalse;
                    break;
                }

            }
        }
    }

    yang_free(adapterAddresses);
    return err;
}



#else
int32_t yang_getLocalInfo(YangIpFamilyType familyType,char* ipAddress)
{
    int32_t err=ERROR_SOCKET;
    struct ifaddrs *address=NULL,*ifAddr=NULL;

    getifaddrs(&address);
    char ip[128];
    ifAddr=address;
    while (ifAddr!=NULL)
    {
    	if(ifAddr->ifa_addr==NULL){
        		ifAddr=ifAddr->ifa_next;
        		continue;
        	}
        if((ifAddr->ifa_flags & IFF_LOOPBACK) == 0&&(ifAddr->ifa_flags & IFF_RUNNING) > 0){
        	yang_memset(ip,0,sizeof(ip));
            if (ifAddr->ifa_addr->sa_family==AF_INET) { //ipv4
                if(familyType==Yang_IpFamilyType_IPV4){
                    inet_ntop(AF_INET, &((struct sockaddr_in*)ifAddr->ifa_addr)->sin_addr, ip, INET_ADDRSTRLEN);
                    if(yang_socket_filterIp(ip)){
                        yang_strcpy(ipAddress,ip);
                        err=Yang_Ok;
                        break;
                    }
                }


            } else if (ifAddr->ifa_addr->sa_family==AF_INET6) { // ipv6
                if(familyType==Yang_IpFamilyType_IPV6){
                    inet_ntop(AF_INET6, &((struct sockaddr_in*)ifAddr->ifa_addr)->sin_addr, ip, INET6_ADDRSTRLEN);
                    yang_strcpy(ipAddress,ip);
                    err=Yang_Ok;
                    break;
                }

            }

        }
        ifAddr=ifAddr->ifa_next;
    }
    if (address != NULL) {
        freeifaddrs(address);
    }
    return err;

}

#endif
