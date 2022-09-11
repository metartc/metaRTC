//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef INCLUDE_YANGUTIL_YANGTYPE_H_
#define INCLUDE_YANGUTIL_YANGTYPE_H_
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <yang_config.h>
#include <yangutil/yang_unistd.h>
#include "YangErrorCode.h"
#define Yang_Server_Srs 0
#define Yang_Server_Zlm 1
#define Yang_Server_P2p 9

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


typedef enum {
	Yang_IpFamilyType_IPV4 =  0x0001,
	Yang_IpFamilyType_IPV6 =  0x0002,
} YangIpFamilyType;

typedef struct {
    uint16_t family;
    uint16_t port;
    uint32_t mapAddress;
    uint8_t  address[16];
} YangIpAddress;


#ifdef __cplusplus
#define YangAutoFree(className, instance) \
impl_YangAutoFree<className> _auto_free_##instance(&instance, false)
#define YangAutoFreeA(className, instance) \
impl_YangAutoFree<className> _auto_free_array_##instance(&instance, true)
template<class T>
class impl_YangAutoFree
{
private:
    T** ptr;
    bool is_array;
public:
    impl_YangAutoFree(T** p, bool array) {
        ptr = p;
        is_array = array;
    }

    virtual ~impl_YangAutoFree() {
        if (ptr == NULL || *ptr == NULL) {
            return;
        }

        if (is_array) {
            delete[] *ptr;
        } else {
            delete *ptr;
        }

        *ptr = NULL;
    }
};
#else
#define bool int32_t
#define true 1
#define false 0
#endif
#endif /* INCLUDE_YANGUTIL_YANGTYPE_H_ */
