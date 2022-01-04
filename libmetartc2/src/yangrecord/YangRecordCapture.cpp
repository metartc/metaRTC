/*
 * YangCaptureBase.cpp
 *
 *  Created on: 2019年10月11日
 *      Author: yang
 */

#include <yangrecord/YangRecordCapture.h>
//#include "yangavutil/video/YangMatImage.h"
//#include "yangavutil/vr/YangMatImageCv.h"
#include "yangavutil/video/YangYuvConvert.h"
#include "yangavutil/video/YangYuvUtil.h"
#include "yangavutil/video/YangBmp.h"
#include <yangutil/yang_unistd.h>

YangRecordCapture::YangRecordCapture(YangContext* pcontext) {
	//m_audio = paudio;
	//m_video = pvideo;
	m_context=pcontext;
	m_audioCapture = NULL;
	m_videoCapture = NULL;
	m_screenCapture = NULL;

	m_out_audioBuffer = NULL;
	m_out_videoBuffer = NULL;
	m_pre_videoBuffer = new YangVideoBuffer(m_context->video.width, m_context->video.height, m_context->video.videoEncoderFormat,
			m_context->video.bitDepth == 8 ? 1 : 2);
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
	//yang_delete(m_aecPlay_al);
	//yang_usleep(5000);

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
		if (m_context->audio.usingMono)
			m_out_audioBuffer = new YangAudioBuffer(m_context->audio.audioCacheNum);
		else
			m_out_audioBuffer = new YangAudioBuffer(m_context->audio.audioCacheNum);
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
		m_out_videoBuffer = new YangVideoBuffer(m_context->video.width, m_context->video.height,
				m_context->video.videoEncoderFormat, m_context->video.bitDepth == 8 ? 1 : 2);
	if (m_videoCapture == NULL) {
		m_videoCapture = m_capture.createVideoCapture(&m_context->video); //new YangVideoCapture(m_context);
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
void YangRecordCapture::setAec(YangAecBase *paec) {
	if (m_audioCapture) {
		m_audioCapture->setAec(paec);
		//	if(m_aecPlay_al==NULL){
		//		m_aecPlay_al=new YangAudioBuffer(m_audio->usingMono?640:4096,m_audio->audioCacheNum);
		//		if(paec!=NULL){
		//			m_ac->setAec(paec);
		//			m_ac->setPlayAudoBuffer(m_aecPlay_al);
		//			paec->setPlayBuffer(m_aecPlay_al);
		//		}
		//		}
	}
}
void YangRecordCapture::startVr(char *pbg) {
	m_bgFileName = string(pbg);
	if (m_vr_videoBuffer == NULL)
		m_vr_videoBuffer = new YangVideoBuffer(m_context->video.width,m_context->video.height,
				m_context->video.videoEncoderFormat, m_context->video.bitDepth == 8 ? 1 : 2);
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
		m_screen_videoBuffer = new YangVideoBuffer(m_context->video.width,
				m_context->video.height, m_context->video.videoEncoderFormat, m_context->video.bitDepth == 8 ? 1 : 2);
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
	/**
    printf("\n***************start vr...................");
	m_isConvert = 1;
	int32_t inWidth = m_video->width;
	int32_t inHeight = m_video->height;
	//int32_t is12bits = m_video->videoCaptureFormat > 0 ? 1 : 0;

	//int64_t videoTimestamp = 0,
	int64_t		prestamp = 0;
	//int64_t videoTimestamp1 = 0;
	long az = inWidth * inHeight * 2;
    uint8_t *srcData=new uint8_t[az];// { 0 };
	//if (is12bits)
	az = inWidth * inHeight * 3 / 2;

	YangMatImageCv *mat = new YangMatImageCv();		//mf.getYangMatImage();

    uint8_t *matDst=new uint8_t[m_video->width * m_video->height * 2];
    uint8_t *matSrcRgb=new uint8_t [m_video->width * m_video->height * 3];
    uint8_t *matSrcBgr=new uint8_t [m_video->width * m_video->height * 3];
	YangYuvConvert yuv;

	if (mat)
		mat->initImg((char*) m_bgFileName.c_str(), m_video->width,m_video->height, 3);
	//yang_rgbtobgr(mat->m_bkData, matSrcBgr, inWidth, inHeight);
	//yuv.rgb24toI420(matSrcBgr, matDst, m_video->width, m_video->height);
	//YangBmp bmp;
	//bmp.create_bmpheader(inWidth,inHeight);
	//yang_rgbtobgr(mat->m_bkData,matSrcBgr,inWidth,inHeight);
	//bmp.save_bmp("/home/yang/test.bmp",(char*)matSrcBgr,inHeight*inWidth*3);

	//	printf("\n***********************vr capture is starting...***********************\n");
	 *
	YangFrame videoFrame;
memset(&videoFrame,0,sizeof(YangFrame));
	while (m_isConvert == 1) {
		if (mat) {
			if (m_vr_videoBuffer->size() == 0) {
				yang_usleep(1000);
				continue;
			}
			videoFrame.payload=srcData;
			videoFrame.nb=az;
			m_vr_videoBuffer->getVideo(&videoFrame);
			yuv.I420torgb24(srcData, matSrcRgb, inWidth, inHeight);
			yang_rgbtobgr(matSrcRgb, matSrcBgr, inWidth, inHeight);
            mat->matImage(matSrcBgr, matDst);

		}
		if (videoFrame.timestamp - prestamp <= 0) {
			prestamp = videoFrame.timestamp;
			continue;
		}

		prestamp = videoFrame.timestamp;
		videoFrame.payload=matDst;
		videoFrame.nb=az;
        if (m_videoCapture->getVideoCaptureState())		m_out_videoBuffer->putVideo(&videoFrame);
        m_pre_videoBuffer->putVideo(&videoFrame);
	}
	//if (videoTimestamp - prestamp <= 0)		continue;
	//prestamp = videoTimestamp;

    yang_delete(mat);
    yang_deleteA(srcData);
    yang_deleteA(matDst);
    yang_deleteA(matSrcRgb);
    yang_deleteA(matSrcBgr);
    **/
}
void YangRecordCapture::startScreenLoop() {
    /**

	m_isConvert = 1;
	int32_t inWidth = m_video->width;
	int32_t inHeight = m_video->height;
	//int32_t is12bits = m_video->videoCaptureFormat > 0 ? 1 : 0;

	int64_t videoTimestamp = 0, prestamp = 0;
	//int64_t videoTimestamp1 = 0;
	long az = inWidth * inHeight * 2;
	uint8_t srcData[az] = { 0 };
	//if (is12bits)
	az = inWidth * inHeight * 3 / 2;

	YangMatImageCv *mat = new YangMatImageCv();	//mf.getYangMatImage();
	uint8_t matDst[m_video->width * m_video->height * 2];
	YangYuvConvert yuv;

	while (m_isConvert == 1) {

		if (m_screen_videoBuffer->size() == 0) {
			yang_usleep(1000);
			continue;
		}
		m_screen_videoBuffer->getVideo(srcData, az, &videoTimestamp);

		if (videoTimestamp - prestamp <= 0) {
			prestamp = videoTimestamp;
			continue;
		}

		prestamp = videoTimestamp;

		if (m_videoCapture->getVideoCaptureState())
			m_out_videoBuffer->putVideo(matDst, az, videoTimestamp);
		m_pre_videoBuffer->putVideo(matDst, az, videoTimestamp);
	}
	//if (videoTimestamp - prestamp <= 0)		continue;
	//prestamp = videoTimestamp;
	yang_delete(mat);
    **/
}

