#include "yangavutil/video/YangPicUtilFfmpeg.h"
#if HavePicUtilFfmpeg

void YangPicUtilFfmpeg::loadLib(){
	//yang_opus_encoder_create=(OpusEncoder *(*)(opus_int32_t Fs, int32_t channels, int32_t application,  int32_t *error))m_lib.loadFunction("");
yang_sws_getContext =(struct SwsContext *(*)(int32_t srcW, int32_t srcH, enum AVPixelFormat srcFormat,
	                                  int32_t dstW, int32_t dstH, enum AVPixelFormat dstFormat,
	                                  int32_t flags, SwsFilter *srcFilter,
	                                  SwsFilter *dstFilter, const double *param))m_lib.loadFunction("sws_getContext");

yang_sws_scale =(int32_t (*)(struct SwsContext *c, const uint8_t *const srcSlice[],
	              const int32_t srcStride[], int32_t srcSliceY, int32_t srcSliceH,
	              uint8_t *const dst[], const int32_t dstStride[]))m_lib.loadFunction("sws_scale");
yang_sws_freeContext =(void (*)(struct SwsContext *swsContext))m_lib.loadFunction("sws_freeContext");




yang_avpicture_alloc =(int32_t (*)(AVPicture *picture, enum AVPixelFormat pix_fmt, int32_t width, int32_t height))m_lib1.loadFunction("avpicture_alloc");
yang_avpicture_free =(void (*)(AVPicture *picture))m_lib1.loadFunction("avpicture_free");


}

void YangPicUtilFfmpeg::unloadLib(){
	yang_sws_getContext=NULL;
	yang_sws_scale=NULL;
	yang_sws_freeContext=NULL;
	yang_avpicture_alloc=NULL;
	yang_avpicture_free=NULL;

}
YangPicUtilFfmpeg::YangPicUtilFfmpeg(void)
{
    inWidth=0,inHeight=0,outWidth=0,outHeight=0;
    img_convert_ctx=NULL;
    unloadLib();

    //in=in_pic.data[0];
    //out=out_pic.data[0];
}


YangPicUtilFfmpeg::~YangPicUtilFfmpeg(void)
{
	closeAll();
	unloadLib();
	m_lib.unloadObject();
	m_lib1.unloadObject();
	//	m_lib2.unloadObject();
}
void YangPicUtilFfmpeg::closeAll()
{
    if(img_convert_ctx!=NULL)     yang_sws_freeContext(img_convert_ctx);
    yang_avpicture_free(&out_pic);
    yang_avpicture_free(&in_pic);
}



void YangPicUtilFfmpeg::getAddr(uint8_t **p_in,uint8_t **p_out){
    *p_in=in_pic.data[0];
    *p_out=out_pic.data[0];
}
void YangPicUtilFfmpeg::init420P(int32_t pinWidth,int32_t pinHeight,int32_t poutWidth,int32_t poutHeight)
{
	init(pinWidth,pinHeight,poutWidth,poutHeight,AV_PIX_FMT_YUV420P,AV_PIX_FMT_YUV420P);
}
void YangPicUtilFfmpeg::initYuy2(int32_t pinWidth,int32_t pinHeight,int32_t poutWidth,int32_t poutHeight)
{
	init(pinWidth,pinHeight,poutWidth,poutHeight,AV_PIX_FMT_YUYV422,AV_PIX_FMT_YUYV422);
}
void YangPicUtilFfmpeg::initNv12(int32_t pinWidth,int32_t pinHeight,int32_t poutWidth,int32_t poutHeight)
{
	init(pinWidth,pinHeight,poutWidth,poutHeight,AV_PIX_FMT_NV12,AV_PIX_FMT_NV12);
}
void YangPicUtilFfmpeg::initYuy2_Bgr24(int32_t pinWidth,int32_t pinHeight,int32_t poutWidth,int32_t poutHeight)
{
	init(pinWidth,pinHeight,poutWidth,poutHeight,AV_PIX_FMT_YUYV422,AV_PIX_FMT_BGR24);
}

void YangPicUtilFfmpeg::init(int32_t pinWidth,int32_t pinHeight,int32_t poutWidth,int32_t poutHeight,AVPixelFormat src,AVPixelFormat dst){
	m_lib.loadObject("libswscale");
	m_lib1.loadObject("libavcodec");
	//m_lib2.loadObject("libopencv_imgcodecs");
	loadLib();
	  inWidth=pinWidth;
	    inHeight=pinHeight;
	    outWidth=poutWidth;
	    outHeight=poutHeight;
	    img_convert_ctx = yang_sws_getContext(inWidth, inHeight,
	    		src, outWidth, outHeight, dst,SWS_FAST_BILINEAR,//SWS_POINT,//SWS_GAUSS,
	                                     NULL, NULL, NULL);

	    yang_avpicture_alloc(&in_pic, src, inWidth, inHeight);
	    yang_avpicture_alloc(&out_pic, dst,  outWidth, outHeight);
}
void YangPicUtilFfmpeg::initBgr24_Yuy2(int32_t pinWidth,int32_t pinHeight,int32_t poutWidth,int32_t poutHeight){
	init(pinWidth,pinHeight,poutWidth,poutHeight,AV_PIX_FMT_BGR24,AV_PIX_FMT_YUYV422);
	//sws_scale
}

void YangPicUtilFfmpeg::resize()
{
	yang_sws_scale(img_convert_ctx, in_pic.data, in_pic.linesize,  0, inHeight, out_pic.data, out_pic.linesize);

}
#endif
