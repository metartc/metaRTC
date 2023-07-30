//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef INCLUDE_YANGAVUTIL_VIDEO_YANGCYUVUTIL_H_
#define INCLUDE_YANGAVUTIL_VIDEO_YANGCYUVUTIL_H_

#include <yangutil/yangtype.h>

void yang_plusNV12(uint8_t* src,uint8_t * dest,int32_t model,int32_t srcWidth,int32_t srcHeight,int32_t destWidth,int32_t destHeight);
void yang_plusI420(uint8_t* src,uint8_t * dest,int32_t model,int32_t srcWidth,int32_t srcHeight,int32_t destWidth,int32_t destHeight);
void yang_plusYuy2(uint8_t* src,uint8_t * dest,int32_t model,int32_t srcWidth,int32_t srcHeight,int32_t destWidth,int32_t destHeight);
void yang_rgbtobgr(uint8_t *rgb,uint8_t *bgr,int32_t srcWidth,int32_t srcHeight);
void yang_zoom4(uint8_t* src,uint8_t* dst,int32_t wid,int32_t hei);





#endif /* INCLUDE_YANGAVUTIL_VIDEO_YANGCYUVUTIL_H_ */
