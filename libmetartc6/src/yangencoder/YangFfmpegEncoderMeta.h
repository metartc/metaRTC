//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGENCODER_SRC_YANGFFMPEGENCODERMETA_H_
#define YANGENCODER_SRC_YANGFFMPEGENCODERMETA_H_
#include <yangutil/sys/YangLoadLib.h>
#include <yangavutil/video/YangVideoEncoderMeta.h>
#include <yangutil/yangavinfotype.h>
#if Yang_Enable_Ffmpeg_Codec
extern "C"{
	#include <libavcodec/avcodec.h>
	#include <libavutil/avutil.h>
	#include <libavutil/imgutils.h>
}

class YangFfmpegEncoderMeta:public YangVideoEncoderMeta {
public:
	YangFfmpegEncoderMeta();
	virtual ~YangFfmpegEncoderMeta();
	void yang_initVmd(YangVideoMeta *p_vmd, YangVideoInfo *p_config, YangVideoEncInfo *penc);
	private:
	void yang_find_next_start_code(YangVideoCodec pve,uint8_t *buf,int32_t bufLen,int32_t *vpsPos,int32_t *vpsLen,int32_t *spsPos,int32_t *spsLen,int32_t *ppsPos,int32_t *ppsLen);
	int32_t set_hwframe_ctx(AVPixelFormat ctxformat,AVPixelFormat swformat,YangVideoInfo *yvp,AVCodecContext *ctx, AVBufferRef *hw_device_ctx,int32_t pwid,int32_t phei);

	void yang_getH2645Config(uint8_t *p_configBuf, int32_t *p_configLen,
				YangVideoInfo *p_config);

		void yang_getSpsPps(YangH2645Conf *p265, YangVideoInfo *config, YangVideoEncInfo *penc);

#if Yang_Enable_FfmpegSo
		YangLoadLib m_lib,m_lib1;
		void loadLib();
		void unloadLib();
	 	AVBufferRef *(*yang_av_hwframe_ctx_alloc)(AVBufferRef *device_ctx);
	    	int32_t (*yang_av_hwframe_ctx_init)(AVBufferRef *ref);
	    	void (*yang_av_buffer_unref)(AVBufferRef **buf);
	    	AVCodec *(*yang_avcodec_find_encoder_by_name)(const char *name);
	    	int32_t (*yang_av_hwdevice_ctx_create)(AVBufferRef **device_ctx, enum AVHWDeviceType type,
	    	                           const char *device, AVDictionary *opts, int32_t flags);
	    	AVFrame *(*yang_av_frame_alloc)(void);
	    	int32_t (*yang_avcodec_open2)(AVCodecContext *avctx, const AVCodec *codec, AVDictionary **options);
	    	int32_t (*yang_av_image_get_buffer_size)(enum AVPixelFormat pix_fmt, int32_t width, int32_t height, int32_t align);
	    	void *(*yang_av_malloc)(size_t size);
	    	int32_t (*yang_av_image_fill_arrays)(uint8_t *dst_data[4], int32_t dst_linesize[4],
	    	                         const uint8_t *src,
	    	                         enum AVPixelFormat pix_fmt, int32_t width, int32_t height, int32_t align);
	    	void (*yang_av_init_packet)(AVPacket *pkt);
	    	int32_t (*yang_av_hwframe_get_buffer)(AVBufferRef *hwframe_ctx, AVFrame *frame, int32_t flags);
	    	int32_t (*yang_av_hwframe_transfer_data)(AVFrame *dst, const AVFrame *src, int32_t flags);
	    	int32_t (*yang_avcodec_send_frame)(AVCodecContext *avctx, const AVFrame *frame);
	    	int32_t (*yang_avcodec_receive_packet)(AVCodecContext *avctx, AVPacket *avpkt);
	    	void (*yang_av_frame_free)(AVFrame **frame);
	    	int32_t (*yang_avcodec_close)(AVCodecContext *avctx);
	    	void (*yang_av_free)(void *ptr);
	    	AVBufferRef *(*yang_av_buffer_ref)(AVBufferRef *buf);
	    	AVCodecContext *(*yang_avcodec_alloc_context3)(const AVCodec *codec);
#else
#define yang_av_hwframe_ctx_alloc av_hwframe_ctx_alloc
#define yang_av_hwframe_ctx_init av_hwframe_ctx_init
#define yang_av_buffer_unref av_buffer_unref
#define yang_avcodec_find_encoder_by_name avcodec_find_encoder_by_name
#define yang_av_hwdevice_ctx_create av_hwdevice_ctx_create
#define yang_av_frame_alloc av_frame_alloc
#define yang_avcodec_open2 avcodec_open2
#define yang_av_image_get_buffer_size av_image_get_buffer_size
#define yang_av_malloc av_malloc
#define yang_av_image_fill_arrays av_image_fill_arrays
#define yang_av_init_packet av_init_packet
#define yang_av_hwframe_get_buffer av_hwframe_get_buffer
#define yang_av_hwframe_transfer_data av_hwframe_transfer_data
#define yang_avcodec_send_frame avcodec_send_frame
#define yang_avcodec_receive_packet avcodec_receive_packet
#define yang_av_frame_free av_frame_free
#define yang_avcodec_close avcodec_close
#define yang_av_free av_free
#define yang_av_buffer_ref av_buffer_ref
#define yang_avcodec_alloc_context3 avcodec_alloc_context3
#endif
};
#endif
#endif /* YANGENCODER_SRC_YANGFFMPEGENCODERMETA_H_ */
