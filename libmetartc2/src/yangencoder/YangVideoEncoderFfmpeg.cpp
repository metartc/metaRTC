#include "YangVideoEncoderFfmpeg.h"
#include "YangH264EncHeader.h"
#if Yang_Ffmpeg_UsingSo
void YangVideoEncoderFfmpeg::loadLib() {
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
void YangVideoEncoderFfmpeg::unloadLib() {
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
enum AVPixelFormat get_hw_format(AVCodecContext *ctx,
		const enum AVPixelFormat *pix_fmts) {
	if(YangVideoEncoderFfmpeg::g_hwType==YangV_Hw_Intel) return AV_PIX_FMT_VAAPI;
	if(YangVideoEncoderFfmpeg::g_hwType==YangV_Hw_Nvdia) return AV_PIX_FMT_CUDA;
	if(YangVideoEncoderFfmpeg::g_hwType==YangV_Hw_Android) return AV_PIX_FMT_MEDIACODEC;
	return AV_PIX_FMT_VAAPI;
	/** const enum AVPixelFormat *p;

	 for (p = pix_fmts; *p != -1; p++) {
	 if (*p == hw_pix_fmt)
	 return *p;
	 }

	 fprintf(stderr, "Failed to get HW surface format.\n");
	 return AV_PIX_FMT_NONE;**/
}
YangVideoHwType YangVideoEncoderFfmpeg::g_hwType=YangV_Hw_Intel;
int32_t YangVideoEncoderFfmpeg::set_hwframe_ctx(AVPixelFormat ctxformat,AVPixelFormat swformat,YangVideoInfo *yvp,AVCodecContext *ctx,
		AVBufferRef *hw_device_ctx, int32_t pwid, int32_t phei) {
	AVBufferRef *hw_frames_ref;
	AVHWFramesContext *frames_ctx = NULL;
	int32_t err = 0;

	if (!(hw_frames_ref = yang_av_hwframe_ctx_alloc(hw_device_ctx))) {
		printf("Failed to create VAAPI frame context.\n");
		return -1;
	}
	frames_ctx = (AVHWFramesContext*) (hw_frames_ref->data);
	frames_ctx->format = ctxformat;
	frames_ctx->sw_format = swformat;
	frames_ctx->width = pwid;
	frames_ctx->height = phei;
	frames_ctx->initial_pool_size = 20;
	if ((err = yang_av_hwframe_ctx_init(hw_frames_ref)) < 0) {
		printf("Failed to initialize VAAPI frame context.Error code: %d\n",
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

YangVideoEncoderFfmpeg::YangVideoEncoderFfmpeg(int32_t ptype,int32_t phwtype) {
	usingVaapi = 1; //pcontext->usingHwDec == 2 ? 1 : 0;
	m_encoderType=(YangVideoEncDecType)ptype;
	g_hwType=(YangVideoHwType)phwtype;
	m_frame = NULL;
	yLen =m_videoInfo.outWidth * m_videoInfo.outHeight;
	uLen = yLen / 4;
	allLen = yLen * 3 / 2;
	buffer = NULL;
	ret = 0;
	m_codec = NULL;
	hw_device_ctx = NULL;
	frame_mem_gpu = NULL;
    m_usingHw=true;
#if Yang_Ffmpeg_UsingSo
	unloadLib();
#endif

}
YangVideoEncoderFfmpeg::~YangVideoEncoderFfmpeg() {
	encode_close();
#if Yang_Ffmpeg_UsingSo
	unloadLib();
	m_lib.unloadObject();
	m_lib1.unloadObject();
#endif

}
void YangVideoEncoderFfmpeg::sendKeyFrame(){
	m_sendKeyframe=1;
}
void YangVideoEncoderFfmpeg::setVideoMetaData(YangVideoMeta *pvmd) {

	/**setZbVmd_f(m_codecCtx->extradata, m_codecCtx->extradata_size,
			pvmd->livingMeta.buffer, &pvmd->livingMeta.bufLen);**/
		//printf("\n....%d\n",m_codecCtx->extradata_size);
	// for(int32_t i=0;i<m_codecCtx->extradata_size;i++){
	// printf("%02x,",*(m_codecCtx->extradata+i));
	// }
}
void YangVideoEncoderFfmpeg::initParam(AVCodecContext *p_codecCtx,YangVideoInfo *pvp,YangVideoEncInfo *penc){
	p_codecCtx->bit_rate = 1000 *pvp->rate;
	//p_codecCtx->global_quality
	printf("\nbitrate===%d\n",(int)p_codecCtx->bit_rate );
	p_codecCtx->width = pvp->outWidth;
	p_codecCtx->height = pvp->outHeight;
	p_codecCtx->profile = pvp->videoEncoderType==Yang_VED_264?FF_PROFILE_H264_CONSTRAINED_BASELINE:FF_PROFILE_HEVC_MAIN;	//66;
	//p_codecCtx->gop_size = pvp->frame;
	//p_codecCtx->level = penc->level_idc;

    p_codecCtx->time_base.den=1;// = (AVRational){1, pvp->frame };
    p_codecCtx->time_base.num=pvp->frame;
    p_codecCtx->framerate.den = pvp->frame;//(AVRational){pvp->frame , 1};
    p_codecCtx->framerate.num=1;
	//p_codecCtx->time_base = (AVRational ) { 1, pvp->frame };
	if(pvp->videoEncoderType==Yang_VED_264)	p_codecCtx->has_b_frames=0;

}
int32_t YangVideoEncoderFfmpeg::init(YangVideoInfo *pvp,YangVideoEncInfo *penc) {
	//AVCodecID codec_id = AV_CODEC_ID_H264;
	// av_register_all();
	// avcodec_register_all();
	// avcodec_register(AV_CODEC_ID_H264);
	//m_codec = avcodec_find_encoder_by_name("h264_vaapi");//avcodec_find_encoder(AV_CODEC_ID_H264);
#if Yang_Ffmpeg_UsingSo
	m_lib.loadObject("libavcodec");
	m_lib1.loadObject("libavutil");
	loadLib();
#endif
	setVideoPara(pvp,penc);
	usingVaapi=1;
	yLen =m_videoInfo.outWidth * m_videoInfo.outHeight;
		uLen = yLen / 4;
		allLen = yLen * 3 / 2;
		//hevc_vaapi nvenc nvdec vdpau h264_nvenc
		if(m_encoderType==Yang_VED_264){
			if(g_hwType==YangV_Hw_Intel)	m_codec = yang_avcodec_find_encoder_by_name("h264_vaapi");//avcodec_find_encoder(AV_CODEC_ID_H264);
            if(g_hwType==YangV_Hw_Nvdia)	{
                m_codec = yang_avcodec_find_encoder_by_name("h264_nvenc");

            }
			if(g_hwType==YangV_Hw_Android)	m_codec = yang_avcodec_find_encoder_by_name("h264_mediacodec");
		}else if(m_encoderType==Yang_VED_265){
			if(g_hwType==YangV_Hw_Intel)	{
				m_codec = yang_avcodec_find_encoder_by_name("hevc_vaapi");
			}
			if(g_hwType==YangV_Hw_Nvdia) 	m_codec = yang_avcodec_find_encoder_by_name("hevc_nvenc");
			if(g_hwType==YangV_Hw_Android)	m_codec = yang_avcodec_find_encoder_by_name("hevc_mediacodec");
		}

	m_codecCtx = yang_avcodec_alloc_context3(m_codec);
	initParam(m_codecCtx,pvp,penc);
	m_codecCtx->get_format = get_hw_format;	// AV_PIX_FMT_NV12;//get_hw_format;
	if(g_hwType==YangV_Hw_Intel){

		ret = yang_av_hwdevice_ctx_create(&hw_device_ctx, AV_HWDEVICE_TYPE_VAAPI,"/dev/dri/renderD128", NULL, 0);
		m_codecCtx->pix_fmt = AV_PIX_FMT_VAAPI;
	}else if(g_hwType==YangV_Hw_Nvdia){
		ret = yang_av_hwdevice_ctx_create(&hw_device_ctx, AV_HWDEVICE_TYPE_CUDA,"CUDA", NULL, 0);
		m_codecCtx->pix_fmt = AV_PIX_FMT_CUDA;
	}else if(g_hwType==YangV_Hw_Android){
		ret = yang_av_hwdevice_ctx_create(&hw_device_ctx, AV_HWDEVICE_TYPE_MEDIACODEC,"MEDIACODEC", NULL, 0);
		m_codecCtx->pix_fmt = AV_PIX_FMT_MEDIACODEC;
	}
	//AVPixelFormat format= pvp->videoEncoderFormat==0?AV_PIX_FMT_NV12:AV_PIX_FMT_YUV420P;
	AVPixelFormat format= AV_PIX_FMT_NV12;//AV_PIX_FMT_YUV420P;
	if(pvp->bitDepth==10) format = AV_PIX_FMT_P010;
	if(pvp->bitDepth==16) format = AV_PIX_FMT_P016;

	if ((ret = set_hwframe_ctx(m_codecCtx->pix_fmt,format,&m_videoInfo,m_codecCtx, hw_device_ctx, m_videoInfo.outWidth,
			m_videoInfo.outHeight)) < 0) {
		printf("Failed to set hwframe context.\n");
		//goto close;
	}

	m_codecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

	ret = yang_avcodec_open2(m_codecCtx, m_codec, NULL);
	if (ret < 0){
		printf("\navcodec_open2 failure................\n");
		exit(1);
	}

	m_frame = yang_av_frame_alloc();
	m_frame->width = m_codecCtx->width;
	m_frame->height = m_codecCtx->height;
	m_frame->format =format;
	// int32_t numBytes = avpicture_get_size(AV_PIX_FMT_YUV420P,m_width,m_height);
	int32_t numBytes = yang_av_image_get_buffer_size(format, m_videoInfo.outWidth,m_videoInfo.outHeight, 1);
	buffer = (uint8_t*) yang_av_malloc(numBytes * sizeof(uint8_t));
	yang_av_image_fill_arrays(m_frame->data, m_frame->linesize, buffer,
			format, m_videoInfo.outWidth, m_videoInfo.outHeight, 1);
	yang_av_init_packet(&packet);
	frame_mem_gpu = yang_av_frame_alloc();
	frame_mem_gpu->format = m_codecCtx->pix_fmt;
	frame_mem_gpu->width = m_codecCtx->width;
	frame_mem_gpu->height = m_codecCtx->height;
	ret = yang_av_hwframe_get_buffer(m_codecCtx->hw_frames_ctx, frame_mem_gpu,
			0);
	if (ret < 0)
		printf("\nERROR:av_hwframe_get_buffer failure!\n");

	m_isInit = 1;
	return Yang_Ok;

}
int32_t YangVideoEncoderFfmpeg::encode(YangFrame* pframe, YangEncoderCallback* pcallback) {
	if(!m_codecCtx) return Yang_Ok;
	if(m_sendKeyframe==1) {
			m_sendKeyframe=2;
			m_frame->pict_type=AV_PICTURE_TYPE_I;
		}
	memcpy(m_frame->data[0], pframe->payload, yLen);
	memcpy(m_frame->data[1], pframe->payload + yLen, uLen * 2);
	yang_av_hwframe_transfer_data(frame_mem_gpu, m_frame, 0);
	ret = yang_avcodec_send_frame(m_codecCtx, frame_mem_gpu);
	ret = yang_avcodec_receive_packet(m_codecCtx, &packet);
	int32_t destLen=0;
	if (ret != 0) {
		//destLen = 0;
		return Yang_Ok;
	}
	destLen = packet.size-4;
	int32_t frametype=YANG_Frametype_P;
	memcpy(m_vbuffer, packet.data+4, destLen);
	if(m_encoderType==Yang_VED_264) 	frametype=m_vbuffer[0]==0x67?YANG_Frametype_I:YANG_Frametype_P;
	if(m_encoderType==Yang_VED_265) 	frametype=m_vbuffer[0]==0x40?YANG_Frametype_I:YANG_Frametype_P;
	pframe->payload=m_vbuffer;
	pframe->frametype=frametype;
	pframe->nb=destLen;
	if(pcallback) pcallback->onVideoData(pframe);
	if(m_sendKeyframe==2) {
		m_frame->pict_type=AV_PICTURE_TYPE_NONE;
		m_sendKeyframe=0;
	}
	return 1;
}

void YangVideoEncoderFfmpeg::encode_close() {
//    av_parser_close(pCodecParserCtx);
	if (usingVaapi) {
		yang_av_buffer_unref(&hw_device_ctx);
		yang_av_frame_free(&frame_mem_gpu);
	}
	yang_av_frame_free(&m_frame);
	m_frame = NULL;
	if (m_codecCtx){
		yang_avcodec_close(m_codecCtx);
		yang_av_free(m_codecCtx);
	}
	m_codecCtx = NULL;
	if(buffer) yang_av_free(buffer);
	buffer = NULL;

}

