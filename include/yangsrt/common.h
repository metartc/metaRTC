//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef MPEGTS_COMMON_H
#define MPEGTS_COMMON_H

#include <stdint.h>
#include "yangutil/YangErrorCode.h"
class MpegTsAdaptationFieldType
{
public:
    // Reserved for future use by ISO/IEC
    static const uint8_t reserved = 0x00;
    // No adaptation_field, payload only
    static const uint8_t payload_only = 0x01;
    // Adaptation_field only, no payload
    static const uint8_t adaption_only = 0x02;
    // Adaptation_field followed by payload
    static const uint8_t payload_adaption_both = 0x03;
};

class YangTsBuffer;

extern void write_pcr(YangTsBuffer *sb, uint64_t pcr);
extern void write_pts(YangTsBuffer *sb, uint32_t fb, uint64_t pts);

extern uint64_t read_pts(YangTsBuffer *sb);
extern uint64_t read_pcr(YangTsBuffer *sb);

#endif //MPEGTS_COMMON_H
