//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGENCODER_SRC_YANGH265ENCODERMETA_H_
#define YANGENCODER_SRC_YANGH265ENCODERMETA_H_
#include <yangavutil/video/YangVideoEncoderMeta.h>
#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangLoadLib.h>
#include <x265.h>

class YangH265EncoderMeta:public YangVideoEncoderMeta {
public:
	YangH265EncoderMeta();
	virtual ~YangH265EncoderMeta();
	void yang_initVmd(YangVideoMeta *p_vmd, YangVideoInfo *p_config, YangVideoEncInfo *penc);
private:
	void yang_getH265Config(uint8_t *p_configBuf, int32_t *p_configLen,
			YangVideoInfo *p_config);
	void yang_getSpsPps(YangH2645Conf *p265, YangVideoInfo *config, YangVideoEncInfo *penc);

	YangLoadLib m_lib;
	void loadLib();
	void unloadLib();
	x265_param *(*yang_x265_param_alloc)(void);
	void (*yang_x265_param_free)(x265_param *);
	void (*yang_x265_param_default)(x265_param*);
	int32_t (*yang_x265_param_default_preset)(x265_param *, const char *preset, const char *tune);
	int32_t (*yang_x265_param_apply_profile)(x265_param*, const char *profile);
	int32_t (*yang_x265_encoder_headers)(x265_encoder*, x265_nal **pp_nal, int32_t *pi_nal);

	void (*yang_x265_encoder_close)(x265_encoder*);
	x265_encoder* (*yang_x265_encoder_open)(x265_param*);
};

#endif /* YANGENCODER_SRC_YANGH265ENCODERMETA_H_ */
