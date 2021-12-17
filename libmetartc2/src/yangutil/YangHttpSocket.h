/*
 * YangUserDataConnection.h
 *
 *  Created on: 2020年9月1日
 *      Author: yang
 */

#ifndef SRC_YANGUSERDATA_INCLUDE_YangHttpSocket_H_
#define SRC_YANGUSERDATA_INCLUDE_YangHttpSocket_H_

#ifdef _WIN32
#include <winsock2.h>

#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <string>



class YangHttpSocket {
public:
	YangHttpSocket();
	virtual ~YangHttpSocket();

	int32_t init(char* serverip,int32_t pport);
	int32_t connectServer();
	int32_t disConnectServer();
	int32_t publish(uint8_t* p,int32_t plen);
    int32_t sendHeartbeat();

    int32_t m_isStart;
    void stop();

protected:
   void run();
   int32_t post(std::string api,uint8_t *p, int32_t plen);
   void stopLoop();
private:
   int32_t m_loop;
   uint8_t m_heartBuf[3];
 #ifdef _WIN32
   SOCKET m_socket;
   SOCKADDR_IN m_serverAddress;
 #else
   int32_t m_socket;
    struct sockaddr_in m_serverAddress;
  #endif

};

#endif /* SRC_YANGUSERDATA_INCLUDE_YangHttpSocket_H_ */
