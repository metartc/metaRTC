//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangavutil/video/YangYuvConvert.h>

#if Yang_Enable_YuvSo
void YangYuvConvert::loadLib() {

	yang_YUY2ToNV12 = (int32_t (*)(const uint8_t *src_yuy2, int32_t src_stride_yuy2,
			uint8_t *dst_y, int32_t dst_stride_y, uint8_t *dst_uv,
			int32_t dst_stride_uv, int32_t width, int32_t height)) m_lib.loadFunction(
			"YUY2ToNV12");
	yang_YUY2ToI420 =(int32_t (*)(const uint8_t *src_yuy2, int32_t src_stride_yuy2,
					uint8_t *dst_y, int32_t dst_stride_y, uint8_t *dst_u,
					int32_t dst_stride_u, uint8_t *dst_v, int32_t dst_stride_v,
					int32_t width, int32_t height)) m_lib.loadFunction("YUY2ToI420");

	yang_YUY2ToARGB=(int (*)(const uint8_t* src_yuy2,
	               int src_stride_yuy2,
	               uint8_t* dst_argb,
	               int dst_stride_argb,
	               int width,
	               int height)) m_lib.loadFunction("YUY2ToARGB");
	yang_I420ToNV12=(int32_t (*)(const uint8_t* src_y, int32_t src_stride_y, const uint8_t* src_u,
	               int32_t src_stride_u,const uint8_t* src_v,int32_t src_stride_v,	uint8_t* dst_y,
	               int32_t dst_stride_y, uint8_t* dst_uv,  int32_t dst_stride_uv,  int32_t width, int32_t height)) m_lib.loadFunction("I420ToNV12");

	yang_I420ToNV21=(int32_t (*)(const uint8_t* src_y,
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
	               int32_t height)) m_lib.loadFunction("I420ToNV21");

	yang_I420ToRGB24 = (int32_t (*)(const uint8_t *src_y, int32_t src_stride_y,
			const uint8_t *src_u, int32_t src_stride_u, const uint8_t *src_v,
			int32_t src_stride_v, uint8_t *dst_rgb24, int32_t dst_stride_rgb24,
			int32_t width, int32_t height)) m_lib.loadFunction("I420ToRGB24");
	yang_NV12ToRGB24 = (int32_t (*)(const uint8_t *src_y, int32_t src_stride_y,
			const uint8_t *src_uv, int32_t src_stride_uv, uint8_t *dst_rgb24,
			int32_t dst_stride_rgb24, int32_t width, int32_t height)) m_lib.loadFunction(
			"NV12ToRGB24");
	yang_NV21ToRGB24 = (int32_t (*)(const uint8_t *src_y, int32_t src_stride_y,
			const uint8_t *src_vu, int32_t src_stride_vu, uint8_t *dst_rgb24,
			int32_t dst_stride_rgb24, int32_t width, int32_t height)) m_lib.loadFunction(
			"NV21ToRGB24");
	yang_ARGBToRGB24 =
			(int32_t (*)(const uint8_t *src_argb, int32_t src_stride_argb,
					uint8_t *dst_rgb24, int32_t dst_stride_rgb24, int32_t width,
					int32_t height)) m_lib.loadFunction("ARGBToRGB24");


	yang_NV12ToI420=(int (*)(const uint8_t* src_y,
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
	               int height)) m_lib.loadFunction("NV12ToI420");

	// Convert NV21 to I420.
	yang_NV21ToI420=(int (*)(const uint8_t* src_y,
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
	               int height)) m_lib.loadFunction("NV21ToI420");


	yang_RGB24ToI420 =
			(int32_t (*)(const uint8_t *src_rgb24, int32_t src_stride_rgb24,
					uint8_t *dst_y, int32_t dst_stride_y, uint8_t *dst_u,
					int32_t dst_stride_u, uint8_t *dst_v, int32_t dst_stride_v,
					int32_t width, int32_t height)) m_lib.loadFunction("RGB24ToI420");

	yang_RGBAToI420=(int (*)(const uint8_t* src_rgba,
	               int src_stride_rgba,  uint8_t* dst_y,  int dst_stride_y,
	               uint8_t* dst_u,   int dst_stride_u,      uint8_t* dst_v,
	               int dst_stride_v,  int width,  int height)) m_lib.loadFunction("RGBAToI420");
	yang_BGRAToI420=(int (*)(const uint8_t* src_bgra,  int src_stride_bgra,
	               uint8_t* dst_y, int dst_stride_y, uint8_t* dst_u, int dst_stride_u,
				   uint8_t* dst_v, int dst_stride_v,int width, int height)) m_lib.loadFunction("BGRAToI420");
	yang_ARGBToI420=(int (*)(const uint8_t* src_argb, int src_stride_argb,
	               uint8_t* dst_y, int dst_stride_y, uint8_t* dst_u,int dst_stride_u,
	               uint8_t* dst_v,  int dst_stride_v, int width,int height)) m_lib.loadFunction("ARGBToI420");
	yang_RGB24ToARGB=(int32_t (*)(const uint8_t* src_rgb24,
	                int32_t src_stride_rgb24,
	                uint8_t* dst_argb,
	                int32_t dst_stride_argb,
	                int32_t width,
	                int32_t height)) m_lib.loadFunction("RGB24ToARGB");
	yang_RAWToARGB=(int32_t (*)(const uint8_t* src_raw,
	              int32_t src_stride_raw,
	              uint8_t* dst_argb,
	              int32_t dst_stride_argb,
	              int32_t width,
	              int32_t height)) m_lib.loadFunction("RAWToARGB");
	yang_NV12Scale = (int32_t (*)(const uint8_t *src_y, int32_t src_stride_y,
			const uint8_t *src_uv, int32_t src_stride_uv, int32_t src_width,
			int32_t src_height, uint8_t *dst_y, int32_t dst_stride_y, uint8_t *dst_uv,
			int32_t dst_stride_uv, int32_t dst_width, int32_t dst_height,
			enum FilterMode filtering)) m_lib.loadFunction("NV12Scale");
	yang_I420Scale = (int32_t (*)(const uint8_t *src_y, int32_t src_stride_y,
			const uint8_t *src_u, int32_t src_stride_u, const uint8_t *src_v,
			int32_t src_stride_v, int32_t src_width, int32_t src_height, uint8_t *dst_y,
			int32_t dst_stride_y, uint8_t *dst_u, int32_t dst_stride_u, uint8_t *dst_v,
			int32_t dst_stride_v, int32_t dst_width, int32_t dst_height,
			enum FilterMode filtering)) m_lib.loadFunction("I420Scale");
	yang_ScalePlane =
			(void (*)(const uint8_t *src, int32_t src_stride, int32_t src_width,
					int32_t src_height, uint8_t *dst, int32_t dst_stride, int32_t dst_width,
					int32_t dst_height, enum FilterMode filtering)) m_lib.loadFunction(
					"ScalePlane");
	yang_ARGBScale=(int32_t (*)(const uint8_t* src_argb,
	              int32_t src_stride_argb,
	              int32_t src_width,
	              int32_t src_height,
	              uint8_t* dst_argb,
	              int32_t dst_stride_argb,
	              int32_t dst_width,
	              int32_t dst_height,
	              enum FilterMode filtering)) m_lib.loadFunction(
	  					"ARGBScale");

	yang_I420Rotate=(int (*)(const uint8_t* src_y,
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
	               enum RotationMode mode))m_lib.loadFunction(
		  					"I420Rotate");
}

void YangYuvConvert::unloadLib() {
	yang_YUY2ToNV12 = NULL;
	yang_YUY2ToI420 = NULL;
	yang_YUY2ToARGB = NULL;
	yang_I420ToNV12 = NULL;
	yang_I420ToNV21 = NULL;
	yang_I420ToRGB24 = NULL;
	yang_NV12ToRGB24 = NULL;
	yang_NV21ToRGB24 = NULL;
	yang_ARGBToRGB24 = NULL;
	yang_RGB24ToARGB=NULL;
	yang_RAWToARGB=NULL;
	yang_NV21ToI420=NULL;
	yang_NV12ToI420=NULL;
	yang_RGB24ToI420 = NULL;
	yang_BGRAToI420=NULL;
	yang_ARGBToI420=NULL;
	yang_NV12Scale = NULL;
	yang_I420Scale = NULL;
	yang_ScalePlane = NULL;
	yang_RGBAToI420=NULL;
	yang_ARGBScale=NULL;
	yang_I420Rotate=NULL;

}
#endif
YangYuvConvert::YangYuvConvert() {
#if Yang_Enable_YuvSo
	m_lib.loadObject("libyuv");
	loadLib();
#endif
}

YangYuvConvert::~YangYuvConvert() {
#if Yang_Enable_YuvSo
	unloadLib();
	m_lib.unloadObject();
#endif
}
int32_t YangYuvConvert::i420toyv12(uint8_t *src,uint8_t *dst,int32_t width,int32_t height)
{
    yang_memcpy(dst,src,width*height);//y分量
    yang_memcpy(dst+width*height,src+width*height+width*height/4,width*height/4);//V分量
    yang_memcpy(dst+width*height+width*height/4,src+width*height,width*height/4);//u分量
    return 0;
}
int32_t YangYuvConvert::yuy2tonv12(uint8_t *src, uint8_t *dst, int32_t width, int32_t height) {
	return yang_YUY2ToNV12((const uint8_t*) src, width << 1, dst, width,
			dst + (width * height), width, width, height);

}
int32_t YangYuvConvert::yuy2toI420(uint8_t *src, uint8_t *dst, int32_t width, int32_t height) {

	return yang_YUY2ToI420((const uint8_t*) src, width << 1, dst, width,
			dst + (width * height), (width >> 1),
			dst + (width * height) + (int) (width * height / 4), (width >> 1),
			width, height);

}
int32_t YangYuvConvert::nv12toI420(uint8_t* src,uint8_t *dst,int32_t width,int32_t height){
	return yang_NV12ToI420((const uint8_t*) src, width ,src+width*height,width,
			dst, width,	 (dst + (width * height)), (width >> 1),
			 (dst + (width * height)+ (int) (width * height / 4)), (width >> 1),
				width, height);
}
int32_t YangYuvConvert::nv21toI420(uint8_t* src,uint8_t *dst,int32_t width,int32_t height){
	return yang_NV21ToI420((const uint8_t*) src, width ,src+width*height,width,
			dst, width,	 (dst + (width * height)), (width >> 1),
			 (dst + (width * height)+ (int) (width * height / 4)), (width >> 1),
				width, height);
}
int32_t YangYuvConvert::yuy2toargb(uint8_t *src, uint8_t *dst, int32_t width, int32_t height) {
	return yang_YUY2ToARGB((const uint8_t*) src, width << 1,
			dst,width * 4,
			width, height);

}

int32_t YangYuvConvert::i420tonv12(uint8_t* src,uint8_t *dst,int32_t width,int32_t height){
	return yang_I420ToNV12((const uint8_t*) src, width,
			(const uint8_t*) (src + (width * height)), (width >> 1),
			(const uint8_t*) (src + (width * height)+ (int) (width * height / 4)), (width >> 1),
			dst, width ,dst+width*height,width,
			width, height);
}
int32_t YangYuvConvert::i420tonv21(uint8_t* src,uint8_t *dst,int32_t width,int32_t height){
	return yang_I420ToNV21((const uint8_t*) src, width,
			(const uint8_t*) (src + (width * height)), (width >> 1),
			(const uint8_t*) (src + (width * height)+ (int) (width * height / 4)), (width >> 1),
			dst, width ,dst+width*height,width,
			width, height);
}


int32_t YangYuvConvert::I420torgb24(uint8_t *src, uint8_t *dst, int32_t width,
		int32_t height) {
	return yang_I420ToRGB24((const uint8_t*) src, width,
			(const uint8_t*) (src + (width * height)), (width >> 1),
			(const uint8_t*) (src + (width * height)
					+ (int) (width * height / 4)), (width >> 1), dst, width * 3,
			width, height);
}
int32_t YangYuvConvert::nv12torgb24(uint8_t *src, uint8_t *dst, int32_t width,
		int32_t height) {
	return yang_NV12ToRGB24((const uint8_t*) src, width,
			(const uint8_t*) (src + (width * height)), width, dst,
			width * 3, width, height);
}
int32_t YangYuvConvert::nv21torgb24(uint8_t *src, uint8_t *dst, int32_t width,
		int32_t height) {
	return yang_NV21ToRGB24((const uint8_t*) src, width,
			(const uint8_t*) (src + (width * height)), width, dst,
			width * 3, width, height);
}
int32_t YangYuvConvert::argbtorgb24(uint8_t *src, uint8_t *dst, int32_t width,
		int32_t height) {
	return yang_ARGBToRGB24((const uint8_t*) src, width * 4, dst,
			width * 3, width, height);
}
int32_t YangYuvConvert::rgb24toargb(uint8_t *src, uint8_t *dst, int32_t width,
		int32_t height) {

	return yang_RGB24ToARGB((const uint8_t*) src, width * 3, dst,
			width * 4, width, height);
}
int32_t YangYuvConvert::rgb24toI420(uint8_t *src_rgb24, uint8_t *dst, int32_t width,
		int32_t height) {
	return yang_RGB24ToI420((const uint8_t*) src_rgb24, width * 3, dst,
			width, dst + (width * height), (width >> 1),
			dst + (width * height) + (int) (width * height / 4), (width >> 1),
			width, height);
}

int32_t YangYuvConvert::rgbatoI420(uint8_t* src_rgba,uint8_t *dst,int32_t width,int32_t height) {
	return yang_RGBAToI420((const uint8_t*) src_rgba, width * 4, dst,
			width, dst + (width * height), (width >> 1),
			dst + (width * height) + (int) (width * height / 4), (width >> 1),
			width, height);
}
int32_t YangYuvConvert::bgratoI420(uint8_t* src_bgra,uint8_t *dst,int32_t width,int32_t height) {
	//return yang_BGRAToI420((const uint8_t*) src_bgra, width * 4, dst,
	return yang_ARGBToI420((const uint8_t*) src_bgra, width * 4, dst,
			width, dst + (width * height), (width >> 1),
			dst + (width * height) + (int) (width * height / 4), (width >> 1),
			width, height);
}
int32_t YangYuvConvert::scaleNv12(uint8_t *src, uint8_t *dst, int32_t srcWidth,
		int32_t srcHeight, int32_t dstWidth, int32_t dstHeight, int32_t mode) {
	return yang_NV12Scale((const uint8_t*) src, srcWidth,
			(const uint8_t*) src + srcWidth * srcHeight, srcWidth, srcWidth,
			srcHeight, dst, dstWidth, dst + dstWidth * dstHeight, dstWidth,
			dstWidth, dstHeight, (libyuv::FilterMode) mode);
}
int32_t YangYuvConvert::scaleI420(uint8_t *src, uint8_t *dst, int32_t srcWidth,
		int32_t srcHeight, int32_t dstWidth, int32_t dstHeight, int32_t mode) {
	int32_t srcLen=srcWidth*srcHeight;
	int32_t dstLen=dstWidth*dstHeight;
	return yang_I420Scale((const uint8_t*) src, srcWidth,
			(const uint8_t*) (src + srcLen), srcWidth>>1,
			(const uint8_t*) (src + srcLen*5/4),srcWidth>>1,
			srcWidth, srcHeight,
			dst, dstWidth,
			dst + dstLen, dstWidth>>1,
			dst + dstLen*5/4, dstWidth>>1,
			dstWidth, dstHeight, (libyuv::FilterMode) mode);
}

int32_t YangYuvConvert::scaleYuy2(uint8_t *src, uint8_t *dst, int32_t srcWidth,
		int32_t srcHeight, int32_t dstWidth, int32_t dstHeight, int32_t mode) {
	yang_ScalePlane((const uint8_t*) src, srcWidth << 1, srcWidth, srcHeight,
			dst, dstWidth << 1, dstWidth, dstHeight, (libyuv::FilterMode) mode);
	return 0;
}

int32_t YangYuvConvert::scaleArgb(uint8_t* src,uint8_t *dst,int32_t srcWidth,int32_t srcHeight,int32_t dstWidth,int32_t dstHeight,int32_t mode){
	return yang_ARGBScale((const uint8_t*)src,srcWidth*4,
	srcWidth,srcHeight,
	dst,dstWidth*4,
	dstWidth,dstHeight,
	(libyuv::FilterMode)mode
	);
}
int32_t YangYuvConvert::rotateI420(uint8_t* src,uint8_t *dst,int32_t width,int32_t height,RotationMode mode){
	int32_t srcLen=width*height;
	return yang_I420Rotate((const uint8_t*) src, width,
			(const uint8_t*) (src + srcLen), width>>1,
			(const uint8_t*) (src + srcLen*5/4),width>>1,
			dst, width,
			 (dst + srcLen), width>>1,
			(dst + srcLen*5/4),width>>1,
			width,height,mode
	);
}


