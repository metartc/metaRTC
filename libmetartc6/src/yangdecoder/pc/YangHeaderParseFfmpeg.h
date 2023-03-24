//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGDECODER_SRC_YANGHEADERPARSEFFMPEG_H_
#define YANGDECODER_SRC_YANGHEADERPARSEFFMPEG_H_
#include <yangutil/sys/YangLoadLib.h>
#include <yangutil/yangavinfotype.h>

extern "C"{
	#include <libavcodec/avcodec.h>
	#include <libavutil/avutil.h>
	#include <libavutil/imgutils.h>
}

class YangHeaderParseFfmpeg {
public:
	YangHeaderParseFfmpeg();
	virtual ~YangHeaderParseFfmpeg();
	void parse(uint8_t* pheader,int32_t pheaderLen);
	void init();
private:
	YangLoadLib m_lib,m_lib1;
	void loadLib();
	void unloadLib();

	AVBufferRef *(*yang_av_hwframe_ctx_alloc)(AVBufferRef *device_ctx);
	int32_t (*yang_av_hwframe_ctx_init)(AVBufferRef *ref);
	AVBufferRef *(*yang_av_buffer_ref)(AVBufferRef *buf);
	void *(*yang_av_malloc)(size_t size);
	int32_t (*yang_av_hwdevice_ctx_create)(AVBufferRef **device_ctx, enum AVHWDeviceType type,
			const char *device, AVDictionary *opts, int32_t flags);
	int32_t (*yang_avcodec_open2)(AVCodecContext *avctx, const AVCodec *codec, AVDictionary **options);
	AVFrame *(*yang_av_frame_alloc)(void);
	int32_t (*yang_av_image_get_buffer_size)(enum AVPixelFormat pix_fmt, int32_t width, int32_t height, int32_t align);
	int32_t (*yang_av_image_fill_arrays)(uint8_t *dst_data[4], int32_t dst_linesize[4],
			const uint8_t *src,
			enum AVPixelFormat pix_fmt, int32_t width, int32_t height, int32_t align);
	void (*yang_av_buffer_unref)(AVBufferRef **buf);
	void (*yang_av_init_packet)(AVPacket *pkt);
	int32_t (*yang_av_hwframe_get_buffer)(AVBufferRef *hwframe_ctx, AVFrame *frame, int32_t flags);
	AVCodec *(*yang_avcodec_find_decoder)(enum AVCodecID id);
	AVCodecContext *(*yang_avcodec_alloc_context3)(const AVCodec *codec);
	int32_t (*yang_avcodec_send_packet)(AVCodecContext *avctx, const AVPacket *avpkt);
	int32_t (*yang_avcodec_receive_frame)(AVCodecContext *avctx, AVFrame *frame);
	int32_t (*yang_av_hwframe_transfer_data)(AVFrame *dst, const AVFrame *src, int32_t flags);
	void (*yang_av_frame_free)(AVFrame **frame);
	void (*yang_av_free)(void *ptr);
	int32_t (*yang_avcodec_close)(AVCodecContext *avctx);
};

#endif /* YANGDECODER_SRC_YANGHEADERPARSEFFMPEG_H_ */
