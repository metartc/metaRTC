#ifndef __YangH264VmdHandle__
#define __YangH264VmdHandle__
#include "yangutil/sys/YangLoadLib.h"
#include <stdint.h>
#include <yangavutil/video/YangVideoEncoderMeta.h>
#include <yangutil/yangavinfotype.h>

#include "x264.h"
class YangH264EncoderMeta:public YangVideoEncoderMeta{
public:
	YangH264EncoderMeta();
	~YangH264EncoderMeta();
	void yang_initVmd(YangVideoMeta *p_vmd, YangVideoInfo *p_config, YangVideoEncInfo *penc);
private:
	void yang_getH264Config(uint8_t *p_configBuf, int32_t *p_configLen,
			YangVideoInfo *p_config);
	//void yang_getH264Config_Flv(YangH2645Conf *p_264, uint8_t *configBuf,	int32_t *p_configLen);
	//void yang_getH264Config_1(YangVideoParam *p_config, YangH2645Conf *p264);
	void yang_getSpsPps(YangH2645Conf *p264, YangVideoInfo *config, YangVideoEncInfo *penc);

	YangLoadLib m_lib;
	void loadLib();
	void unloadLib();
	void (*yang_x264_param_default)(x264_param_t*);
	int32_t    (* yang_x264_param_default_preset)( x264_param_t *, const char *preset, const char *tune );
	int32_t (*yang_x264_param_apply_profile)(x264_param_t*, const char *profile);
	int32_t (*yang_x264_picture_alloc)(x264_picture_t *pic, int32_t i_csp, int32_t i_width,	int32_t i_height);
	int32_t (*yang_x264_encoder_headers)(x264_t*, x264_nal_t **pp_nal, int32_t *pi_nal);
	void (*yang_x264_picture_clean)(x264_picture_t *pic);
	void (*yang_x264_encoder_close)(x264_t*);
	x264_t* (*yang_x264_encoder_open)(x264_param_t*);
};

#endif
