//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangutil/sys/YangLog.h>
#include <yangavutil/video/YangMeta.h>
#include <yangencoder/YangFfmpegEncoderMeta.h>
#include <yangencoder/YangVideoEncoderFfmpeg.h>
#if Yang_Enable_Ffmpeg_Codec
YangFfmpegEncoderMeta::YangFfmpegEncoderMeta() {
#if Yang_Enable_FfmpegSo
	unloadLib();
#endif

}

YangFfmpegEncoderMeta::~YangFfmpegEncoderMeta() {
#if Yang_Enable_FfmpegSo
	unloadLib();
	m_lib.unloadObject();
	m_lib1.unloadObject();
#endif
}

#if Yang_Enable_FfmpegSo
void YangFfmpegEncoderMeta::loadLib() {
	yang_av_buffer_unref = (void (*)(AVBufferRef **buf)) m_lib1.loadFunction(
				"av_buffer_unref");
		yang_av_hwframe_ctx_init = (int32_t (*)(AVBufferRef *ref)) m_lib1.loadFunction(
				"av_hwframe_ctx_init");
		yang_av_frame_alloc = (AVFrame* (*)(void)) m_lib1.loadFunction(
				"av_frame_alloc");
		yang_av_image_get_buffer_size = (int32_t (*)(enum AVPixelFormat pix_fmt,
				int32_t width, int32_t height, int32_t align)) m_lib1.loadFunction(
				"av_image_get_buffer_size");
		yang_av_hwdevice_ctx_create = (int32_t (*)(AVBufferRef **device_ctx,
				enum AVHWDeviceType type, const char *device, AVDictionary *opts,
				int32_t flags)) m_lib1.loadFunction("av_hwdevice_ctx_create");
		yang_av_hwframe_transfer_data = (int32_t (*)(AVFrame *dst, const AVFrame *src,
				int32_t flags)) m_lib1.loadFunction("av_hwframe_transfer_data");
		yang_av_free = (void (*)(void *ptr)) m_lib1.loadFunction("av_free");
		yang_av_frame_free = (void (*)(AVFrame **frame)) m_lib1.loadFunction(
				"av_frame_free");
		yang_av_buffer_ref =
				(AVBufferRef* (*)(AVBufferRef *buf)) m_lib1.loadFunction(
						"av_buffer_ref");
		yang_av_image_fill_arrays = (int32_t (*)(uint8_t *dst_data[4],
				int32_t dst_linesize[4], const uint8_t *src, enum AVPixelFormat pix_fmt,
				int32_t width, int32_t height, int32_t align)) m_lib1.loadFunction(
				"av_image_fill_arrays");
		yang_av_hwframe_ctx_alloc =
				(AVBufferRef* (*)(AVBufferRef *device_ctx)) m_lib1.loadFunction(
						"av_hwframe_ctx_alloc");
		yang_av_hwframe_get_buffer = (int32_t (*)(AVBufferRef *hwframe_ctx,
				AVFrame *frame, int32_t flags)) m_lib1.loadFunction(
				"av_hwframe_get_buffer");
		yang_av_malloc = (void* (*)(size_t size)) m_lib1.loadFunction("av_malloc");

		yang_avcodec_alloc_context3 =
				(AVCodecContext* (*)(const AVCodec *codec)) m_lib.loadFunction(
						"avcodec_alloc_context3");
		yang_av_init_packet = (void (*)(AVPacket *pkt)) m_lib.loadFunction(
				"av_init_packet");
		yang_avcodec_find_encoder_by_name =
				(AVCodec* (*)(const char *name)) m_lib.loadFunction(
						"avcodec_find_encoder_by_name");
		yang_avcodec_open2 = (int32_t (*)(AVCodecContext *avctx, const AVCodec *codec,
				AVDictionary **options)) m_lib.loadFunction("avcodec_open2");
		yang_avcodec_send_frame = (int32_t (*)(AVCodecContext *avctx,
				const AVFrame *frame)) m_lib.loadFunction("avcodec_send_frame");
		yang_avcodec_receive_packet = (int32_t (*)(AVCodecContext *avctx,
				AVPacket *avpkt)) m_lib.loadFunction("avcodec_receive_packet");
		yang_avcodec_close = (int32_t (*)(AVCodecContext *avctx)) m_lib.loadFunction(
				"avcodec_close");
}
void YangFfmpegEncoderMeta::unloadLib() {
	yang_av_hwframe_ctx_alloc = NULL;
	yang_av_hwframe_ctx_init = NULL;
	yang_av_buffer_unref = NULL;
	yang_avcodec_find_encoder_by_name = NULL;
	yang_av_hwdevice_ctx_create = NULL;
	yang_av_frame_alloc = NULL;
	yang_avcodec_open2 = NULL;
	yang_av_image_get_buffer_size = NULL;
	yang_av_malloc = NULL;
	yang_av_image_fill_arrays = NULL;
	yang_av_init_packet = NULL;
	yang_av_hwframe_get_buffer = NULL;
	yang_av_hwframe_transfer_data = NULL;
	yang_avcodec_send_frame = NULL;
	yang_avcodec_receive_packet = NULL;
	yang_av_frame_free = NULL;
	yang_avcodec_close = NULL;
	yang_av_free = NULL;
}
#endif
#define HEX2BIN(a)      (((a)&0x40)?((a)&0xf)+9:((a)&0xf))

void YangFfmpegEncoderMeta::yang_find_next_start_code(YangVideoCodec pve,uint8_t *buf,int32_t bufLen,int32_t *vpsPos,int32_t *vpsLen,int32_t *spsPos,int32_t *spsLen,int32_t *ppsPos,int32_t *ppsLen)
{
   int32_t i = 0;

   *spsPos=0;*ppsPos=0;
   if(pve==Yang_VED_H265)	  {
	   *vpsPos=0;
	   while(i<bufLen-3){
		      if (buf[i] == 0 && buf[i + 1] == 0 &&buf[i + 2] == 0&& buf[i + 3] == 1){
		       	*vpsPos=i+4;
		       	i+=4;
		       	 break;
		       }
		      i++;
	   }
   }
   while (i <bufLen-3) {
       if (buf[i] == 0 && buf[i + 1] == 0 &&buf[i + 2] == 0&& buf[i + 3] == 1){
    	   if(pve==Yang_VED_H265) *vpsLen=i-4;
       	*spsPos=i+4;
       	i+=4;
       	 break;
       }

       i++;
   }
   while (i <bufLen-3) {
       if (buf[i] == 0 && buf[i + 1] == 0 &&buf[i + 2] == 0&& buf[i + 3] == 1){
       	*spsLen=i-*spsPos;
       	*ppsPos=i+4;
       	*ppsLen=bufLen-*ppsPos;
       	 break;
       }

       i++;
   }

}


int32_t YangFfmpegEncoderMeta::set_hwframe_ctx(AVPixelFormat ctxformat,AVPixelFormat swformat,YangVideoInfo *yvp,AVCodecContext *ctx, AVBufferRef *hw_device_ctx,int32_t pwid,int32_t phei) {
	AVBufferRef *hw_frames_ref;
	AVHWFramesContext *frames_ctx = NULL;
	int32_t err = 0;
	int32_t ret=0;
	if (!(hw_frames_ref = yang_av_hwframe_ctx_alloc(hw_device_ctx))) {
        yang_error("Failed to create VAAPI frame context.\n");
		return -1;
	}
	frames_ctx = (AVHWFramesContext*) (hw_frames_ref->data);
	frames_ctx->format = ctxformat;
	frames_ctx->sw_format = swformat;

	frames_ctx->width = pwid;
	frames_ctx->height = phei;
	frames_ctx->initial_pool_size = 0;
	if ((err = yang_av_hwframe_ctx_init(hw_frames_ref)) < 0) {
        yang_error("Failed to initialize VAAPI frame context.Error code: %d\n",
				ret);
		yang_av_buffer_unref(&hw_frames_ref);
		return err;
	}
	ctx->hw_frames_ctx = yang_av_buffer_ref(hw_frames_ref);
	ctx->hw_device_ctx = yang_av_buffer_ref(hw_device_ctx);
	// ctx->hwaccel_flags=1;
	if (!ctx->hw_frames_ctx)
		err = AVERROR(ENOMEM);

	yang_av_buffer_unref(&hw_frames_ref);
	return err;
}

enum AVPixelFormat get_hw_format22(AVCodecContext *ctx,
		const enum AVPixelFormat *pix_fmts) {
	if(YangVideoEncoderFfmpeg::g_hwType==YangV_Hw_Intel) return AV_PIX_FMT_VAAPI;
	if(YangVideoEncoderFfmpeg::g_hwType==YangV_Hw_Nvdia) return AV_PIX_FMT_CUDA;
	if(YangVideoEncoderFfmpeg::g_hwType==YangV_Hw_Android) return AV_PIX_FMT_MEDIACODEC;
	return AV_PIX_FMT_VAAPI;
}
void YangFfmpegEncoderMeta::yang_getSpsPps(YangH2645Conf *pconf,
		YangVideoInfo *p_yvp, YangVideoEncInfo *penc) {
#if Yang_Enable_FfmpegSo
	m_lib.loadObject("libavcodec");
		m_lib1.loadObject("libavutil");
		loadLib();
#endif
		YangVideoCodec m_encoderType=(YangVideoCodec)p_yvp->videoEncoderType;
		YangVideoHwType m_hwType=(YangVideoHwType)p_yvp->videoEncHwType;
		AVCodec *m_codec=NULL;
		 AVCodecContext *m_codecCtx = NULL;
				 AVBufferRef *hw_device_ctx=NULL;
					//hevc_vaapi nvenc nvdec vdpau h264_nvenc
				if(m_encoderType==Yang_VED_H264){
					if(m_hwType==YangV_Hw_Intel)	m_codec = yang_avcodec_find_encoder_by_name("h264_vaapi");//avcodec_find_encoder(AV_CODEC_ID_H264);
					if(m_hwType==YangV_Hw_Nvdia)	m_codec = yang_avcodec_find_encoder_by_name("h264_nvenc");
					if(m_hwType==YangV_Hw_Android)	m_codec = yang_avcodec_find_encoder_by_name("h264_mediacodec");
				}else if(m_encoderType==Yang_VED_H265){
					if(m_hwType==YangV_Hw_Intel)	m_codec = yang_avcodec_find_encoder_by_name("hevc_vaapi");
					if(m_hwType==YangV_Hw_Nvdia) 	m_codec = yang_avcodec_find_encoder_by_name("hevc_nvenc");
					if(m_hwType==YangV_Hw_Android)	m_codec = yang_avcodec_find_encoder_by_name("hevc_mediacodec");
				}
			m_codecCtx = yang_avcodec_alloc_context3(m_codec);
			YangVideoEncoderFfmpeg::initParam(m_codecCtx,p_yvp,penc);
			m_codecCtx->get_format = get_hw_format22;	// AV_PIX_FMT_NV12;//get_hw_format;
			int32_t ret=0;
			//AV_HWDEVICE_TYPE_CUDA
			YangVideoEncoderFfmpeg::g_hwType=(YangVideoHwType)p_yvp->videoEncHwType;
			if(YangVideoEncoderFfmpeg::g_hwType==YangV_Hw_Intel){
				ret = yang_av_hwdevice_ctx_create(&hw_device_ctx, AV_HWDEVICE_TYPE_VAAPI,"/dev/dri/renderD128", NULL, 0);
				m_codecCtx->pix_fmt = AV_PIX_FMT_VAAPI;
			}else if(YangVideoEncoderFfmpeg::g_hwType==YangV_Hw_Nvdia){
				ret = yang_av_hwdevice_ctx_create(&hw_device_ctx, AV_HWDEVICE_TYPE_CUDA,"CUDA", NULL, 0);
				m_codecCtx->pix_fmt = AV_PIX_FMT_CUDA;
			}else if(YangVideoEncoderFfmpeg::g_hwType==YangV_Hw_Android){
				ret = yang_av_hwdevice_ctx_create(&hw_device_ctx, AV_HWDEVICE_TYPE_MEDIACODEC,"MEDIACODEC", NULL, 0);
				m_codecCtx->pix_fmt = AV_PIX_FMT_MEDIACODEC;
			}
			//YangVideoEncoderFfmpeg::g_hwType=m_codecCtx->pix_fmt ;
			if(ret<0){
				printf("\nhw create error!..ret=%d\n",ret);
				exit(1);
			}

			AVPixelFormat ctxformat,swformat;
					if(p_yvp->videoEncHwType==YangV_Hw_Intel) ctxformat = AV_PIX_FMT_VAAPI;
					if(p_yvp->videoEncHwType==YangV_Hw_Nvdia) ctxformat = AV_PIX_FMT_CUDA;
					if(p_yvp->videoEncHwType==YangV_Hw_Android) ctxformat = AV_PIX_FMT_MEDIACODEC;
						if(p_yvp->bitDepth==8) swformat = AV_PIX_FMT_NV12;
						if(p_yvp->bitDepth==10) swformat = AV_PIX_FMT_P010;
						if(p_yvp->bitDepth==16) swformat = AV_PIX_FMT_P016;
			if ((ret = set_hwframe_ctx(ctxformat,swformat,p_yvp,m_codecCtx, hw_device_ctx, p_yvp->outWidth,
					p_yvp->outHeight)) < 0) {
				printf("Failed to set hwframe context.\n");
				//goto close;
			}

			m_codecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
			ret = yang_avcodec_open2(m_codecCtx, m_codec, NULL);
			if (ret < 0){
				yang_error("avcodec_open2 failure");
				exit(1);
			}
			int32_t vpsPos=0,vpsLen=0;
				int32_t spsPos=0,ppsPos=0;
				int32_t spsLen=0,ppsLen=0;
				yang_find_next_start_code(m_encoderType,m_codecCtx->extradata,m_codecCtx->extradata_size,&vpsPos,&vpsLen,&spsPos,&spsLen,&ppsPos,&ppsLen);
				if(m_encoderType==Yang_VED_H265) {
					pconf->vpsLen=vpsLen;
					memcpy(pconf->vps,m_codecCtx->extradata+vpsPos,vpsLen);

				}
				pconf->spsLen=spsLen;
				pconf->ppsLen=ppsLen;
				memcpy(pconf->sps,m_codecCtx->extradata+spsPos,spsLen);
				memcpy(pconf->pps,m_codecCtx->extradata+ppsPos,ppsLen);


			yang_av_buffer_unref(&hw_device_ctx);
			if (m_codecCtx){
				yang_avcodec_close(m_codecCtx);
				yang_av_free(m_codecCtx);
			}
			m_codecCtx = NULL;

}



void YangFfmpegEncoderMeta::yang_initVmd(YangVideoMeta *p_vmd,
		YangVideoInfo *p_yvp, YangVideoEncInfo *penc) {
	if (!p_vmd->isInit) {

		yang_getSpsPps(&p_vmd->mp4Meta, p_yvp,penc);
		if(p_yvp->videoEncoderType==Yang_VED_H264) yang_getConfig_Flv_H264(&p_vmd->mp4Meta, p_vmd->livingMeta.buffer,&p_vmd->livingMeta.bufLen);
		if(p_yvp->videoEncoderType==Yang_VED_H265) yang_getConfig_Flv_H265(&p_vmd->mp4Meta, p_vmd->livingMeta.buffer,&p_vmd->livingMeta.bufLen);
	//	yang_getH265Config_Flv(&p_vmd->mp4Meta, p_vmd->flvMeta.buffer,		&p_vmd->flvMeta.bufLen);
		p_vmd->isInit = 1;
	}
}
#endif
