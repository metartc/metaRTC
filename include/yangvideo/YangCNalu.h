//
// Copyright (c) 2019-2026 yanggaofeng
//
#ifndef INCLUDE_YANGAVUTIL_VIDEO_YANGCNALU_H_
#define INCLUDE_YANGAVUTIL_VIDEO_YANGCNALU_H_

#include <yangutil/yangtype.h>
#include <yangutil/yangavinfo.h>
#include <stdint.h>

int32_t yang_nalu_getH264KeyframePos( uint8_t* data,int32_t nb);
int32_t yang_nalu_getH265KeyframePos( uint8_t* data,int32_t nb);

inline yangbool yang_nalu_isH265Vcl(uint8_t nalType) {
    return nalType <= 31?yangtrue:yangfalse; // VCL
}

inline yangbool yang_nalu_isH265Sei(uint8_t nalType) {
    return (nalType == 39 || nalType == 40)?yangtrue:yangfalse; // prefix / suffix SEI
}

 inline yangbool yang_nalu_isH265FirstSlice(const uint8_t *nalu, uint32_t naluSize) {
    if (naluSize < 3) return yangfalse;
    return ((nalu[2] & 0x80) != 0)?yangtrue:yangfalse;
}



#endif /* INCLUDE_YANGAVUTIL_VIDEO_YANGCNALU_H_ */
