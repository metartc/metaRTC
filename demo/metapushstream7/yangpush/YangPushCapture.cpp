//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangpush/YangPushCapture.h>

#include <yangavutil/video/YangYuvConvert.h>
#include <yangavutil/video/YangYuvUtil.h>

#include <yangcapture/YangCaptureFactory.h>


YangPushCapture::YangPushCapture(YangContext *pcontext) {
	m_context=pcontext;

	m_out_audioBuffer = NULL;
	m_screenCapture=NULL;
	m_videoCapture=NULL;
	m_audioCapture=NULL;


	m_out_videoBuffer=NULL;
	m_screen_pre_videoBuffer=NULL;
	m_screen_out_videoBuffer=NULL;
	m_pre_videoBuffer=new YangVideoBuffer(pcontext->avinfo.video.bitDepth==8?1:2);
	m_pre_videoBuffer->isPreview=1;
	m_pre_videoBuffer->m_frames=pcontext->avinfo.video.frame;
	m_isConvert=0;
	m_isStart=0;

}

YangPushCapture::~YangPushCapture() {
	m_context=NULL;
	stopAll();
	yang_stop_thread(this);
	yang_stop_thread(m_audioCapture);
	yang_stop_thread(m_videoCapture);
	yang_stop_thread(m_screenCapture);


	yang_delete(m_audioCapture);
	yang_delete(m_videoCapture);
	yang_delete(m_screenCapture);

	yang_delete(m_out_audioBuffer);
	yang_delete(m_pre_videoBuffer);
	yang_delete(m_out_videoBuffer);

	m_screen_pre_videoBuffer=NULL;
	m_screen_out_videoBuffer=NULL;


}


void YangPushCapture::startAudioCaptureState() {
    if(m_audioCapture) m_audioCapture->setCatureStart();
}
void YangPushCapture::stopAudioCaptureState() {
    if(m_audioCapture) m_audioCapture->setCatureStop();
}
void YangPushCapture::setAec(YangRtcAec *paec) {
	if (m_audioCapture) {
		m_audioCapture->setAec(paec);
	}
}
void YangPushCapture::setInAudioBuffer(vector<YangAudioPlayBuffer*> *pbuf){
	if(m_audioCapture!=NULL) m_audioCapture->setInAudioBuffer(pbuf);
}
void YangPushCapture::startAudioCapture() {

	if (m_audioCapture && !m_audioCapture->m_isStart)
		m_audioCapture->start();
}
YangAudioBuffer* YangPushCapture::getOutAudioBuffer() {
	return m_out_audioBuffer;
}
int32_t YangPushCapture::initAudio(YangPreProcess *pp) {
	if (m_out_audioBuffer == NULL) {
		if (m_context->avinfo.audio.enableMono)
			m_out_audioBuffer = new YangAudioBuffer(m_context->avinfo.audio.audioCacheNum);
		else
			m_out_audioBuffer = new YangAudioBuffer(m_context->avinfo.audio.audioCacheNum);
	}
	if (m_audioCapture == NULL) {
		YangCaptureFactory m_capture;
		m_audioCapture = m_capture.createRecordAudioCapture(&m_context->avinfo); //new YangAudioCapture(m_context);
		int32_t ret=m_audioCapture->init();
		if(ret){
			if(ret==ERROR_SYS_NoAudioDevice||ret==ERROR_SYS_NoAudioCaptureDevice) {
				yang_error("ERROR_SYS_NoAudioDevice");
					return ret;
			}

		}
		m_audioCapture->setPreProcess(pp);
		m_audioCapture->setOutAudioBuffer(m_out_audioBuffer);

		m_audioCapture->setCatureStop();
	}
	stopAudioCaptureState();
	return Yang_Ok;
}




void YangPushCapture::stopAll(){
	stop();
	yang_stop(m_audioCapture);
	yang_stop(m_videoCapture);
	yang_stop(m_screenCapture);

}


void YangPushCapture::startVideoCaptureState() {
	m_videoCapture->initstamp();
	m_videoCapture->setVideoCaptureStart();
}

void YangPushCapture::startScreenCaptureState() {

	m_screenCapture->setVideoCaptureStart();
}

void YangPushCapture::stopVideoCaptureState() {
	if(m_videoCapture) m_videoCapture->setVideoCaptureStop();

}
void YangPushCapture::stopScreenCaptureState(){
	if(m_screenCapture) m_screenCapture->setVideoCaptureStop();
}
void YangPushCapture::change(int32_t st) {

}


int32_t YangPushCapture::initVideo(){
	if(m_out_videoBuffer==NULL) m_out_videoBuffer = new YangVideoBuffer(m_context->avinfo.video.bitDepth==8?1:2);
	int32_t err=Yang_Ok;
	if (m_videoCapture == NULL) {
		YangCaptureFactory cf;
#ifdef __ANDROID__
		m_videoCapture = cf.createRecordVideoCaptureAndroid(&m_context->avinfo.video,m_context->nativeWindow);
#else
		m_videoCapture = cf.createRecordVideoCapture(&m_context->avinfo.video);//new YangVideoCapture(m_context);
#endif

		if((err=m_videoCapture->init())!=Yang_Ok){
			return yang_error_wrap(err,"video capture init fail!");
		}

		m_out_videoBuffer->init(m_context->avinfo.video.width,m_context->avinfo.video.height,m_context->avinfo.video.videoEncoderFormat);
		m_pre_videoBuffer->init(m_context->avinfo.video.width,m_context->avinfo.video.height,m_context->avinfo.video.videoEncoderFormat);
		m_videoCapture->setOutVideoBuffer(m_out_videoBuffer);
		m_videoCapture->setPreVideoBuffer(m_pre_videoBuffer);
		//m_videoCapture->setVideoCaptureStart();
	}
	stopVideoCaptureState();
	return err;

}

void YangPushCapture::startVideoCapture(){
		if(m_videoCapture&&!m_videoCapture->m_isStart) m_videoCapture->start();
}
void YangPushCapture::startScreenCapture(){
	if(m_screenCapture&&!m_screenCapture->m_isStart) m_screenCapture->start();
}
YangVideoBuffer * YangPushCapture::getOutVideoBuffer(){

	return m_out_videoBuffer;
}

YangVideoBuffer * YangPushCapture::getPreVideoBuffer(){

	return m_pre_videoBuffer;
}
void YangPushCapture::run() {
	m_isStart = 1;
	startLoop();
	m_isStart = 0;
}
void YangPushCapture::stop() {
	stopLoop();
}
void YangPushCapture::stopLoop() {
	m_isConvert = 0;
}

YangVideoBuffer* YangPushCapture::getScreenOutVideoBuffer() {
	return m_screen_out_videoBuffer;
}

YangVideoBuffer* YangPushCapture::getScreenPreVideoBuffer() {
	return m_screen_pre_videoBuffer;
}

void YangPushCapture::startCamera() {
	initVideo();
	startVideoCapture();
}



void YangPushCapture::stopCamera() {
	yang_stop(m_videoCapture);
	yang_stop_thread(m_videoCapture);
	yang_delete(m_videoCapture);
}


void YangPushCapture::setScreenInterval(int32_t pinterval) {
	if(m_screenCapture) m_screenCapture->setInterval(pinterval);
}

void YangPushCapture::setDrawmouse(bool isDraw) {
	if(m_screenCapture) m_screenCapture->setDrawmouse(isDraw);
}
