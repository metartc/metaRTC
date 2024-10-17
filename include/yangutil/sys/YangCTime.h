//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef INCLUDE_YANGUTIL_SYS_YANGCTIME_H_
#define INCLUDE_YANGUTIL_SYS_YANGCTIME_H_
#include <yangutil/yangtype.h>
#define yang_time_t time_t

uint64_t yang_get_system_micro_time();
uint64_t yang_get_milli_time();//haomiao
uint64_t yang_get_micro_time();//weimiao

#define yang_get_milli_tick  yang_get_milli_time
#define yang_get_micro_tick  yang_get_micro_time


#if Yang_OS_WIN
#include <winsock2.h>
int gettimeofday(struct timeval *tp, void *tzp);
#endif
uint64_t yang_get_nano_tick();//namiao
typedef struct YangNtp{
    uint64_t system_ms;
    uint64_t ntp;
    uint32_t ntp_second;
    uint32_t ntp_fractions;
}YangNtp;

uint64_t yang_get_ntptime_fromms(uint64_t ms);
uint64_t yang_get_ntptime_fromntp(uint64_t ntp);
void yang_ntp_from_time_ms(YangNtp* ntp,uint64_t ms);
void yang_ntp_to_time_ms(YangNtp* pntp,uint64_t ntp);

#endif /* INCLUDE_YANGUTIL_SYS_YANGCTIME_H_ */
