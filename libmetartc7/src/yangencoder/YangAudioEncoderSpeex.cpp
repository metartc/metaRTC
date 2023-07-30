//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangencoder/YangAudioEncoderSpeex.h>

void YangAudioEncoderSpeex::loadLib() {
	yang_speex_lib_get_mode =
			(const SpeexMode* (*)(int32_t mode)) m_lib.loadFunction(
					"speex_lib_get_mode");
	yang_speex_encoder_ctl =
			(int32_t (*)(void *state, int32_t request, void *ptr)) m_lib.loadFunction(
					"speex_encoder_ctl");
	yang_speex_encoder_init =
			(void* (*)(const SpeexMode *mode)) m_lib.loadFunction(
					"speex_encoder_init");
	yang_speex_bits_init = (void (*)(SpeexBits *bits)) m_lib.loadFunction(
			"speex_bits_init");
	yang_speex_bits_reset = (void (*)(SpeexBits *bits)) m_lib.loadFunction(
			"speex_bits_reset");
	yang_speex_bits_write =
			(int32_t (*)(SpeexBits *bits, char *bytes, int32_t max_len)) m_lib.loadFunction(
					"speex_bits_write");
	yang_speex_encode =
			(int32_t (*)(void *state, float *in, SpeexBits *bits)) m_lib.loadFunction(
					"speex_encode");
	yang_speex_encoder_destroy = (void (*)(void *state)) m_lib.loadFunction(
			"speex_encoder_destroy");
	yang_speex_bits_destroy = (void (*)(SpeexBits *bits)) m_lib.loadFunction(
			"speex_bits_destroy");
}

void YangAudioEncoderSpeex::unloadLib() {
	yang_speex_lib_get_mode = NULL;
	yang_speex_encoder_init = NULL;
	yang_speex_bits_init = NULL;
	yang_speex_bits_reset = NULL;
	yang_speex_bits_write = NULL;
	yang_speex_encode = NULL;
	yang_speex_encoder_destroy = NULL;
	yang_speex_bits_destroy = NULL;
}
YangAudioEncoderSpeex::YangAudioEncoderSpeex() {

	isConvert = 0;
	m_quality = 8;
	m_state = NULL;
	ret = 0;
	m_cbits = new char[600];

	m_bufLen = 0;
	skip_group_delay = 0;
	//m_mode = speex_wb_mode;
	temp = NULL; //new char[8096];

	m_input = NULL;
	m_input1 = NULL;
	m_in = NULL;
	unloadLib();

}
YangAudioEncoderSpeex::~YangAudioEncoderSpeex(void) {
	closeSpeex();
	yang_deleteA(m_cbits);
	yang_deleteA(temp);

	yang_deleteA(m_input1);
	yang_deleteA(m_input);
	yang_deleteA(m_in);

	unloadLib();
	m_lib.unloadObject();
}

void YangAudioEncoderSpeex::initSpeexPara() {
	//if (m_mode == speex_nb_mode) {
	//	m_quality = 6;
	//m_frameSize = 160;
	//}
	//if (m_mode == speex_wb_mode) {
	m_quality = 10;
	//	m_frameSize = 320;
//	}
	//if (m_mode == speex_uwb_mode) {
	//	m_quality = 10;
	//	m_frameSize = 640;
	//}
}

void YangAudioEncoderSpeex::init(YangAudioInfo *pap) {
	if (m_isInit)
		return;

	m_lib.loadObject("libspeex");
	loadLib();
	setAudioPara(pap);
	m_state = yang_speex_encoder_init(yang_speex_lib_get_mode(SPEEX_MODEID_WB));//speex_wb_mode &speex_nb_mode);
	//speex_lib_get_mode(SPEEX_MODEID_WB);
	//const SpeexMode * speex_lib_get_mode (int32_t mode);
	//initSpeexPara();
	//m_quality = 8;
	m_quality = 10;
	//m_frameSize = 320;
	//m_channel=1;
	//int32_t samplerate = m_context->sample;
	yang_speex_encoder_ctl(m_state, SPEEX_SET_QUALITY, &m_quality);
	//speex_encoder_ctl(m_state, SPEEX_SET_SAMPLING_RATE, &samplerate);
	//int32_t tmp = 0;
	//speex_encoder_ctl(m_state, SPEEX_SET_VBR, &tmp);
	//tmp = 10;
	//speex_encoder_ctl(m_state, SPEEX_SET_QUALITY, &tmp);
	//tmp = 1;
//	speex_encoder_ctl(m_state, SPEEX_SET_COMPLEXITY, &tmp);

	//speex_encoder_ctl(m_state, SPEEX_GET_BITRATE, &tmp);
	//int32_t sam=0;
	//speex_encoder_ctl(m_state, SPEEX_GET_SAMPLING_RATE, &sam);

//	m_resample->init(2,m_context->sample,16000);
	//speex_encoder_ctl(m_state, SPEEX_GET_LOOKAHEAD, &skip_group_delay);

	// int32_t frameSize=0;
//	speex_encoder_ctl(m_state, SPEEX_GET_FRAME_SIZE, &m_frameSize);

	if (!m_audioInfo.enableMono) {
		m_in = new short[882 * 2];
		temp = new char[1024 * 8];
		m_input = new float[m_audioInfo.frameSize * m_audioInfo.channel];
		m_input1 = new short[m_audioInfo.frameSize * m_audioInfo.channel];
	} else {
		m_input1 = new short[320];
		m_input = new float[320];
	}

	//SPEEX_SET_BITRATE
	yang_speex_bits_init(&m_bits);

	m_isInit = 1;


}


int32_t YangAudioEncoderSpeex::encoder(YangFrame* pframe,YangEncoderCallback *pcallback) {

	if (!m_state)
		return 1;
	if (m_audioInfo.enableMono) {
		memcpy((char*) m_input1, pframe->payload, 640);
		for (int32_t i = 0; i < 320; i++) {
			m_input[i] = m_input1[i];
		}
		yang_speex_bits_reset(&m_bits);

		yang_speex_encode(m_state, m_input, &m_bits);
		ret = yang_speex_bits_write(&m_bits, m_cbits, 300);

		if (ret > 0 && pcallback) {
			pframe->payload=(uint8_t*) m_cbits;
			pframe->nb=ret;
			pcallback->onAudioData(pframe);
			return Yang_Ok;
		}
		return 1;

	}
	return 1;

}

void YangAudioEncoderSpeex::closeSpeex() {
	if (m_state)
		yang_speex_encoder_destroy(m_state);
	m_state = NULL;
	yang_speex_bits_destroy(&m_bits);

}
