//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangp2p2/YangP2pCaputreCamera.h>
#include <yangcapture/YangCaptureFactory.h>
YangP2pCaputreCamera::YangP2pCaputreCamera(YangContext *pcontext) {
	m_context=pcontext;
	m_videoCapture=NULL;
	m_audioCapture=NULL;
	m_aec=NULL;

	m_pre_videoBuffer=new YangVideoBuffer(pcontext->avinfo.video.bitDepth==8?1:2);
	m_pre_videoBuffer->isPreview=1;
	m_pre_videoBuffer->m_frames=pcontext->avinfo.video.frame;
}

YangP2pCaputreCamera::~YangP2pCaputreCamera() {
	stopAll();
	yang_stop_thread(m_audioCapture);
	yang_stop_thread(m_videoCapture);
	yang_delete(m_audioCapture);
	yang_delete(m_videoCapture);
	yang_destroy_rtcaec(m_aec);
	yang_free(m_aec);
	yang_delete(m_out_audioBuffer);
	yang_delete(m_pre_videoBuffer);
	yang_delete(m_out_videoBuffer);
}

int32_t YangP2pCaputreCamera::change(int32_t st){

	return Yang_Ok;
}
void YangP2pCaputreCamera::stopAll(){

	yang_stop(m_audioCapture);
	yang_stop(m_videoCapture);


}

void YangP2pCaputreCamera::startAudioCaptureState() {
    if(m_audioCapture) m_audioCapture->setCatureStart();
}
void YangP2pCaputreCamera::stopAudioCaptureState() {
    if(m_audioCapture) m_audioCapture->setCatureStop();
}
void YangP2pCaputreCamera::setAec(YangRtcAec *paec) {
	if (m_audioCapture) {
		m_audioCapture->setAec(paec);
	}
}
void YangP2pCaputreCamera::setInAudioBuffer(vector<YangAudioPlayBuffer*> *pbuf){
	if(m_audioCapture!=NULL) m_audioCapture->setInAudioBuffer(pbuf);
}
void YangP2pCaputreCamera::startAudioCapture() {
	if (m_audioCapture && !m_audioCapture->m_isStart)
		m_audioCapture->start();
}
YangAudioBuffer* YangP2pCaputreCamera::getOutAudioBuffer() {
	return m_out_audioBuffer;
}
int32_t YangP2pCaputreCamera::initAudio(YangPreProcess *pp) {
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
            m_aec->init(m_aec->session,16000,1,320,m_context->avinfo.audio.echoPath);
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

int32_t YangP2pCaputreCamera::initAudio2(YangPreProcess *pp) {
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




void YangP2pCaputreCamera::startVideoCaptureState() {
	m_videoCapture->initstamp();
	m_videoCapture->setVideoCaptureStart();
}



void YangP2pCaputreCamera::stopVideoCaptureState() {
	if(m_videoCapture) m_videoCapture->setVideoCaptureStop();

}




int32_t YangP2pCaputreCamera::initVideo(){
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


void YangP2pCaputreCamera::startVideoCapture(){
		if(m_videoCapture&&!m_videoCapture->m_isStart) m_videoCapture->start();
}

void YangP2pCaputreCamera::startVideoSource() {
	initVideo();
	startVideoCapture();
}


void YangP2pCaputreCamera::stopVideoSource() {
	yang_stop(m_videoCapture);
	yang_stop_thread(m_videoCapture);
	yang_delete(m_videoCapture);
}

