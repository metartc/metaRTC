//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGDECODER_SRC_YANGH2645VIDEODECODERFFMPEG_H_
#define YANGDECODER_SRC_YANGH2645VIDEODECODERFFMPEG_H_
#include <yangdecoder/YangAudioDecoderHandles.h>
#include <yangdecoder/YangVideoDecoder.h>
#include <yangutil/sys/YangLoadLib.h>
#include <yangutil/yangavinfotype.h>

#if Yang_Enable_Ffmpeg_Codec
extern "C" {
	#include <libavcodec/avcodec.h>
	#include <libavutil/avutil.h>
	#include <libavutil/imgutils.h>
}


class YangVideoDecoderFfmpeg: public YangVideoDecoder {
public:
	YangVideoDecoderFfmpeg(YangVideoInfo *pcontext,
			YangVideoCodec pencdectype);
	virtual ~YangVideoDecoderFfmpeg();
	void parseRtmpHeader(uint8_t *p, int32_t pLen, int32_t *pwid, int32_t *phei, int32_t *pfps);
	void init();
	int32_t decode(YangFrame* videoFrame,YangYuvType yuvtype,YangDecoderCallback* pcallback);

	void decode_close();
	static YangVideoHwType g_hwType;
protected:
	uint8_t* m_buffer;
		int32_t m_bufLen;
private:
	YangVideoCodec m_encDecType;
	YangVideoInfo *m_context;
	int32_t m_bitDepth;
	int32_t ret;
	AVCodec *m_codec;
	AVCodecContext *m_codecCtx = NULL;
	AVPacket packet;
	AVFrame *m_frame;
	int32_t m_width, m_height;
	int32_t yLen;
	int32_t uLen;
	int32_t allLen;
	uint8_t *buffer = NULL;
	int32_t usingHw;
	AVBufferRef *hw_device_ctx;
	AVFrame *frame_mem_gpu;

	int32_t decode_1(YangFrame* videoFrame,YangYuvType yuvtype,YangDecoderCallback* pcallback);
	int32_t decode_2(YangFrame* videoFrame,YangYuvType yuvtype,YangDecoderCallback* pcallback);
	int32_t set_hwframe_ctx(AVPixelFormat ctxformat, AVPixelFormat swformat,
			YangVideoInfo *yvp, AVCodecContext *ctx,
			AVBufferRef *hw_device_ctx, int32_t pwid, int32_t phei);

	void parseHeaderH265(uint8_t *p, int32_t pLen, int32_t *pwid, int32_t *phei, int32_t *pfps);
	void parseHeaderH264(uint8_t *p, int32_t pLen, int32_t *pwid, int32_t *phei, int32_t *pfps);
#if Yang_Enable_FfmpegSo
	YangLoadLib m_lib, m_lib1;
	void loadLib();
	void unloadLib();
	AVBufferRef* (*yang_av_hwframe_ctx_alloc)(AVBufferRef *device_ctx);
	int32_t (*yang_av_hwframe_ctx_init)(AVBufferRef *ref);
	AVBufferRef* (*yang_av_buffer_ref)(AVBufferRef *buf);
	void* (*yang_av_malloc)(size_t size);
	int32_t (*yang_av_hwdevice_ctx_create)(AVBufferRef **device_ctx,
			enum AVHWDeviceType type, const char *device, AVDictionary *opts,
			int32_t flags);
	int32_t (*yang_avcodec_open2)(AVCodecContext *avctx, const AVCodec *codec,
			AVDictionary **options);
	AVFrame* (*yang_av_frame_alloc)(void);
	int32_t (*yang_av_image_get_buffer_size)(enum AVPixelFormat pix_fmt, int32_t width,
			int32_t height, int32_t align);
	int32_t (*yang_av_image_fill_arrays)(uint8_t *dst_data[4], int32_t dst_linesize[4],
			const uint8_t *src, enum AVPixelFormat pix_fmt, int32_t width,
			int32_t height, int32_t align);
	void (*yang_av_buffer_unref)(AVBufferRef **buf);
	void (*yang_av_init_packet)(AVPacket *pkt);
	int32_t (*yang_av_hwframe_get_buffer)(AVBufferRef *hwframe_ctx, AVFrame *frame,
			int32_t flags);
	AVCodec* (*yang_avcodec_find_decoder)(enum AVCodecID id);
	AVCodec* (*yang_avcodec_find_decoder_by_name)(const char *name);
	AVCodecContext* (*yang_avcodec_alloc_context3)(const AVCodec *codec);
	int32_t (*yang_avcodec_send_packet)(AVCodecContext *avctx,
			const AVPacket *avpkt);
	int32_t (*yang_avcodec_receive_frame)(AVCodecContext *avctx, AVFrame *frame);
	int32_t (*yang_av_hwframe_transfer_data)(AVFrame *dst, const AVFrame *src,
			int32_t flags);
    void (*yang_avcodec_flush_buffers)(AVCodecContext *avctx);
	void (*yang_av_frame_free)(AVFrame **frame);
	void (*yang_av_free)(void *ptr);
	int32_t (*yang_avcodec_close)(AVCodecContext *avctx);
#else
		#define	yang_avcodec_find_decoder_by_name avcodec_find_decoder_by_name
		#define	yang_av_free  av_free
		#define	yang_av_hwframe_ctx_alloc av_hwframe_ctx_alloc
		#define	yang_av_hwframe_ctx_init  av_hwframe_ctx_init
		#define	yang_av_buffer_ref av_buffer_ref
		#define	yang_av_malloc av_malloc
		#define	yang_av_hwdevice_ctx_create av_hwdevice_ctx_create
		#define	yang_avcodec_open2 avcodec_open2
		#define	yang_av_frame_alloc av_frame_alloc
		#define	yang_av_image_get_buffer_size av_image_get_buffer_size
		#define	yang_av_image_fill_arrays av_image_fill_arrays
		#define	yang_av_init_packet av_init_packet
		#define	yang_av_hwframe_get_buffer av_hwframe_get_buffer
		#define	yang_avcodec_find_decoder avcodec_find_decoder
		#define	yang_avcodec_alloc_context3 avcodec_alloc_context3
		#define	yang_avcodec_send_packet  avcodec_send_packet
		#define	yang_avcodec_receive_frame avcodec_receive_frame
		#define	yang_av_hwframe_transfer_data av_hwframe_transfer_data
        #define yang_avcodec_flush_buffers   avcodec_flush_buffers
		#define	yang_av_frame_free av_frame_free
		#define	yang_avcodec_close avcodec_close
	    #define yang_av_buffer_unref  av_buffer_unref
#endif

};
#endif
#endif /* YANGDECODER_SRC_YANGH2645VIDEODECODERFFMPEG_H_ */
