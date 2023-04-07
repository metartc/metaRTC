//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef INCLUDE_YANGUTIL_YANGTYPE_H_
#define INCLUDE_YANGUTIL_YANGTYPE_H_
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <yang_config.h>
#include <yangutil/yangunistd.h>
#include <yangutil/yangerrorcode.h>
#include <yangutil/yangmemory.h>


#define Yang_Server_Srs 0
#define Yang_Server_Zlm 1
#define Yang_Server_P2p 7
#define Yang_Server_Whip_Whep 9

#define Yang_Rtmp 0
#define Yang_Srt 1
#define Yang_Webrtc 2

#define yangbool int32_t
#define yangtrue 1
#define yangfalse 0

#define yang_delete(a) {if( (a)) {delete (a); (a) = NULL;}}
#define yang_deleteA(a) {if( (a)) {delete[] (a); (a) = NULL;}}
#define yang_free(a) {if( (a)) {free((a)); (a) = NULL;}}
#define yang_min(a, b) (((a) < (b))? (a) : (b))
#define yang_max(a, b) (((a) < (b))? (b) : (a))


#define YANG_UTIME_MILLISECONDS 1000
#define YANG_UTIME_SECONDS 		1000000
#if Yang_Enable_Embbed
#define YANG_VIDEO_ENCODE_BUFFER_LEN 1024 * 512
#define YANG_RTC_RECV_BUFFER_COUNT 512
#define YANG_RTC_RECV_BUFFER_COUNT2 511
#else
#define YANG_VIDEO_ENCODE_BUFFER_LEN 1024 * 1024
#define YANG_RTC_RECV_BUFFER_COUNT 1024
#define YANG_RTC_RECV_BUFFER_COUNT2 1023
#endif
#define YANG_GET_RECV_BUFFER_INDEX(x) 	 x&YANG_RTC_RECV_BUFFER_COUNT2

#define Yang_AV1_Name "AV1X"

#define YANGALIGN(x, a) (((x)+(a)-1)&~((a)-1))
#define YANG_INADDR_ANY 0x00000000
typedef enum{
	Yang_Socket_Protocol_Udp,
	Yang_Socket_Protocol_Tcp
}YangSocketProtocol;

typedef enum {
	Yang_IpFamilyType_IPV4,
	Yang_IpFamilyType_IPV6
} YangIpFamilyType;




#endif /* INCLUDE_YANGUTIL_YANGTYPE_H_ */
