//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangdecoder/pc/YangVideoDecoderFfmpeg.h>
#include <yangavutil/video/YangMeta.h>
#include <yangavutil/video/YangSpspps.h>
#include <yangutil/sys/YangLog.h>

#if Yang_Enable_Ffmpeg_Codec
#if Yang_Enable_FfmpegSo
void YangVideoDecoderFfmpeg::loadLib() {

	yang_av_frame_alloc = (AVFrame* (*)(void)) m_lib1.loadFunction(
			"av_frame_alloc");
	yang_av_buffer_unref = (void (*)(AVBufferRef **buf)) m_lib1.loadFunction(
			"av_buffer_unref");
	yang_av_hwframe_ctx_init = (int32_t (*)(AVBufferRef *ref)) m_lib1.loadFunction(
			"av_hwframe_ctx_init");
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
	yang_av_malloc = (void* (*)(size_t size)) m_lib1.loadFunction("av_malloc");

	yang_avcodec_open2 = (int32_t (*)(AVCodecContext *avctx, const AVCodec *codec,
			AVDictionary **options)) m_lib.loadFunction("avcodec_open2");
	yang_av_init_packet = (void (*)(AVPacket *pkt)) m_lib.loadFunction(
			"av_init_packet");
	yang_av_hwframe_get_buffer = (int32_t (*)(AVBufferRef *hwframe_ctx,
			AVFrame *frame, int32_t flags)) m_lib.loadFunction(
					"av_hwframe_get_buffer");
	yang_avcodec_find_decoder =
			(AVCodec* (*)(enum AVCodecID id)) m_lib.loadFunction(
					"avcodec_find_decoder");
	yang_avcodec_find_decoder_by_name=(AVCodec *(*)(const char *name)) m_lib.loadFunction(
			"avcodec_find_decoder_by_name");
	yang_avcodec_alloc_context3 =
			(AVCodecContext* (*)(const AVCodec *codec)) m_lib.loadFunction(
					"avcodec_alloc_context3");
	yang_avcodec_send_packet = (int32_t (*)(AVCodecContext *avctx,
			const AVPacket *avpkt)) m_lib.loadFunction("avcodec_send_packet");
	yang_avcodec_receive_frame =
			(int32_t (*)(AVCodecContext *avctx, AVFrame *frame)) m_lib.loadFunction(
					"avcodec_receive_frame");
	yang_avcodec_flush_buffers=(void (*)(AVCodecContext *avctx)) m_lib.loadFunction(
			"avcodec_flush_buffers");
	yang_avcodec_close = (int32_t (*)(AVCodecContext *avctx)) m_lib.loadFunction(
			"avcodec_close");
}
void YangVideoDecoderFfmpeg::unloadLib() {
	yang_avcodec_find_decoder_by_name=NULL;
	yang_av_free = NULL;
	yang_av_hwframe_ctx_alloc = NULL;
	yang_av_hwframe_ctx_init = NULL;
	yang_av_buffer_ref = NULL;
	yang_av_malloc = NULL;
	yang_av_hwdevice_ctx_create = NULL;
	yang_avcodec_open2 = NULL;
	yang_av_frame_alloc = NULL;
	yang_av_image_get_buffer_size = NULL;
	yang_av_image_fill_arrays = NULL;
	yang_av_init_packet = NULL;
	yang_av_hwframe_get_buffer = NULL;
	yang_avcodec_find_decoder = NULL;
	yang_avcodec_alloc_context3 = NULL;
	yang_avcodec_send_packet = NULL;
	yang_avcodec_receive_frame = NULL;
	yang_av_hwframe_transfer_data = NULL;
	yang_avcodec_flush_buffers=NULL;
	yang_av_frame_free = NULL;
	yang_avcodec_close = NULL;
}
#endif
enum AVPixelFormat get_hw_format1(AVCodecContext *ctx,
		const enum AVPixelFormat *pix_fmts) {
	if(YangVideoDecoderFfmpeg::g_hwType==YangV_Hw_Intel) return AV_PIX_FMT_VAAPI;
	if(YangVideoDecoderFfmpeg::g_hwType==YangV_Hw_Nvdia) return AV_PIX_FMT_CUDA;
	if(YangVideoDecoderFfmpeg::g_hwType==YangV_Hw_Android) return AV_PIX_FMT_MEDIACODEC;
	return AV_PIX_FMT_VAAPI;

}


int32_t YangVideoDecoderFfmpeg::set_hwframe_ctx(AVPixelFormat ctxformat,AVPixelFormat swformat,YangVideoInfo *yvp,AVCodecContext *ctx,
		AVBufferRef *hw_device_ctx, int32_t pwid, int32_t phei) {
	AVBufferRef *hw_frames_ref;
	AVHWFramesContext *frames_ctx = NULL;
	int32_t err = 0;

	if (!(hw_frames_ref = yang_av_hwframe_ctx_alloc(hw_device_ctx))) {
		printf("Failed to create hw frame context.\n");
		return -1;
	}
	frames_ctx = (AVHWFramesContext*) (hw_frames_ref->data);
	frames_ctx->format = ctxformat;
	frames_ctx->sw_format = swformat;

	frames_ctx->width = pwid;
	frames_ctx->height = phei;

	if ((err = yang_av_hwframe_ctx_init(hw_frames_ref)) < 0) {
		printf("Failed to initialize VAAPI frame context.Error code: %d\n",
				ret);
		yang_av_buffer_unref(&hw_frames_ref);
		return err;
	}
	ctx->hw_frames_ctx = yang_av_buffer_ref(hw_frames_ref);
	ctx->hw_device_ctx = yang_av_buffer_ref(hw_device_ctx);

	if (!ctx->hw_frames_ctx)
		err = AVERROR(ENOMEM);

	yang_av_buffer_unref(&hw_frames_ref);
	return err;
}

YangVideoDecoderFfmpeg::YangVideoDecoderFfmpeg(YangVideoInfo *pcontext,YangVideoCodec pencdectype) {
	m_encDecType= pencdectype;
	m_context=pcontext;
	usingHw = 0;	//pcontext->usingHwDec==2?1:0;
	if(pcontext->videoDecHwType>0) usingHw=1;

	m_buffer=NULL;
	m_bufLen=0;

	g_hwType=(YangVideoHwType)pcontext->videoDecHwType;
	m_bitDepth=pcontext->bitDepth;

	m_width = 0;
	m_height = 0;
	m_frame = NULL;
	yLen = 0;
	uLen = 0;
	allLen = 0;
	buffer = NULL;
	ret = 0;
	m_codec = NULL;

	hw_device_ctx = NULL;
	frame_mem_gpu = NULL;
#if Yang_Enable_FfmpegSo
	unloadLib();
#endif

}
YangVideoDecoderFfmpeg::~YangVideoDecoderFfmpeg() {
	m_context=NULL;
	if(m_buffer) delete[] m_buffer;
	m_buffer=NULL;
	decode_close();
#if Yang_Enable_FfmpegSo
	unloadLib();
	m_lib.unloadObject();
	m_lib1.unloadObject();
#endif

}
YangVideoHwType YangVideoDecoderFfmpeg::g_hwType=YangV_Hw_Intel;
void  YangVideoDecoderFfmpeg::parseHeaderH265(uint8_t *p,int32_t pLen,int32_t *pwid,int32_t *phei,int32_t *pfps){

	AVCodec *t_codec = yang_avcodec_find_decoder(AV_CODEC_ID_H265);
	AVCodecContext*	t_codecCtx = yang_avcodec_alloc_context3(t_codec);
	t_codecCtx->extradata = (uint8_t*) yang_av_malloc(pLen + AV_INPUT_BUFFER_PADDING_SIZE);
	t_codecCtx->extradata_size = pLen;
	memcpy(t_codecCtx->extradata, p, pLen);
	int32_t ret = yang_avcodec_open2(t_codecCtx, t_codec, NULL);
	if (ret < 0)
		yang_error("avcodec_open2 failure");

	*pwid=t_codecCtx->width;
	*phei=t_codecCtx->height;
	*pfps=t_codecCtx->time_base.den;///t_codecCtx->time_base.num;

	yang_av_free(t_codecCtx->extradata);
	t_codecCtx->extradata=NULL;
	t_codecCtx->extradata_size = 0;
	yang_avcodec_close(t_codecCtx);
	yang_av_free(t_codecCtx);
	t_codecCtx=NULL;
	t_codec=NULL;
}
void  YangVideoDecoderFfmpeg::parseHeaderH264(uint8_t *p,int32_t pLen,int32_t *pwid,int32_t *phei,int32_t *pfps){

	AVCodec *t_codec = yang_avcodec_find_decoder(AV_CODEC_ID_H264);
	AVCodecContext*	t_codecCtx = yang_avcodec_alloc_context3(t_codec);
	t_codecCtx->extradata = (uint8_t*) yang_av_malloc(pLen + AV_INPUT_BUFFER_PADDING_SIZE);
	t_codecCtx->extradata_size = pLen;
	memset(t_codecCtx->extradata, 0, pLen + AV_INPUT_BUFFER_PADDING_SIZE);
	memcpy(t_codecCtx->extradata, p, pLen);

	int32_t ret = yang_avcodec_open2(t_codecCtx, t_codec, NULL);
	if (ret < 0)
		yang_error("avcodec_open2 failure");

	*pwid=t_codecCtx->width;
	*phei=t_codecCtx->height;
	*pfps=t_codecCtx->time_base.den;

	yang_av_free(t_codecCtx->extradata);
	t_codecCtx->extradata=NULL;
	t_codecCtx->extradata_size = 0;
	yang_avcodec_close(t_codecCtx);
	yang_av_free(t_codecCtx);
	t_codecCtx=NULL;
	t_codec=NULL;
}
void YangVideoDecoderFfmpeg::parseRtmpHeader(uint8_t *p, int32_t pLen, int32_t *pwid,
		int32_t *phei, int32_t *pfps) {

	uint8_t headers[128];
	memset(headers, 0, 128);
	int32_t headerLen = 0;
	if(m_encDecType==Yang_VED_H264){
		yang_getH264RtmpHeader(p, headers, &headerLen);
#if Yang_OS_WIN
		uint8_t* sps=NULL;
		int32_t spsLen=0;
		yang_h264_decode_getH264RtmpHeader(p,&sps,&spsLen);
		yang_h264_decode_spspps(sps,spsLen,pwid,phei,pfps);
#else
		parseHeaderH264(headers,headerLen,pwid,phei,pfps);
#endif
	}else if(m_encDecType==Yang_VED_H265) {
		yang_getH265RtmpHeader(p, headers, &headerLen);
		parseHeaderH265(headers,headerLen,pwid,phei,pfps);

	}
	m_width = *pwid;
	m_height= *phei ;

	if(!m_buffer) m_buffer=new uint8_t[m_width*m_height*3/2];

	int32_t bitLen=(m_bitDepth==8?1:2);

	yLen = m_width * m_height*bitLen;
	uLen = yLen / 4;
	allLen = yLen * 3 / 2;


	m_codecCtx->extradata = (uint8_t*) yang_av_malloc(headerLen + AV_INPUT_BUFFER_PADDING_SIZE);
	m_codecCtx->extradata_size = headerLen;
	memcpy(m_codecCtx->extradata, headers, headerLen);

	m_codecCtx->width = m_width;
	m_codecCtx->height = m_height;

	if (usingHw) {
		AVPixelFormat ctxformat,swformat;
		if(m_context->videoDecHwType==YangV_Hw_Intel) ctxformat = AV_PIX_FMT_VAAPI;
		if(m_context->videoDecHwType==YangV_Hw_Nvdia) ctxformat = AV_PIX_FMT_CUDA;
		if(m_context->videoDecHwType==YangV_Hw_Android) ctxformat = AV_PIX_FMT_MEDIACODEC;
		if(m_context->bitDepth==8) swformat = AV_PIX_FMT_NV12;
		if(m_context->bitDepth==10) swformat = AV_PIX_FMT_P010;
		if(m_context->bitDepth==16) swformat = AV_PIX_FMT_P016;
		if ((ret = set_hwframe_ctx(ctxformat,swformat,m_context,m_codecCtx, hw_device_ctx, m_width, m_height))
				< 0) {
			yang_error("Failed to set hwframe context.\n");
			//goto close;
		}
		m_codecCtx->get_format = get_hw_format1;
	}

	ret = yang_avcodec_open2(m_codecCtx, m_codec, NULL);
	if (ret < 0)
		yang_error("avcodec_open2 failure");
	AVPixelFormat fmt=AV_PIX_FMT_YUV420P;
	if(m_bitDepth==8){
		if(usingHw) fmt=AV_PIX_FMT_NV12;
	}else if(m_bitDepth==10){
		fmt=AV_PIX_FMT_P010;
	}else if(m_bitDepth==16){
		fmt=AV_PIX_FMT_P016;
	}
	m_frame = yang_av_frame_alloc();

	int32_t numBytes = yang_av_image_get_buffer_size(fmt, m_width,m_height, 1);

	buffer = (uint8_t*) yang_av_malloc(numBytes * sizeof(uint8_t));


	if (usingHw) {
		yang_av_image_fill_arrays(m_frame->data, m_frame->linesize, buffer,
				fmt, m_width, m_height, 1);
		yang_av_init_packet(&packet);
		frame_mem_gpu = yang_av_frame_alloc();
		frame_mem_gpu->format = m_codecCtx->pix_fmt;
		frame_mem_gpu->width = m_codecCtx->width;
		frame_mem_gpu->height = m_codecCtx->height;
		ret = yang_av_hwframe_get_buffer(m_codecCtx->hw_frames_ctx,frame_mem_gpu, 0);
		if (ret < 0)
			printf("\nERROR:av_hwframe_get_buffer failure!\n");
	} else {
		yang_av_image_fill_arrays(m_frame->data, m_frame->linesize, buffer,
				fmt, m_width, m_height, 1);
		yang_av_init_packet(&packet);
	}

	m_isInit = 1;
}

void YangVideoDecoderFfmpeg::init() {

#if Yang_Enable_FfmpegSo
	m_lib.loadObject("libavcodec");
	m_lib1.loadObject("libavutil");
	loadLib();
#endif

	if(usingHw){
		if(m_encDecType==Yang_VED_H264){
			if(m_context->videoDecHwType==YangV_Hw_Intel)	m_codec = yang_avcodec_find_decoder_by_name("h264_vaapi");//avcodec_find_encoder(AV_CODEC_ID_H264);
			if(m_context->videoDecHwType==YangV_Hw_Nvdia)	m_codec = yang_avcodec_find_decoder_by_name("h264_nvdec");//h264_cuvid
			if(m_context->videoDecHwType==YangV_Hw_Android)	m_codec = yang_avcodec_find_decoder_by_name("h264_mediacodec");
		}else if(m_encDecType==Yang_VED_H265){
			if(m_context->videoDecHwType==YangV_Hw_Intel)	m_codec = yang_avcodec_find_decoder_by_name("hevc_vaapi");
			if(m_context->videoDecHwType==YangV_Hw_Nvdia) 	m_codec = yang_avcodec_find_decoder_by_name("hevc_nvdec");
			if(m_context->videoDecHwType==YangV_Hw_Android)	m_codec = yang_avcodec_find_decoder_by_name("hevc_mediacodec");
		}
	}else{
		if(m_encDecType==Yang_VED_H264){
			m_codec = yang_avcodec_find_decoder(AV_CODEC_ID_H264);
		}else if(m_encDecType==Yang_VED_H265){
			m_codec = yang_avcodec_find_decoder(AV_CODEC_ID_H265);
		}
	}
	m_codecCtx = yang_avcodec_alloc_context3(m_codec);
}

int32_t YangVideoDecoderFfmpeg::decode(YangFrame* videoFrame,YangYuvType yuvtype,YangDecoderCallback* pcallback){
	if (usingHw)
		return decode_2(videoFrame,yuvtype,pcallback);
	else
		return decode_1(videoFrame,yuvtype,pcallback);

}
int32_t YangVideoDecoderFfmpeg::decode_1(YangFrame* videoFrame,YangYuvType yuvtype,YangDecoderCallback* pcallback) {
	packet.data = videoFrame->payload;
	packet.size = videoFrame->nb;

	ret = yang_avcodec_send_packet(m_codecCtx, &packet);
	if (ret != 0) {
		m_bufLen= 0;
		if(ret==AVERROR(EAGAIN)) {
			yang_error("avcodec_send_packet EAGAIN error:input is not accepted in the current state");
			return ERROR_CODEC_Decode;
		}
		if(ret==AVERROR_EOF){
			yang_error("avcodec_send_packet AVERROR_EOF error: the decoder has been flushed, and no new packets can be sent to it ");
			return ERROR_CODEC_Decode;
		}
		if(ret==AVERROR_INVALIDDATA){
			yang_error("avcodec_send_packet AVERROR_INVALIDDATA");
			yang_trace("\n%d:",videoFrame->nb);
			for(int i=0;i<100;i++) yang_trace("%02x,",videoFrame->payload[i]);
			return ERROR_CODEC_Decode;
		}
		yang_error("avcodec_send_packet err(%d)",ret);
		yang_trace("\n");
		for(int i=0;i<100;i++) yang_trace("%02x,",videoFrame->payload[i]);
		return ERROR_CODEC_Decode;
	}
	//avcodec_receive_frame
	while(ret==0){
		ret = yang_avcodec_receive_frame(m_codecCtx, m_frame);

		if (ret != 0) {
			m_bufLen = 0;
			if(ret==AVERROR_EOF){
				yang_avcodec_flush_buffers(m_codecCtx);
				return -1;
			}
			if(ret==AVERROR(EAGAIN)) {
				// yang_error("avcodec_receive_packet err EAGAIN");
				return -1;
			}

			yang_error("avcodec_receive_packet err(%d)",ret);
			return 1;
		}

		for (int i = 0; i < m_height; i++) {
			memcpy(m_buffer + i * m_width, m_frame->data[0] + i * m_frame->linesize[0], m_width);
		}
		for (int i = 0; i < m_height / 2; i++) {
			memcpy(m_buffer + yLen+i * m_width / 2,m_frame->data[1] + i * m_frame->linesize[1], m_width / 2);
		}
		for (int i = 0; i < m_height / 2; i++) {
			memcpy(m_buffer + yLen+ uLen+ i * m_width / 2, m_frame->data[2] + i * m_frame->linesize[2], m_width / 2);
		}
		m_bufLen= allLen;
		videoFrame->payload=m_buffer;
		videoFrame->nb=m_bufLen;
		if(pcallback) pcallback->onVideoData(videoFrame);
	}
	return Yang_Ok;
}
int32_t YangVideoDecoderFfmpeg::decode_2(YangFrame* videoFrame,YangYuvType yuvtype,YangDecoderCallback* pcallback) {
	packet.data = videoFrame->payload;
	packet.size = videoFrame->nb;
	ret = yang_avcodec_send_packet(m_codecCtx, &packet);
	if (ret != 0) {
		m_bufLen= 0;
		return 2;
	}
	while(ret==0){
		ret = yang_avcodec_receive_frame(m_codecCtx, frame_mem_gpu);

		if (ret != 0) {
			m_bufLen = 0;
			if(ret==AVERROR_EOF){
				yang_avcodec_flush_buffers(m_codecCtx);
				return ret;
			}
			if(ret==AVERROR(EAGAIN)) {
				// yang_error("avcodec_receive_packet err EAGAIN");
				return ret;
			}

			yang_error("avcodec_receive_packet err(%d)",ret);
			return 1;
		}
		yang_av_hwframe_transfer_data(m_frame, frame_mem_gpu, 0);

		for (int i = 0; i < m_height; i++) {
			memcpy(m_buffer + i * m_width, m_frame->data[0] + i * m_frame->linesize[0], m_width);
		}
		for (int i = 0; i < m_height / 2; i++) {
			memcpy(m_buffer + yLen+i * m_width / 2,m_frame->data[1] + i * m_frame->linesize[1], m_width / 2);
		}
		for (int i = 0; i < m_height / 2; i++) {
			memcpy(m_buffer + yLen+ uLen+ i * m_width / 2, m_frame->data[2] + i * m_frame->linesize[2], m_width / 2);
		}
		m_bufLen = allLen;
		videoFrame->payload=m_buffer;
		videoFrame->nb=m_bufLen;
		if(pcallback) pcallback->onVideoData(videoFrame);
	}
	return Yang_Ok;
}
void YangVideoDecoderFfmpeg::decode_close() {
	if (usingHw) {
		yang_av_buffer_unref(&hw_device_ctx);
		yang_av_frame_free(&frame_mem_gpu);
	}

	yang_av_frame_free(&m_frame);
	m_frame = NULL;
	if (m_codecCtx) {
		yang_avcodec_close(m_codecCtx);
		yang_av_free(m_codecCtx);
	}

	m_codecCtx = NULL;
	if (buffer)
		yang_av_free(buffer);
	buffer = NULL;

}
#endif

