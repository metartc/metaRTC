/*
 * YangAudioEncoderOpus.cpp
 *
 *  Created on: 2019年9月29日
 *      Author: yang
 */
#include "YangAudioEncoderOpus.h"

#include "yangutil/yang_unistd.h"
#include "stdio.h"
#include "string.h"


void YangAudioEncoderOpus::loadLib() {
	yang_opus_encoder_create = (OpusEncoder* (*)(opus_int32 Fs, int32_t channels,
			int32_t application, int32_t *error)) m_lib.loadFunction(
			"opus_encoder_create");
	yang_opus_encoder_init = (int32_t (*)(OpusEncoder *st, opus_int32 Fs,
			int32_t channels, int32_t application)) m_lib.loadFunction(
			"opus_encoder_init");
	yang_opus_encode =
			(opus_int32 (*)(OpusEncoder *st, const opus_int16 *pcm,
					int32_t frame_size, uint8_t *data,
					opus_int32 max_data_bytes)) m_lib.loadFunction(
					"opus_encode");
	yang_opus_encoder_ctl =
			(int32_t (*)(OpusEncoder *st, int32_t request, ...)) m_lib.loadFunction(
					"opus_encoder_ctl");
	yang_opus_encoder_destroy = (void (*)(OpusEncoder *st)) m_lib.loadFunction(
			"opus_encoder_destroy");
	yang_opus_strerror = (const char* (*)(int32_t error)) m_lib.loadFunction(
			"opus_strerror");
}

void YangAudioEncoderOpus::unloadLib() {
	yang_opus_encoder_create = NULL;
	yang_opus_encoder_init = NULL;
	yang_opus_encode = NULL;
	yang_opus_encoder_ctl = NULL;
	yang_opus_encoder_destroy = NULL;
	yang_opus_strerror = NULL;
}
YangAudioEncoderOpus::YangAudioEncoderOpus() {
	ret = 0;
	m_cbits = NULL;
	m_encoder = NULL;
	m_input1 = NULL;
	m_input = NULL;
	m_in = NULL;
	m_frameShortSize=0;
	m_frameSize=0;
	//temp = NULL;
	unloadLib();
}

YangAudioEncoderOpus::~YangAudioEncoderOpus() {
	closeEnc();
	yang_deleteA(m_cbits);

	yang_deleteA(m_in);
	yang_deleteA(m_input1);
	yang_deleteA(m_input);


	unloadLib();
	m_lib.unloadObject();
}

#define MAX_PACKET_SIZE (3*1276)
void YangAudioEncoderOpus::init(YangAudioInfo *pap) {
	if (m_isInit)
		return;

	m_lib.loadObject("libopus");
	loadLib();
	setAudioPara(pap);
	int32_t err = 0;

	m_encoder = yang_opus_encoder_create(m_audioInfo.sample, m_audioInfo.channel,
			OPUS_APPLICATION_VOIP, &err);//OPUS_APPLICATION_AUDIO
	if (err < 0) {
		fprintf(stderr, "failed to create an Opus encoder: %s\n",
				yang_opus_strerror(err));
#ifdef _MSC_VER
    ExitProcess(1);
#else
    _exit(0);
#endif

	}
	yang_opus_encoder_ctl(m_encoder, OPUS_SET_VBR(0));
	//#define BITRATE 16000
	//#define BITRATE 16000
//yang_opus_encoder_ctl(m_encoder, OPUS_SET_BITRATE(bitrate_bps));
	if (m_audioInfo.usingMono)
		yang_opus_encoder_ctl(m_encoder,
				OPUS_SET_BANDWIDTH(OPUS_BANDWIDTH_WIDEBAND));
//yang_opus_encoder_ctl(m_encoder, OPUS_SET_VBR(use_vbr));
//yang_opus_encoder_ctl(m_encoder, OPUS_SET_VBR_CONSTRAINT(cvbr));
//yang_opus_encoder_ctl(m_encoder, OPUS_SET_COMPLEXITY(10));
//yang_opus_encoder_ctl(m_encoder, OPUS_SET_INBAND_FEC(use_inbandfec));
//yang_opus_encoder_ctl(m_encoder, OPUS_SET_FORCE_CHANNELS(forcechannels));
//yang_opus_encoder_ctl(m_encoder, OPUS_SET_DTX(use_dtx));
//yang_opus_encoder_ctl(m_encoder, OPUS_SET_PACKET_LOSS_PERC(packet_loss_perc));
	int32_t skip = 0;
	yang_opus_encoder_ctl(m_encoder, OPUS_GET_LOOKAHEAD(&skip));
	yang_opus_encoder_ctl(m_encoder, OPUS_SET_LSB_DEPTH(16));
//yang_opus_encoder_ctl(m_encoder, OPUS_SET_EXPERT_FRAME_DURATION(variable_duration));

	m_frameSize=m_audioInfo.sample/50;

	m_frameShortSize=m_frameSize*m_audioInfo.channel;
	m_in = new short[m_frameShortSize];
	m_cbits = new uint8_t[MAX_PACKET_SIZE];
	m_input = new uint8_t[m_frameShortSize*2];
	m_input1 = new short[m_frameShortSize];
	m_isInit = 1;

}


int32_t YangAudioEncoderOpus::encoder(YangFrame* pframe,YangEncoderCallback *pcallback) {
	if (!m_encoder)		return 1;

		memcpy(m_input, pframe->payload, pframe->nb);
		for (int32_t i = 0; i < m_frameShortSize; i++) {
			m_input1[i] = m_input[2 * i + 1] << 8 | m_input[2 * i];
		}

		ret = yang_opus_encode(m_encoder, m_input1, m_frameSize, m_cbits,	MAX_PACKET_SIZE);
		if (ret > 0 && pcallback){
			pframe->payload=m_cbits;
			pframe->nb=ret;
			pcallback->onAudioData(pframe);

		}


	return Yang_Ok;
}
/**
 void YangAudioEncoderOpus::encoder(uint8_t *p_buf,int32_t bufLen){
 if(!m_encoder) return;
 if(m_yap.usingMono){
 memcpy(m_input, p_buf, 640);
 for (int32_t i=0;i<m_yap.frameSize;i++)
 {
 m_input1[i]=m_input[2*i+1]<<8|m_input[2*i];
 }
 ret = yang_opus_encode(m_encoder,m_input1, m_yap.frameSize, m_cbits, MAX_PACKET_SIZE);
 if (ret > 0 && m_pipe)	m_pipe->putPipeData( m_cbits, ret);
 }else{
 memcpy(temp + m_bufLen, p_buf, 4096);
 m_bufLen = m_bufLen + 4096;
 int32_t len = 0;
 int32_t frameSize=(m_yap.sample/50)*4;
 //int32_t readLen=frameSize;
 while (m_bufLen >= frameSize) {
 memcpy((char*)m_in, temp + len, frameSize);
 ret = yang_opus_encode(m_encoder,m_in, frameSize, m_cbits, MAX_PACKET_SIZE);
 if (ret > 0 && m_pipe)	m_pipe->putPipeData((uint8_t*) m_cbits, ret);
 //printf("%d,",ret);
 m_bufLen -= frameSize;
 len += frameSize;
 }
 memcpy(temp, temp + len, m_bufLen);
 }
 }

 void YangAudioEncoderOpus::encoder(uint8_t *p_buf,int32_t bufLen,uint8_t *p_out,int32_t *outLen){

 }
 **/
void YangAudioEncoderOpus::closeEnc() {

	if (m_encoder)
		yang_opus_encoder_destroy(m_encoder);
	m_encoder = NULL;

}
