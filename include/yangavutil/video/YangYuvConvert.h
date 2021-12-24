/*
 * YangYuvUtil.h
 *
 *  Created on: 2020年10月8日
 *      Author: yang
 */

#ifndef YangYuvConvert_H_
#define YangYuvConvert_H_
//#include "yangutil/yangavtype.h"
#include "stdint.h"
#include "yangutil/sys/YangLoadLib.h"
#include "libyuv.h"
using namespace libyuv;

class YangYuvConvert {
public:
	YangYuvConvert();
	virtual ~YangYuvConvert();
	//void rgbtonv12();
	int32_t yuy2tonv12(uint8_t* src,uint8_t *dst,int32_t srcWidth,int32_t srcHeiht);
	int32_t yuy2toi420(uint8_t* src,uint8_t *dst,int32_t srcWidth,int32_t srcHeiht);
	int32_t yuy2toargb(uint8_t* src,uint8_t *dst,int32_t srcWidth,int32_t srcHeiht);
	int32_t i420tonv12(uint8_t* src,uint8_t *dst,int32_t srcWidth,int32_t srcHeiht);
	int32_t i420tonv21(uint8_t* src,uint8_t *dst,int32_t srcWidth,int32_t srcHeiht);
	int32_t bgr24toyuy2(uint8_t* src,uint8_t *dst,int32_t srcWidth,int32_t srcHeiht);
	int32_t rgb24toI420(uint8_t* src_rgb24,uint8_t *dst,int32_t width,int32_t height);
	int32_t rgbatoI420(uint8_t* src_rgba,uint8_t *dst,int32_t width,int32_t height);
	int32_t bgratoI420(uint8_t* src_rgba,uint8_t *dst,int32_t width,int32_t height);
	//I420ToRGB24
	int32_t I420torgb24(uint8_t* src_rgb24,uint8_t *dst,int32_t width,int32_t height);
	int32_t nv12torgb24(uint8_t* src_rgb24,uint8_t *dst,int32_t width,int32_t height);
	int32_t nv21torgb24(uint8_t* src_rgb24,uint8_t *dst,int32_t width,int32_t height);
	int32_t argbtorgb24(uint8_t* src_argb, uint8_t *dst,int32_t width,int32_t height);
	int32_t rgb24toargb(uint8_t *src_rgb24, uint8_t *dst, int32_t width,int32_t height);
	int32_t scaleNv12(uint8_t* src,uint8_t *dst,int32_t srcWidth,int32_t srcHeight,int32_t dstWidth,int32_t dstHeight,int32_t mode=2);
	int32_t scaleI420(uint8_t* src,uint8_t *dst,int32_t srcWidth,int32_t srcHeight,int32_t dstWidth,int32_t dstHeight,int32_t mode=2);
	int32_t scaleYuy2(uint8_t* src,uint8_t *dst,int32_t srcWidth,int32_t srcHeight,int32_t dstWidth,int32_t dstHeight,int32_t mode=2);
	int32_t scaleRgb(uint8_t* src,uint8_t *dst,int32_t srcWidth,int32_t srcHeight,int32_t dstWidth,int32_t dstHeight,int32_t mode=2);
	int32_t scaleArgb(uint8_t* src,uint8_t *dst,int32_t srcWidth,int32_t srcHeight,int32_t dstWidth,int32_t dstHeight,int32_t mode=2);


private:
	YangLoadLib m_lib;
	void loadLib();
	void unloadLib();
	//I420ToNV12
	int32_t (*yang_YUY2ToNV12)(const uint8_t* src_yuy2,
	               int32_t src_stride_yuy2,
	               uint8_t* dst_y,
	               int32_t dst_stride_y,
	               uint8_t* dst_uv,
	               int32_t dst_stride_uv,
	               int32_t width,
	               int32_t height);
	int32_t (*yang_YUY2ToI420)(const uint8_t* src_yuy2,
	               int32_t src_stride_yuy2,
	               uint8_t* dst_y,
	               int32_t dst_stride_y,
	               uint8_t* dst_u,
	               int32_t dst_stride_u,
	               uint8_t* dst_v,
	               int32_t dst_stride_v,
	               int32_t width,
	               int32_t height);
	int (*yang_YUY2ToARGB)(const uint8_t* src_yuy2,
	               int src_stride_yuy2,
	               uint8_t* dst_argb,
	               int dst_stride_argb,
	               int width,
	               int height);
	int32_t (*yang_I420ToNV12)(const uint8_t* src_y,
	               int32_t src_stride_y,
	               const uint8_t* src_u,
	               int32_t src_stride_u,
	               const uint8_t* src_v,
	               int32_t src_stride_v,
	               uint8_t* dst_y,
	               int32_t dst_stride_y,
	               uint8_t* dst_uv,
	               int32_t dst_stride_uv,
	               int32_t width,
	               int32_t height);

	int32_t (*yang_I420ToNV21)(const uint8_t* src_y,
	               int32_t src_stride_y,
	               const uint8_t* src_u,
	               int32_t src_stride_u,
	               const uint8_t* src_v,
	               int32_t src_stride_v,
	               uint8_t* dst_y,
	               int32_t dst_stride_y,
	               uint8_t* dst_vu,
	               int32_t dst_stride_vu,
	               int32_t width,
	               int32_t height);

	int32_t (*yang_I420ToRGB24)(const uint8_t* src_y,
	                int32_t src_stride_y,
	                const uint8_t* src_u,
	                int32_t src_stride_u,
	                const uint8_t* src_v,
	                int32_t src_stride_v,
	                uint8_t* dst_rgb24,
	                int32_t dst_stride_rgb24,
	                int32_t width,
	                int32_t height);
	int32_t (*yang_NV12ToRGB24)(const uint8_t* src_y,
	                int32_t src_stride_y,
	                const uint8_t* src_uv,
	                int32_t src_stride_uv,
	                uint8_t* dst_rgb24,
	                int32_t dst_stride_rgb24,
	                int32_t width,
	                int32_t height);
	int32_t (*yang_NV21ToRGB24)(const uint8_t* src_y,
	                int32_t src_stride_y,
	                const uint8_t* src_vu,
	                int32_t src_stride_vu,
	                uint8_t* dst_rgb24,
	                int32_t dst_stride_rgb24,
	                int32_t width,
	                int32_t height);
	int32_t (*yang_ARGBToRGB24)(const uint8_t* src_argb,
	                int32_t src_stride_argb,
	                uint8_t* dst_rgb24,
	                int32_t dst_stride_rgb24,
	                int32_t width,
	                int32_t height);
	int32_t (*yang_RGB24ToARGB)(const uint8_t* src_rgb24,
	                int32_t src_stride_rgb24,
	                uint8_t* dst_argb,
	                int32_t dst_stride_argb,
	                int32_t width,
	                int32_t height);
	int32_t (*yang_RAWToARGB)(const uint8_t* src_raw,
	              int32_t src_stride_raw,
	              uint8_t* dst_argb,
	              int32_t dst_stride_argb,
	              int32_t width,
	              int32_t height);
	int32_t (*yang_RGB24ToI420)(const uint8_t* src_rgb24,
	                int32_t src_stride_rgb24,
	                uint8_t* dst_y,
	                int32_t dst_stride_y,
	                uint8_t* dst_u,
	                int32_t dst_stride_u,
	                uint8_t* dst_v,
	                int32_t dst_stride_v,
	                int32_t width,
	                int32_t height);
	int (*yang_RGBAToI420)(const uint8_t* src_rgba,
	               int src_stride_rgba,
	               uint8_t* dst_y,
	               int dst_stride_y,
	               uint8_t* dst_u,
	               int dst_stride_u,
	               uint8_t* dst_v,
	               int dst_stride_v,
	               int width,
	               int height);

	int (*yang_BGRAToI420)(const uint8_t* src_bgra,
	               int src_stride_bgra,
	               uint8_t* dst_y,
	               int dst_stride_y,
	               uint8_t* dst_u,
	               int dst_stride_u,
	               uint8_t* dst_v,
	               int dst_stride_v,
	               int width,
	               int height);
	int (*yang_ARGBToI420)(const uint8_t* src_argb,
	               int src_stride_argb,
	               uint8_t* dst_y,
	               int dst_stride_y,
	               uint8_t* dst_u,
	               int dst_stride_u,
	               uint8_t* dst_v,
	               int dst_stride_v,
	               int width,
	               int height);
	int32_t (*yang_NV12Scale)(const uint8_t* src_y,
	              int32_t src_stride_y,
	              const uint8_t* src_uv,
	              int32_t src_stride_uv,
	              int32_t src_width,
	              int32_t src_height,
	              uint8_t* dst_y,
	              int32_t dst_stride_y,
	              uint8_t* dst_uv,
	              int32_t dst_stride_uv,
	              int32_t dst_width,
	              int32_t dst_height,
	              enum FilterMode filtering);
	int32_t (*yang_I420Scale)(const uint8_t* src_y,
	              int32_t src_stride_y,
	              const uint8_t* src_u,
	              int32_t src_stride_u,
	              const uint8_t* src_v,
	              int32_t src_stride_v,
	              int32_t src_width,
	              int32_t src_height,
	              uint8_t* dst_y,
	              int32_t dst_stride_y,
	              uint8_t* dst_u,
	              int32_t dst_stride_u,
	              uint8_t* dst_v,
	              int32_t dst_stride_v,
	              int32_t dst_width,
	              int32_t dst_height,
	              enum FilterMode filtering);
	void (*yang_ScalePlane)(const uint8_t* src,
	                int32_t src_stride,
	                int32_t src_width,
	                int32_t src_height,
	                uint8_t* dst,
	                int32_t dst_stride,
	                int32_t dst_width,
	                int32_t dst_height,
	                enum FilterMode filtering);
	int32_t (*yang_ARGBScale)(const uint8_t* src_argb,
	              int32_t src_stride_argb,
	              int32_t src_width,
	              int32_t src_height,
	              uint8_t* dst_argb,
	              int32_t dst_stride_argb,
	              int32_t dst_width,
	              int32_t dst_height,
	              enum FilterMode filtering);
};

#endif /* YANGYUVUTIL_H_ */
