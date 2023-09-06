//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YangYuvConvert_H_
#define YangYuvConvert_H_
#include <yangutil/sys/YangLoadLib.h>

#if Yang_Enable_YuvSo
	#include <libyuv.h>
#else
extern "C"{
	#include <libyuv.h>
}
#endif
using namespace libyuv;

#if Yang_OS_ANDROID
#define Yang_Enable_YuvSo 0
#else
#define Yang_Enable_YuvSo 0
#endif

class YangYuvConvert {
public:
	YangYuvConvert();
	virtual ~YangYuvConvert();

	int32_t yuy2tonv12(uint8_t* src,uint8_t *dst,int32_t width,int32_t height);

	int32_t yuy2toargb(uint8_t* src,uint8_t *dst,int32_t width,int32_t height);
	int32_t i420toyv12(uint8_t *src,uint8_t *dst,int32_t width,int32_t height);
	int32_t i420tonv12(uint8_t* src,uint8_t *dst,int32_t width,int32_t height);
	int32_t i420tonv21(uint8_t* src,uint8_t *dst,int32_t width,int32_t height);
	int32_t bgr24toyuy2(uint8_t* src,uint8_t *dst,int32_t width,int32_t height);
	//toI420
	int32_t rgb24toI420(uint8_t* src_rgb24,uint8_t *dst,int32_t width,int32_t height);
	int32_t rgbatoI420(uint8_t* src_rgba,uint8_t *dst,int32_t width,int32_t height);
	int32_t bgratoI420(uint8_t* src_rgba,uint8_t *dst,int32_t width,int32_t height);
	int32_t yuy2toI420(uint8_t* src,uint8_t *dst,int32_t width,int32_t height);
	int32_t nv12toI420(uint8_t* src,uint8_t *dst,int32_t width,int32_t height);
	int32_t nv21toI420(uint8_t* src,uint8_t *dst,int32_t width,int32_t height);

	//ToRGB24
	int32_t I420torgb24(uint8_t* src_rgb24,uint8_t *dst,int32_t width,int32_t height);
	int32_t nv12torgb24(uint8_t* src_rgb24,uint8_t *dst,int32_t width,int32_t height);
	int32_t nv21torgb24(uint8_t* src_rgb24,uint8_t *dst,int32_t width,int32_t height);
	int32_t argbtorgb24(uint8_t* src_argb, uint8_t *dst,int32_t width,int32_t height);
	int32_t rgb24toargb(uint8_t *src_rgb24, uint8_t *dst, int32_t width,int32_t height);
	//scale
	int32_t scaleNv12(uint8_t* src,uint8_t *dst,int32_t srcWidth,int32_t srcHeight,int32_t dstWidth,int32_t dstHeight,int32_t mode=2);
	int32_t scaleI420(uint8_t* src,uint8_t *dst,int32_t srcWidth,int32_t srcHeight,int32_t dstWidth,int32_t dstHeight,int32_t mode=2);
	int32_t scaleYuy2(uint8_t* src,uint8_t *dst,int32_t srcWidth,int32_t srcHeight,int32_t dstWidth,int32_t dstHeight,int32_t mode=2);
	int32_t scaleRgb(uint8_t* src,uint8_t *dst,int32_t srcWidth,int32_t srcHeight,int32_t dstWidth,int32_t dstHeight,int32_t mode=2);
	int32_t scaleArgb(uint8_t* src,uint8_t *dst,int32_t srcWidth,int32_t srcHeight,int32_t dstWidth,int32_t dstHeight,int32_t mode=2);
// kRotate0 = 0,      // No rotation.	  kRotate90 = 90,    // Rotate 90 degrees clockwise.
//	  kRotate180 = 180,  // Rotate 180 degrees.	  kRotate270 = 270,  // Rotate 270 degrees clockwise.
	   // Deprecated.	  kRotateNone = 0,	  kRotateClockwise = 90,	  kRotateCounterClockwise = 270
	int32_t rotateI420(uint8_t* src,uint8_t *dst,int32_t width,int32_t height,RotationMode mode);

#if Yang_Enable_YuvSo
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


	int (*yang_NV12ToI420)(const uint8_t* src_y,
	               int src_stride_y,
	               const uint8_t* src_uv,
	               int src_stride_uv,
	               uint8_t* dst_y,
	               int dst_stride_y,
	               uint8_t* dst_u,
	               int dst_stride_u,
	               uint8_t* dst_v,
	               int dst_stride_v,
	               int width,
	               int height);

	// Convert NV21 to I420.
	int (*yang_NV21ToI420)(const uint8_t* src_y,
	               int src_stride_y,
	               const uint8_t* src_vu,
	               int src_stride_vu,
	               uint8_t* dst_y,
	               int dst_stride_y,
	               uint8_t* dst_u,
	               int dst_stride_u,
	               uint8_t* dst_v,
	               int dst_stride_v,
	               int width,
	               int height);


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
	int (*yang_I420Rotate)(const uint8_t* src_y,
	               int src_stride_y,
	               const uint8_t* src_u,
	               int src_stride_u,
	               const uint8_t* src_v,
	               int src_stride_v,
	               uint8_t* dst_y,
	               int dst_stride_y,
	               uint8_t* dst_u,
	               int dst_stride_u,
	               uint8_t* dst_v,
	               int dst_stride_v,
	               int width,
	               int height,
	               enum RotationMode mode);
#else
#define yang_YUY2ToNV12 YUY2ToNV12
	#define  yang_YUY2ToI420 YUY2ToI420
	 #define yang_YUY2ToARGB YUY2ToARGB
	#define  yang_I420ToNV12 I420ToNV12

	 #define yang_I420ToNV21 I420ToNV21

	 #define yang_I420ToRGB24 I420ToRGB24
	 #define yang_NV12ToRGB24 NV12ToRGB24
	 #define yang_NV21ToRGB24 NV21ToRGB24
	 #define yang_ARGBToRGB24 ARGBToRGB24
	 #define yang_RGB24ToARGB RGB24ToARGB
	 #define yang_RAWToARGB RAWToARGB

	#define yang_NV12ToI420 NV12ToI420
	#define yang_NV21ToI420 NV21ToI420
	#define  yang_RGB24ToI420 RGB24ToI420
	#define  yang_RGBAToI420 RGBAToI420

	#define  yang_BGRAToI420 BGRAToI420
	#define  yang_ARGBToI420 ARGBToI420
	#define  yang_NV12Scale NV12Scale
	#define  yang_I420Scale I420Scale
	#define  yang_ScalePlane ScalePlane
	#define  yang_ARGBScale ARGBScale

	#define yang_I420Rotate I420Rotate
#endif
};

#endif /* YANGYUVUTIL_H_ */
