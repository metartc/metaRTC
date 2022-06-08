//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangsrt/crc.h>

// @see http://www.stmc.edu.hk/~vincent/ffmpeg_0.4.9-pre1/libavformat/mpegtsenc.c
uint32_t crc32(const uint8_t *data, int32_t len)
{
    int32_t i;
    uint32_t crc = 0xffffffff;

    for (i = 0; i<len; i++)
        crc = (crc << 8) ^ crc_table[((crc >> 24) ^ *data++) & 0xff];

    return crc;
}
