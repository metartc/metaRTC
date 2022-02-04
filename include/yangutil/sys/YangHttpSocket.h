//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef SRC_YANGUSERDATA_INCLUDE_YangHttpSocket_H_
#define SRC_YANGUSERDATA_INCLUDE_YangHttpSocket_H_

#ifdef __cplusplus
extern "C"{
#include <yangutil/sys/YangCHttpSocket.h>
}
#else
#include <yangutil/sys/YangCHttpSocket.h>
#endif

//#include <string>
/**
class YangHttpSocket {
public:
	YangHttpSocket();
	virtual ~YangHttpSocket();
    int32_t post(std::string& rets,char* ip,int32_t port,char* api,uint8_t *p, int32_t plen);
private:
    int32_t getIndex(char* buf,int plen);
	int32_t init(char* serverip,int32_t pport);
	int32_t connectServer();
 #ifdef _WIN32
   SOCKET m_socket;
   SOCKADDR_IN m_serverAddress;
 #else
   int32_t m_socket;
    struct sockaddr_in m_serverAddress;
  #endif

};
**/
#endif /* SRC_YANGUSERDATA_INCLUDE_YangHttpSocket_H_ */
