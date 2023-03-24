//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangdecoder/pc/YangAudioDecoderAac.h>
#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangLog.h>



void YangAudioDecoderAac::loadLib(){
	yang_NeAACDecOpen=( NeAACDecHandle (*)(void))m_lib.loadFunction("NeAACDecOpen");
	yang_NeAACDecGetCurrentConfiguration=( NeAACDecConfigurationPtr (*)(NeAACDecHandle hDecoder))m_lib.loadFunction("NeAACDecGetCurrentConfiguration");
	yang_NeAACDecSetConfiguration=( uint8_t (*)(NeAACDecHandle hDecoder,
	                                                   NeAACDecConfigurationPtr config))m_lib.loadFunction("NeAACDecSetConfiguration");
	yang_NeAACDecInit=( long (*)(NeAACDecHandle hDecoder,
	                              uint8_t *buffer,
	                              unsigned long buffer_size,
	                              unsigned long *samplerate,
	                              uint8_t *channels))m_lib.loadFunction("NeAACDecInit");
	yang_NeAACDecDecode=( void* (*)(NeAACDecHandle hDecoder,
	                                 NeAACDecFrameInfo *hInfo,
	                                 uint8_t *buffer,
	                                 unsigned long buffer_size))m_lib.loadFunction("NeAACDecDecode");
	yang_NeAACDecClose=( void (*)(NeAACDecHandle hDecoder))m_lib.loadFunction("NeAACDecClose");

}


void YangAudioDecoderAac::unloadLib(){
	yang_NeAACDecOpen=NULL;
	yang_NeAACDecGetCurrentConfiguration=NULL;
	yang_NeAACDecSetConfiguration=NULL;
	yang_NeAACDecInit=NULL;
	yang_NeAACDecDecode=NULL;
	yang_NeAACDecClose=NULL;


}
YangAudioDecoderAac::YangAudioDecoderAac(YangAudioParam *pcontext) {
	m_context=pcontext;
	m_samplerate = 44100;
	m_channel = 2;
	m_bufLen = 4096;
	isFirst = 1;
	isConvert = 0;
	m_handle = NULL;
	m_buffer = new uint8_t[4096];
	temp = NULL;
	unloadLib();
}

YangAudioDecoderAac::~YangAudioDecoderAac() {

	closeAacdec();
	temp = NULL;
	m_handle = NULL;
	yang_deleteA(m_buffer);
	unloadLib();
	m_lib.unloadObject();
}

void YangAudioDecoderAac::init() {

	if(m_isInit) return;
	m_lib.loadObject("libfaad");
	loadLib();
	if (m_handle == NULL)
		m_handle = yang_NeAACDecOpen();
	if (!m_handle) {
		yang_error("NeAACDecOpen failed");

	}
	NeAACDecConfigurationPtr conf = yang_NeAACDecGetCurrentConfiguration(m_handle);
	if (!conf) {
		printf("NeAACDecGetCurrentConfiguration failed\n");
		// goto error;
	}
	conf->defObjectType = LC;
	conf->defSampleRate = 44100;

	conf->outputFormat = FAAD_FMT_16BIT;
	conf->dontUpSampleImplicitSBR = 1;

	yang_NeAACDecSetConfiguration(m_handle, conf);
	m_alen=4096;
	m_isInit=1;
}


int32_t YangAudioDecoderAac::decode(YangFrame* pframe,YangDecoderCallback* pcallback){
	if (isFirst&&m_handle) {
		long res = yang_NeAACDecInit(m_handle, m_buffer, m_bufLen, &m_samplerate,&m_channel);
		isFirst = 0;
	}

	if(m_handle) temp = (uint8_t *) yang_NeAACDecDecode(m_handle, &m_info, pframe->payload, pframe->nb);

	if (temp&&pcallback){
		pframe->payload=temp;
		pframe->nb=4096;
		pcallback->onAudioData(pframe);
	}
	return Yang_Ok;

}

void YangAudioDecoderAac::closeAacdec() {
	if (m_handle) {
		yang_NeAACDecClose(m_handle);
	}
	m_handle = NULL;
}

