
//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangavutil/audio/YangAudioUtil.h>

#if Yang_OS_ANDROID
#include <yangaudiodev/android/YangAudioCaptureAndroid.h>

void g_yang_androidaudio_bqRecorderCallback(uint8_t* data,int32_t nb,void* user)
{

	if(user==NULL) return;

	YangAudioCaptureHandle* ah=(YangAudioCaptureHandle*)user;

	ah->putBuffer2(data, nb);

}
YangAudioCaptureAndroid::YangAudioCaptureAndroid(YangAVInfo* avinfo) //:YangAudioCapture(pcontext)
		{

	//m_context = pcontext;
    m_ahandle = new YangAudioCaptureHandle(avinfo);
	aIndex = 0;
	m_size = 0;
	m_loops = 0;
	m_channel =avinfo->audio.channel;
	m_sample = avinfo->audio.sample;

	m_audioAndroid=(YangAudioAndroid*)yang_calloc(sizeof(YangAudioAndroid),1);

}

YangAudioCaptureAndroid::~YangAudioCaptureAndroid() {
	/*if (m_loops) {
		stop();
		while (m_isStart) {
			yang_usleep(500);
		}
	}*/
	closeAudio();
}
void YangAudioCaptureAndroid::closeAudio() {

	yang_destroy_audioAndroid(m_audioAndroid);
	yang_free(m_audioAndroid);

}
void YangAudioCaptureAndroid::setCatureStart() {
	m_ahandle->m_enableBuf = 1;
}
void YangAudioCaptureAndroid::setCatureStop() {
	m_ahandle->m_enableBuf = 0;
}
void YangAudioCaptureAndroid::setOutAudioBuffer(YangAudioBuffer *pbuffer) {
	m_ahandle->setOutAudioBuffer(pbuffer);
}
void YangAudioCaptureAndroid::setPlayAudoBuffer(YangAudioBuffer *pbuffer) {
	m_ahandle->m_aecPlayBuffer = pbuffer;
}
void YangAudioCaptureAndroid::setAec(YangRtcAec *paec) {
	m_ahandle->m_aec = paec;
}
void YangAudioCaptureAndroid::setInAudioBuffer(vector<YangAudioPlayBuffer*> *pal) {

}
void YangAudioCaptureAndroid::setPreProcess(YangPreProcess *pp) {

}


int32_t YangAudioCaptureAndroid::init() {
	if(yang_create_audioAndroid_record(m_audioAndroid,g_yang_androidaudio_bqRecorderCallback,m_ahandle, m_sample,m_channel)!=Yang_Ok){
		return yang_error_wrap(ERROR_SYS_AudioRender,"init android record fail");
	}

	return Yang_Ok;
}

void YangAudioCaptureAndroid::startLoop() {

	m_loops = 1;

	m_loops = 0;

}

void YangAudioCaptureAndroid::stopLoop() {
	m_loops = 0;
}
#endif
