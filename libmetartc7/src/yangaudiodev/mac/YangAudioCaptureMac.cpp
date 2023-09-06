//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangutil/yangtype.h>
#if Yang_OS_APPLE
#include <yangaudiodev/mac/YangAudioCaptureMac.h>

#include <yangutil/sys/YangMath.h>
#include <yangavutil/audio/YangAudioUtil.h>

void yang_mac_audio_capture_callback(uint8_t* data,uint32_t nb,void* user){
    YangAudioCaptureMac* mac=(YangAudioCaptureMac*)user;
    if(mac) mac->on_audio(data,nb);
}

YangAudioCaptureMac::YangAudioCaptureMac(YangAVInfo *avinfo) //:YangAudioCapture(pcontext)
		{

	m_avinfo = avinfo;
	m_ahandle = new YangAudioCaptureHandle(avinfo);
	aIndex = 0;

    m_loops = yangfalse;
    m_isInited=yangfalse;
    m_isStart=yangfalse;
	m_buffer = NULL;
    m_bufferList= NULL;
    m_macAudio=NULL;
	m_channel = avinfo->audio.channel;
	m_sample = avinfo->audio.sample;
    m_size=0;
    //onlySupportSingle = yangfalse;

    m_bufferLen=0;
    m_buffer=NULL;
    m_callback.user=this;
    m_callback.on_audio=yang_mac_audio_capture_callback;
    m_callback.getRenderData=NULL;
    m_macSample=48000;
    m_macChannel=2;
    memset(&m_audioFrame,0,sizeof(YangFrame));
    memset(&m_resample,0,sizeof(YangAudioResample));
    yang_create_audioresample(&m_resample);

}

YangAudioCaptureMac::~YangAudioCaptureMac() {
	if (m_loops) {
		stop();
		while (m_isStart) {
			yang_usleep(500);
		}
	}

    yang_free(m_buffer);
	yang_delete(m_ahandle);
    yang_delete(m_macAudio);
    yang_delete(m_bufferList);
    yang_destroy_audioresample(&m_resample);

}
void YangAudioCaptureMac::setCatureStart() {
	m_ahandle->m_enableBuf = 1;
}
void YangAudioCaptureMac::setCatureStop() {
	m_ahandle->m_enableBuf = 0;
}
void YangAudioCaptureMac::setOutAudioBuffer(YangAudioBuffer *pbuffer) {
	m_ahandle->setOutAudioBuffer(pbuffer);
}
void YangAudioCaptureMac::setPlayAudoBuffer(YangAudioBuffer *pbuffer) {
	m_ahandle->m_aecPlayBuffer = pbuffer;
}
void YangAudioCaptureMac::setAec(YangRtcAec *paec) {
	m_ahandle->m_aec = paec;
}
void YangAudioCaptureMac::setInAudioBuffer(
		vector<YangAudioPlayBuffer*> *pal) {

}
void YangAudioCaptureMac::setPreProcess(YangPreProcess *pp) {

}

int32_t YangAudioCaptureMac::init() {
    if(m_isInited) return Yang_Ok;
    if(m_macAudio==NULL){
        m_macAudio=new YangAudioMac(true);
        m_macAudio->setInputCallback(&m_callback);

    }
    if(m_macAudio->init()!=Yang_Ok){
        return 1;
    }
    m_macSample=m_macAudio->getSample();
    m_macChannel=m_macAudio->getChannel();
    m_resample.init(m_resample.context,m_macSample,m_macChannel,m_sample,m_channel,20);

    m_size = m_macSample*m_macChannel*2/50; // 2 bytes/sample, 2 channels
    m_buffer = (uint8_t*) malloc(4096*4);
    m_bufferList=new YangAudioPlayBuffer();
    m_isInited=yangtrue;
	return Yang_Ok;
}

void YangAudioCaptureMac::on_audio(uint8_t* pcm,uint32_t pnb){
if(!m_isInited) return;
    if(m_bufferList){
        m_audioFrame.nb=pnb;
        m_audioFrame.payload=pcm;
        m_bufferList->putAudio(&m_audioFrame);

    }
}



void YangAudioCaptureMac::startLoop() {

    if(!m_isInited) {
        yang_error("init fail,not ready!");
        return;
    }
    if(m_macAudio){
        if( m_macAudio->start()!=Yang_Ok){
            yang_error("mac audio capture start fail!");
        }

    }

    int32_t nb=0;
    int16_t* data=NULL;
    YangFrame audioFrame;
    uint32_t frames=0;
    memset(&audioFrame,0,sizeof(audioFrame));
    uint8_t *pcm=NULL;
    m_loops = yangtrue;
    while(m_loops){
        if(m_bufferList->size()==0){
            yang_usleep(1000);
            continue;
        }

        pcm=m_bufferList->getAudios(&audioFrame);

        if(pcm==NULL) continue;

        nb=audioFrame.nb/2;
        data=(int16_t*)m_buffer;
        float* fpcm=(float*)pcm;
        frames=audioFrame.nb/4;
        for(uint32_t i=0;i<frames;i++){
            data[i]=yang_floattoint16(fpcm[i]);
        }


        audioFrame.payload = m_buffer;
        audioFrame.nb = m_size;
        m_resample.resample(m_resample.context,&audioFrame);

        if(m_ahandle)
            m_ahandle->putBuffer(audioFrame.payload,audioFrame.nb);

    }


}

void YangAudioCaptureMac::stopLoop() {
    m_loops = yangfalse;
    if(m_macAudio) m_macAudio->stop();
    m_isStart=yangfalse;
}
#endif
