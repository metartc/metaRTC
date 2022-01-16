#include "YangH264EncoderMeta.h"
#include <yangutil/yang_unistd.h>
#include <yangutil/sys/YangLog.h>
#include <yangavutil/video/YangMeta.h>
#include <stdio.h>
#include <malloc.h>
#include "YangH264EncoderSoft.h"
YangH264EncoderMeta::YangH264EncoderMeta() {
	m_lib.loadObject("libx264");
	loadLib();
}
YangH264EncoderMeta::~YangH264EncoderMeta() {
	unloadLib();
	m_lib.unloadObject();
}
void YangH264EncoderMeta::loadLib() {
	yang_x264_param_default = (void (*)(x264_param_t*)) m_lib.loadFunction(
			"x264_param_default");
	yang_x264_param_default_preset = (int32_t (*)(x264_param_t*, const char *preset,
			const char *tune)) m_lib.loadFunction("x264_param_default_preset");
	char s[30];
	memset(s, 0, 30);
	sprintf(s, "x264_encoder_open_%d", X264_BUILD);
	yang_x264_encoder_open = (x264_t* (*)(x264_param_t*)) m_lib.loadFunction(s);
	yang_x264_param_apply_profile =
			(int32_t (*)(x264_param_t*, const char *profile)) m_lib.loadFunction(
					"x264_param_apply_profile");
	yang_x264_picture_alloc = (int32_t (*)(x264_picture_t *pic, int32_t i_csp,
			int32_t i_width, int32_t i_height)) m_lib.loadFunction(
			"x264_picture_alloc");
	yang_x264_encoder_headers = (int32_t (*)(x264_t*, x264_nal_t **pp_nal,
			int32_t *pi_nal)) m_lib.loadFunction("x264_encoder_headers");
	yang_x264_picture_clean =
			(void (*)(x264_picture_t *pic)) m_lib.loadFunction(
					"x264_picture_clean");
	yang_x264_encoder_close = (void (*)(x264_t*)) m_lib.loadFunction(
			"x264_encoder_close");
}
void YangH264EncoderMeta::unloadLib() {
	yang_x264_param_default_preset = NULL;
	yang_x264_encoder_open = NULL;
	yang_x264_param_default = NULL;
	yang_x264_param_apply_profile = NULL;
	yang_x264_picture_alloc = NULL;
	yang_x264_encoder_headers = NULL;
	yang_x264_picture_clean = NULL;
	yang_x264_encoder_close = NULL;
}
#define HEX2BIN(a)      (((a)&0x40)?((a)&0xf)+9:((a)&0xf))

void YangH264EncoderMeta::yang_getSpsPps(YangH2645Conf *p264,
		YangVideoInfo *config, YangVideoEncInfo *penc) {
	x264_t *p264Handle = NULL;
	x264_param_t *param = new x264_param_t();
	if (penc->preset < 5)
		yang_x264_param_default_preset(param, x264_preset_names[penc->preset],
				"zerolatency");
	else
		yang_x264_param_default(param);

	YangH264EncoderSoft::initX264Param(config, penc, param);
	yang_x264_param_apply_profile(param, x264_profile_names[0]);
	if ((p264Handle = yang_x264_encoder_open(param)) == NULL) {
		printf("x264_encoder_open failed/n");
#ifdef _MSC_VER
         ExitProcess(1);
#else
        _exit(1);
  #endif

	}
	int32_t iNal = 0;
	x264_nal_t *p264Nal = NULL;

	yang_x264_encoder_headers(p264Handle, &p264Nal, &iNal);


	for (int32_t i = 0; i < iNal; ++i) {
		switch (p264Nal[i].i_type) {
		case NAL_SPS:

			p264->spsLen = p264Nal[i].i_payload - 4;
			//spsBuf
			memcpy(p264->sps, p264Nal[i].p_payload + 4, p264->spsLen);
			// put_be32((char*)spsBuf,sps_len);

			break;
		case NAL_PPS:
			p264->ppsLen = p264Nal[i].i_payload - 4;

			memcpy(p264->pps, p264Nal[i].p_payload + 4, p264->ppsLen);
			// put_be32((char*)ppsBuf,pps_len);

			break;
		default:
			break;
		}
	}

	yang_x264_encoder_close(p264Handle);
	p264Handle = NULL;
	delete param;
	param = NULL;
	p264Nal = NULL;

}

void YangH264EncoderMeta::yang_initVmd(YangVideoMeta *p_vmd,
		YangVideoInfo *p_config, YangVideoEncInfo *penc) {
	if (!p_vmd->isInit) {
		yang_getSpsPps(&p_vmd->mp4Meta, p_config, penc);
		yang_getConfig_Flv_H264(&p_vmd->mp4Meta, p_vmd->livingMeta.buffer,
				&p_vmd->livingMeta.bufLen);

		p_vmd->isInit = 1;
	}
}

