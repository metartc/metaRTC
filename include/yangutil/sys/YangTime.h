//
// Copyright (c) 2019-2022 yanggaofeng
//
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
#ifdef __cplusplus


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
#endif
