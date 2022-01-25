/*
 * YangEncoderBase.cpp
 *
 *  Created on: 2019年10月11日
 *      Author: yang
 */
#include <yangrecord/YangRecEncoder.h>
#include <yangavutil/video/YangVideoEncoderMeta.h>
//include <yangencoder/YangVideoEncoderMeta.h>


#include "pthread.h"
#include <iostream>
#include <yangutil/yang_unistd.h>
#include <yangencoder/YangEncoderFactory.h>


YangRecEncoder::YangRecEncoder(YangAudioInfo *paudio, YangVideoInfo *pvideo,
		YangVideoEncInfo *penc) {
	m_ve=NULL;
	m_ae=NULL;
	m_audio = paudio;
	m_video = pvideo;
	m_enc = penc;
	m_vmd = NULL;
	m_out_videoBuffer = NULL;
	m_out_auidoBuffer = NULL;

}

YangRecEncoder::~YangRecEncoder() {
	if(m_ae) m_ae->stop();
	if(m_ve) m_ve->stop();
	if(m_ae){
		while(m_ae->m_isStart) yang_usleep(500);
	}
	if(m_ve){
		while(m_ve->m_isStart) yang_usleep(500);
	}
	yang_delete(m_ae);
	yang_delete(m_ve);
	m_audio = NULL;
	m_video = NULL;
	m_enc = NULL;
	yang_free(m_vmd);
	yang_delete(m_out_videoBuffer); //=NULL;
	yang_delete(m_out_auidoBuffer); //=NULL;
}
YangVideoMeta* YangRecEncoder::getOutVideoMetaData() {
	return m_vmd;
}

void YangRecEncoder::initAudioEncoder() {
	if (m_out_auidoBuffer == NULL)
		m_out_auidoBuffer = new YangAudioEncoderBuffer(m_audio->audioCacheNum);
	if (m_ae == NULL) {
		//	YangEncoderFactory yf;
		m_ae = new YangAudioEncoderHandle(m_audio);
		m_ae->setOutAudioBuffer(m_out_auidoBuffer);
		m_ae->init();
	}

}
void YangRecEncoder::initVideoEncoder() {
	if (m_out_videoBuffer == NULL)
		m_out_videoBuffer = new YangVideoEncoderBuffer(m_video->videoCacheNum);
	if (m_vmd == NULL)
		m_vmd = (YangVideoMeta*) calloc(1, sizeof(YangVideoMeta));
	YangEncoderFactory fac;
	YangVideoEncoderMeta *yvh = fac.createVideoEncoderMeta(m_video);
	yvh->yang_initVmd(m_vmd, m_video, m_enc);
	yang_delete(yvh);
	if (m_ve == NULL) {
		//	YangEncoderFactory yf;
		m_ve = new YangVideoEncoderHandle(m_video, m_enc);
		m_ve->setOutVideoBuffer(m_out_videoBuffer);
		m_ve->init();
		m_ve->setVideoMetaData(m_vmd);
	}
}
void YangRecEncoder::startAudioEncoder() {
	if (m_ae && !m_ae->m_isStart) {
		m_ae->start();
		yang_usleep(1000);
	}
}
void YangRecEncoder::startVideoEncoder() {
	if (m_ve && !m_ve->m_isStart) {
		m_ve->start();
		yang_usleep(2000);
	}
}
void YangRecEncoder::setInAudioBuffer(YangAudioBuffer *pbuf) {
	if (m_ae != NULL)
		m_ae->setInAudioBuffer(pbuf);
}
void YangRecEncoder::setInVideoBuffer(YangVideoBuffer *pbuf) {
	//printf("\n.......................%u\n",m_ve);
	if (m_ve != NULL)
		m_ve->setInVideoBuffer(pbuf);
}
YangAudioEncoderBuffer* YangRecEncoder::getOutAudioBuffer() {
	return m_out_auidoBuffer;
}
YangVideoEncoderBuffer* YangRecEncoder::getOutVideoBuffer() {
	return m_out_videoBuffer;
}

