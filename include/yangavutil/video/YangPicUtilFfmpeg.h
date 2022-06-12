//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YangPicUtilFfmpeg___
#define YangPicUtilFfmpeg___
#include <yang_config.h>
#include "yangutil/sys/YangLoadLib.h"
#if HavePicUtilFfmpeg
//extern "C" {
#include "libswscale/swscale.h"
#include "libavformat/avformat.h"
//}
class YangPicUtilFfmpeg
{
public:
	YangPicUtilFfmpeg(void);
    ~YangPicUtilFfmpeg(void);
    int32_t inWidth,inHeight,outWidth,outHeight;
    void resize();
    void init(int32_t pinWidth,int32_t pinHeight,int32_t poutWidth,int32_t poutHeight,AVPixelFormat src,AVPixelFormat dst);
    void init(int32_t pinWidth,int32_t pinHeight,int32_t poutWidth,int32_t poutHeight);
    void initYuy2(int32_t pinWidth,int32_t pinHeight,int32_t poutWidth,int32_t poutHeight);
    void initNv12(int32_t pinWidth,int32_t pinHeight,int32_t poutWidth,int32_t poutHeight);
    void init420P(int32_t pinWidth,int32_t pinHeight,int32_t poutWidth,int32_t poutHeight);
    void initYuy2_Bgr24(int32_t pinWidth,int32_t pinHeight,int32_t poutWidth,int32_t poutHeight);
    void initBgr24_Yuy2(int32_t pinWidth,int32_t pinHeight,int32_t poutWidth,int32_t poutHeight);
    void getAddr(uint8_t **p_in,uint8_t **p_out);
    void closeAll();

    struct SwsContext *img_convert_ctx;
    AVPicture in_pic, out_pic;
private:

    YangLoadLib m_lib,m_lib1;
    	void loadLib();
    	void unloadLib();
    	struct SwsContext *(*yang_sws_getContext)(int32_t srcW, int32_t srcH, enum AVPixelFormat srcFormat,
    		                                  int32_t dstW, int32_t dstH, enum AVPixelFormat dstFormat,
    		                                  int32_t flags, SwsFilter *srcFilter,
    		                                  SwsFilter *dstFilter, const double *param);

    		int32_t (*yang_sws_scale)(struct SwsContext *c, const uint8_t *const srcSlice[],
    		              const int32_t srcStride[], int32_t srcSliceY, int32_t srcSliceH,
    		              uint8_t *const dst[], const int32_t dstStride[]);
    		void (*yang_sws_freeContext)(struct SwsContext *swsContext);




    		int32_t (*yang_avpicture_alloc)(AVPicture *picture, enum AVPixelFormat pix_fmt, int32_t width, int32_t height);
    		void (*yang_avpicture_free)(AVPicture *picture);
};
#endif
#endif

