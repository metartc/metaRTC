//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangaudiodev/mac/YangAudioPlayMac.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangMath.h>
#include <yangutil/yangautofree.h>
#if Yang_OS_APPLE

int32_t yang_mac_audio_render_callback(YangFrame* audioFrame,void* user){
    YangAudioPlayMac* mac=(YangAudioPlayMac*)user;
    if(mac) return mac->on_audio(audioFrame);
    return 1;
}

YangAudioPlayMac::YangAudioPlayMac(YangAVInfo* avinfo,YangSynBufferManager* streams):YangAudioPlay(avinfo,streams){

    m_loops = yangfalse;
    m_isStart = yangfalse;
    m_isInited = yangfalse;
	ret = 0;
	m_frames = 0;
    m_channel = avinfo->audio.channel;
    m_sample = avinfo->audio.sample;
    m_macAudio=NULL;
    m_callback.user=this;
    m_callback.on_audio=NULL;
    m_callback.getRenderData=yang_mac_audio_render_callback;
    m_macSample=48000;
    m_macChannel=2;
    m_audiolen = m_frames * m_channel * 2;
    m_buffer=NULL;
    //memset(&m_audioFrame,0,sizeof(YangFrame));

}

YangAudioPlayMac::~YangAudioPlayMac() {

	closeAudio();
    yang_free(m_buffer);
}

int YangAudioPlayMac::init() {
    if (m_isInited)
		return Yang_Ok;



    if(m_macAudio==NULL){
        m_macAudio=new YangAudioMac(false);
        m_macAudio->setInputCallback(&m_callback);
        m_macAudio->init();
        m_macSample=m_macAudio->getSample();
        m_macChannel=m_macAudio->getChannel();
    }

    m_frames = m_macSample *m_channel/ 50;
    m_audiolen=m_frames*2;
    if(m_buffer==NULL) m_buffer=(uint8_t*)malloc(m_frames*sizeof(float));
    m_audioData.initRender(m_macSample,m_macChannel);
    m_audioData.initPlay(m_sample,m_channel);
    m_isInited=yangtrue;
	return Yang_Ok;

}

int32_t YangAudioPlayMac::on_audio(YangFrame* audioFrame){
    int16_t* tmp;
    float* data=(float*)m_buffer;
    if(audioFrame==NULL) return 1;
    tmp =(int16_t*)m_audioData.getRenderAudioData(m_audiolen);
    if(tmp){
        for(int i=0;i<m_frames;i++)
            data[i]=yang_int16tofloat(tmp[i]);
        audioFrame->nb=m_audiolen*2;
        audioFrame->payload=m_buffer;
        return Yang_Ok;
    }
    return 1;
}

void YangAudioPlayMac::closeAudio() {

    yang_delete(m_macAudio);
    //yang_destroy_audioresample(&m_resample);

}

void YangAudioPlayMac::start(){
    m_isStart=yangtrue;
    m_loops = yangtrue;
    if(m_macAudio) m_macAudio->start();
}


void YangAudioPlayMac::stopLoop() {
    m_loops = yangfalse;
    if(m_macAudio) m_macAudio->stop();
    m_isStart=yangfalse;
}

void YangAudioPlayMac::startLoop() {

}

#endif
