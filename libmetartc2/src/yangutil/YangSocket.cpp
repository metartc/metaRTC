#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#else
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#endif

#include <yangutil/sys/YangSocket.h>
#include <errno.h>
#include<stdlib.h>
#include<yangutil/yang_unistd.h>
#include <yangutil/sys/YangLog.h>

using namespace std;
void yang_getIp( std::string domain, std::string& ip)
{
#ifdef _WIN32
    WORD wVersionRequested = MAKEWORD(2, 2);
    WSADATA wsaData;
    WSAStartup(wVersionRequested, &wsaData);
#endif
	struct hostent *host = gethostbyname(domain.c_str());
	if (host == NULL){
		ip=domain;
 #ifdef _WIN32
        WSACleanup();
#endif
		return;
	}
	for (int i = 0; host->h_addr_list[i]; i++)
	{
		ip=inet_ntoa(*(struct in_addr*)host->h_addr_list[i]);
		break;
	}
 #ifdef _WIN32
	 WSACleanup();
#endif
	//ip=domain;
}


#ifdef _WIN32
void SplitString2Int( const string& src, string delimit, vector<int32_t>& vecRet)
{
    string null_subst = "0";
    if( src.empty() || src == "" || delimit.empty() || delimit == "" )
        return;

    uint32_t deli_len = delimit.size();
    uint32_t nIndex = 0;
    uint32_t last_search_position = 0;
    while( ( nIndex = src.find( delimit, last_search_position ) ) != -1 )
    {
        if( nIndex == last_search_position )
        {
            int32_t nValue = atoi( null_subst.c_str() );
            vecRet.push_back( nValue );
        }
        else
        {
            int32_t nValue = atoi( src.substr( last_search_position, nIndex - last_search_position ).c_str() );
            vecRet.push_back( nValue );
        }
        last_search_position = nIndex + deli_len;
    }

    string last_one = src.substr( last_search_position );
    if ( last_one.empty() )
    {
        int32_t nValue = atoi( null_subst.c_str() );
        vecRet.push_back( nValue );
    }
    else
    {
        int32_t nValue = atoi( last_one.c_str() );
        vecRet.push_back( nValue );
    }
}

std::string yang_getLocalInfo(void){
    string localIP = "";

        WORD wVersionRequested = MAKEWORD(2, 2);
        WSADATA wsaData;
        if (WSAStartup(wVersionRequested, &wsaData) != 0)
            return "";
        char local[255] = {0};
        gethostname(local, sizeof(local));
        hostent* ph = gethostbyname(local);
        if (ph == NULL)
            return "";

        for(int32_t i=0;;i++)
        {
            localIP = inet_ntoa(*(IN_ADDR*)ph->h_addr_list[i]);
            std::vector<int32_t> vecRet;
            SplitString2Int( localIP, ".", vecRet );
            if(vecRet[0] == 127 && vecRet[1] == 0 && vecRet[2] == 0)	//获得192.168.0.X最后一位
                continue;
            if(ph->h_addr_list[i]+ph->h_length >= ph->h_name)                break;
            break;
        }
        //in_addr addr;
        //memcpy(&addr, ph->h_addr_list[0], sizeof(in_addr)); // 这里仅获取第一个ip
        //localIP.assign(inet_ntoa(addr));
        WSACleanup();

        return localIP;

 }
#else
std::string yang_getLocalInfo(void)
{
    int32_t fd;
    int32_t interfaceNum = 0;
    struct ifreq buf[16];
    struct ifconf ifc;
    struct ifreq ifrcopy;
    char mac[16] = {0};
    char ip[32] = {0};
    char broadAddr[32] = {0};
    char subnetMask[32] = {0};


    string res="127.0.0.1";
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
            ifrcopy = buf[interfaceNum];
            if (ioctl(fd, SIOCGIFFLAGS, &ifrcopy))
            {
            	yang_error("ioctl: %s [%s:%d]\n", strerror(errno), __FILE__, __LINE__);

                close(fd);
                return res;
            }

            //get the mac of this interface
            if (!ioctl(fd, SIOCGIFHWADDR, (char *)(&buf[interfaceNum])))
            {
                memset(mac, 0, sizeof(mac));
                snprintf(mac, sizeof(mac), "%02x%02x%02x%02x%02x%02x",
                         (uint8_t)buf[interfaceNum].ifr_hwaddr.sa_data[0],
                        (uint8_t)buf[interfaceNum].ifr_hwaddr.sa_data[1],
                        (uint8_t)buf[interfaceNum].ifr_hwaddr.sa_data[2],

                        (uint8_t)buf[interfaceNum].ifr_hwaddr.sa_data[3],
                        (uint8_t)buf[interfaceNum].ifr_hwaddr.sa_data[4],
                        (uint8_t)buf[interfaceNum].ifr_hwaddr.sa_data[5]);

            }
            else
            {
            	yang_error("ioctl: %s [%s:%d]\n", strerror(errno), __FILE__, __LINE__);
                close(fd);
                return res;
            }

            //get the IP of this interface

            if (!ioctl(fd, SIOCGIFADDR, (char *)&buf[interfaceNum]))
            {
            	string s=(char *)inet_ntoa(((struct sockaddr_in *)&(buf[interfaceNum].ifr_addr))->sin_addr);
            	if(s!="127.0.0.1") res=s;
                snprintf(ip, sizeof(ip), "%s",
                         (char *)inet_ntoa(((struct sockaddr_in *)&(buf[interfaceNum].ifr_addr))->sin_addr));

            }
            else
            {
            	yang_error("ioctl: %s [%s:%d]\n", strerror(errno), __FILE__, __LINE__);
                close(fd);
                return res;
            }

            //get the broad address of this interface

            if (!ioctl(fd, SIOCGIFBRDADDR, &buf[interfaceNum]))
            {
                snprintf(broadAddr, sizeof(broadAddr), "%s",
                         (char *)inet_ntoa(((struct sockaddr_in *)&(buf[interfaceNum].ifr_broadaddr))->sin_addr));

            }
            else
            {
                yang_error("ioctl: %s [%s:%d]\n", strerror(errno), __FILE__, __LINE__);
                close(fd);
                return res;
            }

            //get the subnet mask of this interface
            if (!ioctl(fd, SIOCGIFNETMASK, &buf[interfaceNum]))
            {
                snprintf(subnetMask, sizeof(subnetMask), "%s",
                         (char *)inet_ntoa(((struct sockaddr_in *)&(buf[interfaceNum].ifr_netmask))->sin_addr));

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

    return res;
}
#endif
