//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef _YangConvert_H__
#define _YangConvert_H__

#include "stdint.h"

//#include "YangResize.h"
#define u8_t uint8_t
#ifdef __cplusplus
extern "C" {
#endif

void yang_pic_zoomIn4(uint8_t *src,uint8_t *dest,int32_t srcWidth,int32_t srcHeight,int32_t destWidth,int32_t destHeigh);
void yang_pic_zoom4Yuy2(uint8_t *src,uint8_t *dest,int32_t srcWidth,int32_t srcHeight,int32_t destWidth,int32_t destHeight);

void yang_pic_resize_NV12(uint8_t *src,uint8_t *dest,int32_t srcWidth,int32_t srcHeight,int32_t destWidth,int32_t destHeight);
void yang_pic_resize_Yuy2_NV12(uint8_t *src,uint8_t *dest,int32_t p_srcWidth,int32_t p_srcHeight,int32_t p_destWidth,int32_t p_destHeight);
void yang_pic_nv12_nearest_scale(uint8_t*  src, uint8_t*  dst,int32_t srcWidth,int32_t srcHeight,int32_t dstWidth,int32_t dstHeight) ;
void yang_pic_YUY2toI420(int32_t inWidth, int32_t inHeight, uint8_t *pSrc, uint8_t *pDest);
void yang_pic_YUY2toNV12(int32_t inWidth, int32_t inHeight, uint8_t *pSrc, uint8_t *pDest);
void yang_pic_plusNV12(uint8_t* src,uint8_t * dest,int32_t model,int32_t srcWidth,int32_t srcHeight,int32_t destWidth,int32_t destHeight);
void yang_pic_plusYuy2(uint8_t* src,uint8_t * dest,int32_t model,int32_t srcWidth,int32_t srcHeight,int32_t destWidth,int32_t destHeight);

void yang_pic_RGB24_TO_YV12(uint8_t* yv12,uint8_t* rgb24,int32_t w,int32_t h);
void yang_pic_RGB24_To_I420(  uint8_t *RGBbuf,  uint8_t *YUV,  int32_t width,  int32_t height );
void yang_pic_RGB24_To_NV12(  uint8_t *RGBbuf,  uint8_t *YUV,  int32_t width,  int32_t height );
void yang_pic_YUY2_To_RGB24(uint8_t *YUY2buff,uint8_t *RGBbuff,unsigned long dwSize);

	uint8_t clip255(long v);
#ifdef __cplusplus
}
#endif
#endif

