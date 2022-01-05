/*
 * YangH265EncoderMeta.h
 *
 *  Created on: 2020年10月14日
 *      Author: yang
 */

#ifndef YANGENCODER_SRC_YANGH265ENCODERMETA_H_
#define YANGENCODER_SRC_YANGH265ENCODERMETA_H_
#include "yangutil/sys/YangLoadLib.h"
#include <stdint.h>
#include <yangutil/yangavinfotype.h>

#include "x265.h"
#include <yangavutil/video/YangVideoEncoderMeta.h>

class YangH265EncoderMeta:public YangVideoEncoderMeta {
public:
	YangH265EncoderMeta();
	virtual ~YangH265EncoderMeta();
	void yang_initVmd(YangVideoMeta *p_vmd, YangVideoInfo *p_config, YangVideoEncInfo *penc);
private:
	void yang_getH265Config(uint8_t *p_configBuf, int32_t *p_configLen,
			YangVideoInfo *p_config);
	//void yang_getH265Config_Flv(YangH2645Conf *p_265, uint8_t *configBuf,	int32_t *p_configLen);
	//void yang_getH265Config_1(YangVideoParam *p_config, YangH2645Conf *p265);
	void yang_getSpsPps(YangH2645Conf *p265, YangVideoInfo *config, YangVideoEncInfo *penc);
	//void initParam(x265_param *param,YangVideoParam *yvp, int32_t p_rc_method);
	YangLoadLib m_lib;
	void loadLib();
	void unloadLib();
	x265_param *(*yang_x265_param_alloc)(void);
	void (*yang_x265_param_free)(x265_param *);
	void (*yang_x265_param_default)(x265_param*);
	int32_t (*yang_x265_param_default_preset)(x265_param *, const char *preset, const char *tune);
	int32_t (*yang_x265_param_apply_profile)(x265_param*, const char *profile);
	//int32_t (*yang_x265_picture_alloc)(x265_picture_t *pic, int32_t i_csp, int32_t i_width,	int32_t i_height);
	int32_t (*yang_x265_encoder_headers)(x265_encoder*, x265_nal **pp_nal, int32_t *pi_nal);
	//void (*yang_x265_picture_clean)(x265_picture *pic);
	void (*yang_x265_encoder_close)(x265_encoder*);
	x265_encoder* (*yang_x265_encoder_open)(x265_param*);
};

#endif /* YANGENCODER_SRC_YANGH265ENCODERMETA_H_ */
