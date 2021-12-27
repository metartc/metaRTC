#ifndef YANG_TIME_H__
#define YANG_TIME_H__




#include <stdint.h>
#ifdef __cplusplus
extern "C"{
#include <yangutil/sys/YangCTime.h>
}
#else
#include <yangutil/sys/YangCTime.h>
#endif

#define yang_get_system_time yang_get_system_micro_time
#define yang_update_system_time yang_get_system_micro_time
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
