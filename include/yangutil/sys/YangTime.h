#ifndef YANG_TIME_H__
#define YANG_TIME_H__
#ifndef _MSC_VER
#include <sys/time.h>
#endif
#include <stdlib.h>
#include <iostream>
#include <stdint.h>
using namespace std;
#define yang_get_system_time YangSystime::get_system_micro_time
#define yang_update_system_time YangSystime::get_system_micro_time

int64_t yang_get_milli_time();//haomiao
int64_t yang_get_micro_time();//weimiao
#ifdef _MSC_VER
#include <winsock2.h>
int gettimeofday(struct timeval *tp, void *tzp);
int64_t yang_get_milli_tick();//haomiao
int64_t yang_get_micro_tick();//weimiao
#else
#define yang_get_milli_tick  yang_get_milli_time
#define yang_get_micro_tick  yang_get_micro_time
#endif



int64_t yang_get_nano_tick();//namiao
class YangSystime{
public:
	YangSystime();
	~YangSystime();

	static int64_t system_time_us_cache;
	static int64_t system_time_startup_time;
        static void update_system_time();
        static int64_t get_system_milli_time();
        static int64_t get_system_micro_time();
#ifdef _MSC_VER
        static LARGE_INTEGER  large_interger;
        static int64_t dff;
#endif
};

class YangNtp
{
public:
    YangNtp();
    virtual ~YangNtp();
public:
    uint64_t m_system_ms;
    uint64_t m_ntp;
    uint32_t m_ntp_second;
    uint32_t m_ntp_fractions;
public:
    static YangNtp from_time_ms(uint64_t ms);
    static YangNtp to_time_ms(uint64_t ntp);
public:
    static uint64_t kMagicNtpFractionalUnit;
};

class YangWallClock
{
public:
    YangWallClock();
    virtual ~YangWallClock();
public:
    /**
     * Current time in get_system_milli_time.
     */
    virtual int64_t now();
};


#endif
