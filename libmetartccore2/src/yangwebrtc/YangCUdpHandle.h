/*
 * YangCUdpHandle.h
 *
 *  Created on: 2021年12月28日
 *      Author: yang
 */

#ifndef SRC_YANGWEBRTC_YANGCUDPHANDLE_H_
#define SRC_YANGWEBRTC_YANGCUDPHANDLE_H_

#include <yangwebrtc/YangUdpHandleH.h>
int32_t yang_init_rtcudp(YangUdpHandle* udp,char* pserverIp,int32_t plocalPort);
void yang_destroy_rtcudp(YangUdpHandle* udp);
void yang_start_rtcudp(YangUdpHandle* udp);
void yang_stop_rtcudp(YangUdpHandle* udp);
int32_t yang_rtc_sendData(YangUdpHandle* udp,char* p,int32_t plen);



#endif /* SRC_YANGWEBRTC_YANGCUDPHANDLE_H_ */
