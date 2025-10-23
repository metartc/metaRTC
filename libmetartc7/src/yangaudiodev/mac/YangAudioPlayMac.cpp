//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangaudiodev/mac/YangAudioPlayMac.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangMath.h>

#if Yang_OS_APPLE

YangFrame* yang_mac_audio_render_callback(void* user){
	YangAudioPlayMac* mac=(YangAudioPlayMac*)user;
	if(mac)
		return mac->on_audio();
	return NULL;
}

YangAudioPlayMac::YangAudioPlayMac(YangAVInfo* avinfo,YangSynBufferManager* streams):YangAudioPlay(avinfo,streams){

	m_isStart = yangfalse;
	m_isInited = yangfalse;

	m_frames = 0;
	m_channel = avinfo->audio.channel;
	m_sample = avinfo->audio.sample;
	audioLen=m_sample*m_channel*2/50;
	m_macAudio=NULL;
	m_callback.user=this;
	m_callback.on_audio=NULL;
	m_callback.getRenderData=yang_mac_audio_render_callback;

	m_audiolen = m_sample*m_channel*2/50;

}

YangAudioPlayMac::~YangAudioPlayMac() {

	closeAudio();
}

int YangAudioPlayMac::init() {
	if (m_isInited)
		return Yang_Ok;

	if(m_macAudio==NULL){
		m_macAudio=(YangAudioMac*)yang_calloc(sizeof(YangAudioMac),1);
		yang_create_audioMac(m_macAudio,yangfalse,m_sample,m_channel);
		m_macAudio->setAudioCallback(m_macAudio->session,&m_callback);
		m_macAudio->init(m_macAudio->session);

	}

	m_audioData.initRender(m_sample,m_channel);
	m_audioData.initPlay(m_sample,m_channel);
	m_isInited=yangtrue;
	return Yang_Ok;

}

YangFrame* YangAudioPlayMac::on_audio(){

	uint8_t* tmp =m_audioData.getRenderAudioData(m_audiolen);
	if(tmp){
		m_audioFrame.nb=m_audiolen;
		m_audioFrame.payload=tmp;
		return &m_audioFrame;
	}

	return NULL;
}

void YangAudioPlayMac::closeAudio() {

	yang_destroy_audioMac(m_macAudio);
	yang_free(m_macAudio);

}

void YangAudioPlayMac::start(){
	m_isStart=yangtrue;

	if(m_macAudio)
		m_macAudio->start(m_macAudio->session);
}


void YangAudioPlayMac::stopLoop() {

	if(m_macAudio)
		m_macAudio->stop(m_macAudio->session);

	m_isStart=yangfalse;
}

void YangAudioPlayMac::startLoop() {

}

#endif
