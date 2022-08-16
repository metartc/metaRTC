//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef INCLUDE_YANGUTIL_SYS_YANGMATH_H_
#define INCLUDE_YANGUTIL_SYS_YANGMATH_H_
#include <stdint.h>
static inline int16_t yang_rtp_seq_distance(const uint16_t prev_value, const uint16_t value)
{
    return (int16_t)(value - prev_value);
}
#ifdef __cplusplus
extern "C"{
#endif

uint64_t yang_random();
int32_t yang_insert_uint16_sort(uint16_t a[],uint16_t value,uint32_t* alen);
#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_YANGUTIL_SYS_YANGMATH_H_ */
