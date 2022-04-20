//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangutil/yang_unistd.h>
#include <yangp2p/YangP2pCapture.h>
#include <yangavutil/video/YangYuvConvert.h>
#include <yangavutil/video/YangYuvUtil.h>

#include <yangcapture/YangCaptureFactory.h>


YangP2pCapture::YangP2pCapture(YangContext *pcontext) {
	m_context=pcontext;

	m_out_audioBuffer = NULL;

	m_videoCapture=NULL;
	m_audioCapture=NULL;


	m_out_videoBuffer=NULL;

	m_pre_videoBuffer=new YangVideoBuffer(pcontext->avinfo.video.bitDepth==8?1:2);
	m_pre_videoBuffer->isPreview=1;
	m_pre_videoBuffer->m_frames=pcontext->avinfo.video.frame;
	m_isConvert=0;
	m_isStart=0;
	m_aec=NULL;
#if Yang_HaveVr
       m_out_vr_pre_videoBuffer=NULL;
#endif
}

YangP2pCapture::~YangP2pCapture() {
	m_context=NULL;
	stopAll();
	yang_stop_thread(this);
	yang_stop_thread(m_audioCapture);
	yang_stop_thread(m_videoCapture);



	yang_delete(m_audioCapture);
	yang_delete(m_videoCapture);


	yang_delete(m_out_audioBuffer);
	yang_delete(m_pre_videoBuffer);
	yang_delete(m_out_videoBuffer);
#if Yang_HaveVr
	yang_delete(m_out_vr_pre_videoBuffer);
#endif
	yang_destroy_rtcaec(m_aec);
	yang_free(m_aec);
	//yang_delete(m_screen_pre_videoBuffer);

}


void YangP2pCapture::startAudioCaptureState() {
    if(m_audioCapture) m_audioCapture->setCatureStart();
}
void YangP2pCapture::stopAudioCaptureState() {
    if(m_audioCapture) m_audioCapture->setCatureStop();
}
void YangP2pCapture::setAec(YangRtcAec *paec) {
	if (m_audioCapture) {
		m_audioCapture->setAec(paec);
	}
}
void YangP2pCapture::setInAudioBuffer(vector<YangAudioPlayBuffer*> *pbuf){
	if(m_audioCapture!=NULL) m_audioCapture->setInAudioBuffer(pbuf);
}
void YangP2pCapture::startAudioCapture() {
	if (m_audioCapture && !m_audioCapture->m_isStart)
		m_audioCapture->start();
}
YangAudioBuffer* YangP2pCapture::getOutAudioBuffer() {
	return m_out_audioBuffer;
}
int32_t YangP2pCapture::initAudio(YangPreProcess *pp) {
	if (m_out_audioBuffer == NULL) {
			m_out_audioBuffer = new YangAudioBuffer(m_context->avinfo.audio.audioCacheNum);
	}
	if (m_audioCapture == NULL) {
		YangCaptureFactory m_capture;
		//m_audioCapture = m_capture.createAudioCapture(m_context); //new YangAudioCapture(m_context);
		if(m_context->avinfo.audio.hasAec){
			m_audioCapture = m_capture.createAudioCapture(m_context);
			if(m_aec==NULL){
				m_aec=(YangRtcAec*)calloc(sizeof(YangRtcAec),1);
				yang_create_rtcaec(m_aec);
			};
			m_aec->init(m_aec->context,16000,1,320,m_context->avinfo.audio.echoPath);
			m_audioCapture->setAec(m_aec);
		}else{
			m_audioCapture = m_capture.createRecordAudioCapture(m_context);
		}
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

int32_t YangP2pCapture::initAudio2(YangPreProcess *pp) {
	if (m_out_audioBuffer == NULL) {
		if (m_context->avinfo.audio.usingMono)
			m_out_audioBuffer = new YangAudioBuffer(m_context->avinfo.audio.audioCacheNum);
		else
			m_out_audioBuffer = new YangAudioBuffer(m_context->avinfo.audio.audioCacheNum);
	}
	if (m_audioCapture == NULL) {
		YangCaptureFactory m_capture;
		m_audioCapture = m_capture.createAudioCapture(m_context); //new YangAudioCapture(m_context);
		//m_audioCapture = m_capture.createRecordAudioCapture(m_context);
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


void YangP2pCapture::stopAll(){
	stop();
	yang_stop(m_audioCapture);
	yang_stop(m_videoCapture);


}


void YangP2pCapture::startVideoCaptureState() {
	m_videoCapture->initstamp();
	m_videoCapture->setVideoCaptureStart();
}



void YangP2pCapture::stopVideoCaptureState() {
	if(m_videoCapture) m_videoCapture->setVideoCaptureStop();

}

void YangP2pCapture::change(int32_t st) {

}


int32_t YangP2pCapture::initVideo(){
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


void YangP2pCapture::startVideoCapture(){
		if(m_videoCapture&&!m_videoCapture->m_isStart) m_videoCapture->start();
}

YangVideoBuffer * YangP2pCapture::getOutVideoBuffer(){

	return m_out_videoBuffer;
}

YangVideoBuffer * YangP2pCapture::getPreVideoBuffer(){

	return m_pre_videoBuffer;
}
void YangP2pCapture::run() {
	m_isStart = 1;
	startLoop();
	m_isStart = 0;
}
void YangP2pCapture::stop() {
	stopLoop();
}
void YangP2pCapture::stopLoop() {
	m_isConvert = 0;
}

void YangP2pCapture::startCamera() {
	initVideo();
	startVideoCapture();
}


void YangP2pCapture::stopCamera() {
	yang_stop(m_videoCapture);
	yang_stop_thread(m_videoCapture);
	yang_delete(m_videoCapture);
}


