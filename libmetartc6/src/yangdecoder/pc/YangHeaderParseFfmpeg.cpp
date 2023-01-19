//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangdecoder/pc/YangHeaderParseFfmpeg.h>
#include <yangutil/sys/YangLog.h>
void YangHeaderParseFfmpeg::loadLib() {

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
	yang_avcodec_alloc_context3 =
			(AVCodecContext* (*)(const AVCodec *codec)) m_lib.loadFunction(
					"avcodec_alloc_context3");
	yang_avcodec_send_packet = (int32_t (*)(AVCodecContext *avctx,
			const AVPacket *avpkt)) m_lib.loadFunction("avcodec_send_packet");
	yang_avcodec_receive_frame =
			(int32_t (*)(AVCodecContext *avctx, AVFrame *frame)) m_lib.loadFunction(
					"avcodec_receive_frame");
	yang_avcodec_close = (int32_t (*)(AVCodecContext *avctx)) m_lib.loadFunction(
			"avcodec_close");
}

void YangHeaderParseFfmpeg::unloadLib() {
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
	yang_av_frame_free = NULL;
	yang_avcodec_close = NULL;
}
YangHeaderParseFfmpeg::YangHeaderParseFfmpeg() {
	unloadLib();

}

YangHeaderParseFfmpeg::~YangHeaderParseFfmpeg() {
	unloadLib();
	m_lib.unloadObject();
	m_lib1.unloadObject();
}
void YangHeaderParseFfmpeg::init(){
	m_lib.loadObject("libavcodec");
		m_lib1.loadObject("libavutil");
		loadLib();
}
void YangHeaderParseFfmpeg::parse(uint8_t* headers,int32_t headerLen){
	AVCodec *t_codec = yang_avcodec_find_decoder(AV_CODEC_ID_H265);
	AVCodecContext*	t_codecCtx = yang_avcodec_alloc_context3(t_codec);
	t_codecCtx->extradata = (uint8_t*) yang_av_malloc(headerLen + AV_INPUT_BUFFER_PADDING_SIZE);
	t_codecCtx->extradata_size = headerLen;
	memcpy(t_codecCtx->extradata, headers, headerLen);
	int32_t ret = yang_avcodec_open2(t_codecCtx, t_codec, NULL);
		if (ret < 0)
			yang_error("avcodec_open2 failure");


		yang_avcodec_close(t_codecCtx);
		yang_av_free(t_codecCtx);
		t_codecCtx=NULL;

}
