//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef INCLUDE_YANGUTIL_SYS_YANGCMATH_H_
#define INCLUDE_YANGUTIL_SYS_YANGCMATH_H_
#include <stdint.h>
// The "distance" between two uint16 number, for example:
//      distance(prev_value=3, value=5) === (int16_t)(uint16_t)((uint16_t)3-(uint16_t)5) === -2
//      distance(prev_value=3, value=65534) === (int16_t)(uint16_t)((uint16_t)3-(uint16_t)65534) === 5
//      distance(prev_value=65532, value=65534) === (int16_t)(uint16_t)((uint16_t)65532-(uint16_t)65534) === -2
// For RTP sequence, it's only uint16 and may flip back, so 3 maybe 3+0xffff.
// @remark Note that yang_rtp_seq_distance(0, 32768)>0 is TRUE      but for WebRTC jitter buffer it's FALSE and we follow it.
// @remark For srs_rtp_seq_distance(32768, 0)>0, it's FALSE definitely.
static inline int16_t yang_rtp_seq_distance(const uint16_t prev_value, const uint16_t value)
{
    return (int16_t)(value - prev_value);
}
uint64_t yang_random();
int32_t yang_insert_uint16(uint16_t a[],uint16_t value,uint32_t* alen);


#endif /* INCLUDE_YANGUTIL_SYS_YANGCMATH_H_ */
