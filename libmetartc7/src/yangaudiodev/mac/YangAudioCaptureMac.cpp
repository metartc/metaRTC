//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangaudiodev/mac/YangAudioCaptureMac.h>
#if Yang_OS_APPLE

#include <yangutil/sys/YangMath.h>
#include <yangavutil/audio/YangAudioUtil.h>

void yang_mac_audio_capture_callback(uint8_t* data,uint32_t nb,void* user){
	YangAudioCaptureMac* mac=(YangAudioCaptureMac*)user;
	if(mac)
		mac->on_audio(data,nb);
}

YangAudioCaptureMac::YangAudioCaptureMac(YangAVInfo *avinfo) //:YangAudioCapture(pcontext)
{
	m_avinfo = avinfo;
	m_ahandle = new YangAudioCaptureHandle(avinfo);
	aIndex = 0;

	m_loops = yangfalse;
	m_isInited=yangfalse;
	m_isStart=yangfalse;

	m_macAudio=NULL;
	m_channel = avinfo->audio.channel;
	m_sample = avinfo->audio.sample;

	m_callback.user=this;
	m_callback.on_audio=yang_mac_audio_capture_callback;
	m_callback.getRenderData=NULL;

	memset(&m_audioFrame,0,sizeof(YangFrame));
}

YangAudioCaptureMac::~YangAudioCaptureMac() {
	if (m_loops) {
		stop();
		while (m_isStart) {
			yang_usleep(500);
		}
	}

	yang_delete(m_ahandle);
	yang_destroy_audioMac(m_macAudio);
	yang_free(m_macAudio);

}


void YangAudioCaptureMac::setCaptureVolume(int32_t vol){

}

void YangAudioCaptureMac::setPlayVolume(int32_t vol){

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

void YangAudioCaptureMac::setPlayAudioParam(int32_t puid,YangAudioParam* audioParam){
	if(audioParam==NULL) return;
	//m_audioData.initPlay(audioParam->sample, audioParam->channel);
}


int32_t YangAudioCaptureMac::init() {
	if(m_isInited)
		return Yang_Ok;

	if(m_macAudio==NULL){
		m_macAudio=(YangAudioMac*)yang_calloc(sizeof(YangAudioMac),1);
		yang_create_audioMac(m_macAudio,yangtrue,m_sample,m_channel);
		m_macAudio->setAudioCallback(m_macAudio->session,&m_callback);

	}

	if(m_macAudio->init(m_macAudio->session)!=Yang_Ok){
		return 1;
	}

	m_isInited=yangtrue;
	return Yang_Ok;
}

void YangAudioCaptureMac::on_audio(uint8_t* pcm,uint32_t pnb){
	if(!m_isInited)
		return;

	if(m_ahandle)
			m_ahandle->putBuffer(pcm,pnb);
}

void YangAudioCaptureMac::startLoop() {

	if(!m_isInited) {
		yang_error("init fail,not ready!");
		return;
	}
	if(m_macAudio){
		if( m_macAudio->start(m_macAudio->session)!=Yang_Ok){
			yang_error("mac audio capture start fail!");
		}

	}

	m_isStart=yangtrue;
	m_loops = yangtrue;
	while(m_loops){
		yang_usleep(100*1000);

	}

	m_isStart=yangfalse;
}

void YangAudioCaptureMac::stopLoop() {
	m_loops = yangfalse;
	if(m_macAudio)
		m_macAudio->stop(m_macAudio->session);
	//m_isStart=yangfalse;
}
#endif
