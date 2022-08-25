//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef __YangH264EncoderSoft__
#define __YangH264EncoderSoft__
#include <yangencoder/YangVideoEncoder.h>
#include <yangutil/buffer/YangVideoBuffer.h>
#include <yangutil/buffer/YangVideoEncoderBuffer.h>
#include <yangutil/sys/YangIni.h>
#include <yangutil/sys/YangLoadLib.h>
#include <x264.h>

#ifdef __ANDROID__
#define Yang_X264_So 0
#else
#define Yang_X264_So 1
#endif

class YangH264EncoderSoft: public YangVideoEncoder {
public:
	YangH264EncoderSoft();
	~YangH264EncoderSoft(void);
	static void initX264Param(YangVideoInfo *pvp,YangVideoEncInfo *penc,x264_param_t *param);
	int32_t init(YangContext* pcontext,YangVideoInfo* pvideoInfo);

	void setVideoMetaData(YangVideoMeta *pvmd);
	void parseRtmpHeader(uint8_t *p, int32_t pLen, int32_t *pwid, int32_t *phei, int32_t *pfps);
	void sendMsgToEncoder(YangRtcEncoderMessage *msg);
private:

	int32_t m_sendKeyframe;
	int32_t encode(YangFrame* pframe, YangEncoderCallback* pcallback);
protected:
	int32_t hex2bin(char *str, char **hex);

	uint8_t *m_nal;
	int32_t nal_len, destLength;
	x264_nal_t *m_264Nal;
	int32_t m_i264Nal;
	x264_picture_t *m_264Pic;
	x264_t *m_264Handle;
	x264_picture_t pic_out;
	YangLoadLib m_lib;

	void saveFile(char *fileName, uint8_t *pBuffer, int32_t BufferLen);

private:

	bool m_hasHeader;
#if Yang_X264_So
	void loadLib();
	void unloadLib();
	void    (*yang_x264_param_default)( x264_param_t * );
	int32_t     (*yang_x264_param_default_preset)( x264_param_t *, const char *preset, const char *tune );
	int32_t     (*yang_x264_param_apply_profile)( x264_param_t *, const char *profile );
	int32_t (*yang_x264_picture_alloc)( x264_picture_t *pic, int32_t i_csp, int32_t i_width, int32_t i_height );
	int32_t     (*yang_x264_encoder_encode)( x264_t *, x264_nal_t **pp_nal, int32_t *pi_nal, x264_picture_t *pic_in, x264_picture_t *pic_out );
	void (*yang_x264_picture_clean)( x264_picture_t *pic );
	void    (*yang_x264_encoder_close)( x264_t * );
	x264_t* (*yang_x264_encoder_open)( x264_param_t * );
#else
	#define yang_x264_param_default x264_param_default
	#define yang_x264_param_default_preset x264_param_default_preset
	#define yang_x264_param_apply_profile x264_param_apply_profile
	#define yang_x264_picture_alloc x264_picture_alloc
	#define yang_x264_encoder_encode x264_encoder_encode
	#define yang_x264_picture_clean x264_picture_clean
	#define yang_x264_encoder_close x264_encoder_close
	#define yang_x264_encoder_open x264_encoder_open
#endif
};
#endif
