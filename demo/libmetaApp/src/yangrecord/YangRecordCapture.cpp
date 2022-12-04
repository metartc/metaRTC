//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrecord/YangRecordCapture.h>

#include "yangavutil/video/YangYuvConvert.h"
#include "yangavutil/video/YangYuvUtil.h"
#include "yangavutil/video/YangBmp.h"
#include <yangutil/yang_unistd.h>

YangRecordCapture::YangRecordCapture(YangContext* pcontext) {

	m_context=pcontext;
	m_audioCapture = NULL;
	m_videoCapture = NULL;
	m_screenCapture = NULL;

	m_out_audioBuffer = NULL;
	m_out_videoBuffer = NULL;
	m_pre_videoBuffer = new YangVideoBuffer(m_context->avinfo.video.width, m_context->avinfo.video.height, m_context->avinfo.video.videoEncoderFormat,
			m_context->avinfo.video.bitDepth == 8 ? 1 : 2);
	m_vr_videoBuffer = NULL;
	m_screen_videoBuffer = NULL;
	m_pre_videoBuffer->isPreview = 1;
	m_isStart = 0;
	m_isConvert = 0;
	m_isScreen = 0;

}

YangRecordCapture::~YangRecordCapture() {
	if (m_audioCapture)
		m_audioCapture->stop();
	if (m_videoCapture)
		m_videoCapture->stop();

	if (m_audioCapture) {
		while (m_audioCapture->m_isStart) {
			yang_usleep(1000);
		}
	}
	if (m_videoCapture) {
		while (m_videoCapture->m_isStart) {
			yang_usleep(1000);
		}
	}
	m_context=NULL;
	yang_delete(m_pre_videoBuffer);
	yang_delete(m_out_videoBuffer);
	yang_delete(m_out_audioBuffer);


	yang_delete(m_audioCapture);
	yang_delete(m_videoCapture);

}
void YangRecordCapture::stop() {
	stopLoop();
}

void YangRecordCapture::run() {
	m_isStart = 1;
	if (m_isScreen)
		startScreenLoop();
	else
		startVrLoop();
	m_isStart = 0;
}
void YangRecordCapture::stopLoop() {
	m_isConvert = 0;
}

void YangRecordCapture::startAudioCaptureState() {
	m_audioCapture->setCatureStart();
}
void YangRecordCapture::startVideoCaptureState() {
	m_videoCapture->initstamp();
	m_videoCapture->setVideoCaptureStart();
}
void YangRecordCapture::startPauseCaptureState() {
	m_audioCapture->setCatureStop();
	m_videoCapture->setVideoCaptureStop();

}
void YangRecordCapture::stopPauseCaptureState() {
	m_videoCapture->setVideoCaptureStart();
	m_audioCapture->setCatureStart();
}
void YangRecordCapture::stopAudioCaptureState() {
	m_audioCapture->setCatureStop();
}
void YangRecordCapture::stopVideoCaptureState() {
	m_videoCapture->setVideoCaptureStop();

}

void YangRecordCapture::initAudio(YangPreProcess *pp) {
	if (m_out_audioBuffer == NULL) {
		if (m_context->avinfo.audio.enableMono)
			m_out_audioBuffer = new YangAudioBuffer(m_context->avinfo.audio.audioCacheNum);
		else
			m_out_audioBuffer = new YangAudioBuffer(m_context->avinfo.audio.audioCacheNum);
	}
	if (m_audioCapture == NULL) {
		m_audioCapture = m_capture.createRecordAudioCapture(m_context); //.createAudioCapture(m_context);//new YangAudioCapture(m_context);
		m_audioCapture->setPreProcess(pp);
		m_audioCapture->setOutAudioBuffer(m_out_audioBuffer);
		m_audioCapture->init();
		m_audioCapture->setCatureStop();
	}
	stopAudioCaptureState();
}
void YangRecordCapture::initVideo() {
	if (m_out_videoBuffer == NULL)
		m_out_videoBuffer = new YangVideoBuffer(m_context->avinfo.video.width, m_context->avinfo.video.height,
				m_context->avinfo.video.videoEncoderFormat, m_context->avinfo.video.bitDepth == 8 ? 1 : 2);
	if (m_videoCapture == NULL) {
		m_videoCapture = m_capture.createVideoCapture(&m_context->avinfo.video); //new YangVideoCapture(m_context);
		m_videoCapture->setOutVideoBuffer(m_out_videoBuffer);
		m_videoCapture->setPreVideoBuffer(m_pre_videoBuffer);
		m_videoCapture->setVideoCaptureStart();
		m_videoCapture->init();
	}
	stopVideoCaptureState();
}
void YangRecordCapture::startAudioCapture() {
	if (m_audioCapture && !m_audioCapture->m_isStart)
		m_audioCapture->start();
}
void YangRecordCapture::startVideoCapture() {
	if (m_videoCapture && !m_videoCapture->m_isStart)
		m_videoCapture->start();
}
void YangRecordCapture::stopAudioCapture() {
	if (m_audioCapture)
		m_audioCapture->stop();
}
void YangRecordCapture::stopVideoCapture() {
	if (m_videoCapture)
		m_videoCapture->stop();
}
YangAudioBuffer* YangRecordCapture::getOutAudioBuffer() {
	return m_out_audioBuffer;
}
YangVideoBuffer* YangRecordCapture::getOutVideoBuffer() {
	return m_out_videoBuffer;
}

YangVideoBuffer* YangRecordCapture::getPreVideoBuffer() {
	return m_pre_videoBuffer;
}
void YangRecordCapture::setInAudioBuffer(vector<YangAudioPlayBuffer*> *pbuf) {
	if (m_audioCapture != NULL)
		m_audioCapture->setInAudioBuffer(pbuf);
}
void YangRecordCapture::setAec(YangRtcAec *paec) {
	if (m_audioCapture) {
		m_audioCapture->setAec(paec);

	}
}
void YangRecordCapture::startVr(char *pbg) {
	m_bgFileName = string(pbg);
	if (m_vr_videoBuffer == NULL)
		m_vr_videoBuffer = new YangVideoBuffer(m_context->avinfo.video.width,m_context->avinfo.video.height,
				m_context->avinfo.video.videoEncoderFormat, m_context->avinfo.video.bitDepth == 8 ? 1 : 2);
	if (m_videoCapture) {
		m_videoCapture->setPreVideoBuffer(m_vr_videoBuffer);
		m_videoCapture->setOutVideoBuffer(NULL);
	}
	yang_reindex(m_pre_videoBuffer);
	yang_reindex(m_out_videoBuffer);
	start();
}

void YangRecordCapture::stopVr() {
	stop();
	yang_reindex(m_pre_videoBuffer);
	yang_reindex(m_out_videoBuffer);
	if (m_videoCapture) {
		m_videoCapture->setPreVideoBuffer(m_pre_videoBuffer);
		m_videoCapture->setOutVideoBuffer(m_out_videoBuffer);
	}

}

void YangRecordCapture::startScreen() {
	m_isScreen = 1;
	if (m_screen_videoBuffer == NULL)
		m_screen_videoBuffer = new YangVideoBuffer(m_context->avinfo.video.width,
				m_context->avinfo.video.height, m_context->avinfo.video.videoEncoderFormat, m_context->avinfo.video.bitDepth == 8 ? 1 : 2);
	if (m_videoCapture) {
		m_videoCapture->setPreVideoBuffer(NULL);
		m_videoCapture->setOutVideoBuffer(NULL);
	}

	if (m_screenCapture) {
		//m_screenCapture->setPreVideoBuffer(m_screen_videoBuffer);
        //m_screenCapture->setOutVideoBuffer(m_screen_videoBuffer);
	}
	yang_reindex(m_pre_videoBuffer);
	yang_reindex(m_screen_videoBuffer);
	start();
}
void YangRecordCapture::stopScreen() {
	m_isScreen = 0;
	if (m_videoCapture) {
		m_videoCapture->setPreVideoBuffer(m_pre_videoBuffer);
		m_videoCapture->setOutVideoBuffer(m_out_videoBuffer);
	}
}

void YangRecordCapture::startVrLoop() {

}
void YangRecordCapture::startScreenLoop() {

}

