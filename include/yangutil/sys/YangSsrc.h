//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGWEBRTC_YANGRTCUTIL_H_
#define YANGWEBRTC_YANGRTCUTIL_H_
#include <yangutil/yangtype.h>
#ifdef __cplusplus
extern "C"{
#endif

uint32_t yang_generate_ssrc();
uint32_t yang_crc32_ieee(const void* buf, int32_t size, uint32_t previous);

#ifdef __cplusplus
}
#endif


#endif /* YANGWEBRTC_YANGRTCUTIL_H_ */
