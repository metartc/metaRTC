//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangutil/sys/YangSsrc.h>
#include <yangrtp/YangRtpConstant.h>

#if Yang_OS_WIN
#include <Windows.h>
#else
#include <sys/time.h>
#endif

uint32_t g_ssrc_num=0;


uint64_t __crc32_reflect(uint64_t data, int32_t width)
{
    uint64_t res = data & 0x01;

    for (int32_t i = 0; i < (int)width - 1; i++) {
        data >>= 1;
        res = (res << 1) | (data & 0x01);
    }

    return res;
}
void __crc32_make_table(uint32_t t[256], uint32_t poly, yangbool reflect_in)
{
    int32_t width = 32; // 32bits checksum.
    uint64_t msb_mask = (uint32_t)(0x01 << (width - 1));
    uint64_t mask = (uint32_t)(((msb_mask - 1) << 1) | 1);

    int32_t tbl_idx_width = 8; // table index size.
    int32_t tbl_width = 0x01 << tbl_idx_width; // table size: 256

    for (int32_t i = 0; i < (int)tbl_width; i++) {
        uint64_t reg = (uint64_t)i;

        if (reflect_in) {
            reg = __crc32_reflect(reg, tbl_idx_width);
        }

        reg = reg << (width - tbl_idx_width);
        for (int32_t j = 0; j < tbl_idx_width; j++) {
            if ((reg&msb_mask) != 0) {
                reg = (reg << 1) ^ poly;
            } else {
                reg = reg << 1;
            }
        }

        if (reflect_in) {
            reg = __crc32_reflect(reg, width);
        }

        t[i] = (uint32_t)(reg & mask);
    }
}

// @see pycrc table_driven at https://github.com/winlinvip/pycrc/blob/master/pycrc/algorithms.py#L207
uint32_t __crc32_table_driven(uint32_t* t, const void* buf, int32_t size, uint32_t previous, yangbool reflect_in, uint32_t xor_in, yangbool reflect_out, uint32_t xor_out)
{
    int32_t width = 32; // 32bits checksum.
    uint64_t msb_mask = (uint32_t)(0x01 << (width - 1));
    uint64_t mask = (uint32_t)(((msb_mask - 1) << 1) | 1);

    int32_t tbl_idx_width = 8; // table index size.

    uint8_t* p = (uint8_t*)buf;
    uint64_t reg = 0;

    if (!reflect_in) {
        reg = xor_in;

        for (int32_t i = 0; i < (int)size; i++) {
            uint8_t tblidx = (uint8_t)((reg >> (width - tbl_idx_width)) ^ p[i]);
            reg = t[tblidx] ^ (reg << tbl_idx_width);
        }
    } else {
        reg = previous ^ __crc32_reflect(xor_in, width);

        for (int32_t i = 0; i < (int)size; i++) {
            uint8_t tblidx = (uint8_t)(reg ^ p[i]);
            reg = t[tblidx] ^ (reg >> tbl_idx_width);
        }

        reg = __crc32_reflect(reg, width);
    }

    if (reflect_out) {
        reg = __crc32_reflect(reg, width);
    }

    reg ^= xor_out;
    return (uint32_t)(reg & mask);
}

// @see pycrc https://github.com/winlinvip/pycrc/blob/master/pycrc/algorithms.py#L207
// IEEETable is the table for the IEEE polynomial.
static uint32_t __crc32_IEEE_table[256];
static yangbool __crc32_IEEE_table_initialized = yangfalse;
uint32_t yang_crc32_ieee(const void* buf, int32_t size, uint32_t previous)
{
    // @see golang IEEE of hash/crc32/crc32.go
    // IEEE is by far and away the most common CRC-32 polynomial.
    // Used by ethernet (IEEE 802.3), v.42, fddi, gzip, zip, png, ...
    // @remark The poly of CRC32 IEEE is 0x04C11DB7, its reverse is 0xEDB88320,
    //      please read https://en.wikipedia.org/wiki/Cyclic_redundancy_check
    uint32_t poly = 0x04C11DB7;

    yangbool reflect_in = yangtrue;
    uint32_t xor_in = 0xffffffff;
    yangbool reflect_out = yangtrue;
    uint32_t xor_out = 0xffffffff;

    if (!__crc32_IEEE_table_initialized) {
        __crc32_make_table(__crc32_IEEE_table, poly, reflect_in);
        __crc32_IEEE_table_initialized = yangtrue;
    }

    return __crc32_table_driven(__crc32_IEEE_table, buf, size, previous, reflect_in, xor_in, reflect_out, xor_out);
}

uint32_t yang_generate_ssrc(){
	 if (!g_ssrc_num) {


	        #ifdef _MSC_VER
                   g_ssrc_num = GetCurrentProcessId() * 10000 + GetCurrentProcessId() * 100 + GetCurrentProcessId();
	        #else
	               g_ssrc_num = getpid() * 10000 + getpid() * 100 + getpid();
	        #endif

	    }

	    return ++g_ssrc_num;
}


