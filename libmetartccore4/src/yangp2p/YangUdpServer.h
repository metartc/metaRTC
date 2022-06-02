//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGWEBRTC_YangUdpServer_H_
#define YANGWEBRTC_YangUdpServer_H_

#include <yangp2p/YangUdpServerH.h>
#ifdef __cplusplus
extern "C"{
#endif
int32_t yang_create_udpserver(YangUdpServer* udp,char* pserverIp,int32_t listenPort,int32_t plocalPort);
void yang_destroy_udpserver(YangUdpServer* udp);
void yang_start_udpserver(YangUdpServer* udp);
void yang_stop_udpserver(YangUdpServer* udp);
int32_t yang_rtc_sendData(YangUdpServer* udp,char* p,int32_t plen);
#ifdef __cplusplus
}
#endif
#endif /* YANGWEBRTC_YANGUDPHANDLE_H_ */
