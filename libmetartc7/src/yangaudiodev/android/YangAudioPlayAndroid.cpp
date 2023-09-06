//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangaudiodev/android/YangAudioPlayAndroid.h>
#include <yangutil/yangautofree.h>
#include <yangutil/sys/YangLog.h>
#if Yang_OS_ANDROID

YangAudioPlayAndroid::YangAudioPlayAndroid(YangAVInfo* avinfo,YangSynBufferManager* streams):YangAudioPlay(avinfo,streams){
	m_audioAndroid=(YangAudioAndroid*)calloc(sizeof(YangAudioAndroid),1);

	m_loops = 0;
	m_frames = 0;
	m_isStart = 0;
	m_isInit = 0;
}

YangAudioPlayAndroid::~YangAudioPlayAndroid() {
	closeAudio();
}

int YangAudioPlayAndroid::init() {
	if (m_isInit == 1)
		return Yang_Ok;

	m_frames = m_avinfo->audio.sample / 50;
	if(yang_create_audioAndroid_play(m_audioAndroid, m_avinfo->audio.sample,m_avinfo->audio.channel)!=Yang_Ok){
		return yang_error_wrap(ERROR_SYS_AudioRender,"init android play fail");
	}
	uint32_t val = 0;
	int32_t dir = 0;

	m_audioData.initRender(m_sample,m_channel);
	m_isInit = 1;
	return Yang_Ok;

}

void YangAudioPlayAndroid::closeAudio() {

	yang_destroy_audioAndroid(m_audioAndroid);
	yang_free(m_audioAndroid);

}

void YangAudioPlayAndroid::stopLoop() {
	m_loops = 0;
}

void YangAudioPlayAndroid::startLoop() {
	if(m_isInit==0) return;
	m_loops = 1;
	unsigned long status = 0;
	uint8_t *pcm = new uint8_t[4096*2];
	memset(pcm,0,4096*2);
	YangAutoFreeA(uint8_t,pcm);

	YangFrame frame;
	memset(&frame,0,sizeof(YangFrame));
	int err = 0;
	uint8_t* tmp=NULL;

	int32_t audiolen = m_frames * m_channel * 2;
	while (m_loops == 1) {

		if(m_audioAndroid==NULL||m_audioAndroid->getInputdataCount(m_audioAndroid->context)!=0) {
			usleep(1000);
			continue;
		}

			tmp =m_audioData.getRenderAudioData(audiolen);

			if (tmp){
				m_audioAndroid->audioOut(m_audioAndroid->context,(uint16_t*)tmp,audiolen);
			}else{
				m_audioAndroid->audioOut(m_audioAndroid->context,(uint16_t*)pcm,audiolen);
			}
	}
}
#endif

