//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangencoder/YangVideoEncoderHandle.h>
#include <yangencoder/YangEncoderFactory.h>

#include <yangutil/yang_unistd.h>
#include <yangavutil/video/YangYuvConvert.h>
#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangLog.h>

#include <stdlib.h>
#include <malloc.h>



YangVideoEncoderHandle::YangVideoEncoderHandle(YangContext* pcontext,YangVideoInfo* pvideoInfo) {
	m_context=pcontext;
	m_isInit = 0;
	m_isStart = 0;
	m_isConvert = 1;
	m_in_videoBuffer = NULL;
	m_out_videoBuffer = NULL;
	m_videoInfo = pvideoInfo;
	m_encInfo = &pcontext->avinfo.enc;
	m_vmd = NULL;
	m_sendMsgToEncoder=0;
	m_uid=0;
}

YangVideoEncoderHandle::~YangVideoEncoderHandle(void) {
	if (m_isConvert) {
		stop();
		while (m_isStart) {
			yang_usleep(1000);
		}
	}
	m_videoInfo = NULL;
	m_encInfo = NULL;
	m_in_videoBuffer = NULL;
	m_out_videoBuffer = NULL;
	m_vmd = NULL;

}
void YangVideoEncoderHandle::stop() {
	stopLoop();
}

void YangVideoEncoderHandle::run() {
	m_isStart = 1;
	startLoop();
	m_isStart = 0;
}
void YangVideoEncoderHandle::sendMsgToEncoder(YangRequestType request){
	m_sendMsgToEncoder=1;
	m_request.request=request;
	m_request.requestValue=0;
}
void YangVideoEncoderHandle::setVideoMetaData(YangVideoMeta *pvmd) {
	m_vmd = pvmd;
}

void YangVideoEncoderHandle::init() {
	m_isInit = 1;

}

void YangVideoEncoderHandle::setInVideoBuffer(YangVideoBuffer *plist) {
	m_in_videoBuffer = plist;
}
void YangVideoEncoderHandle::setOutVideoBuffer(YangVideoEncoderBuffer *plist) {
	m_out_videoBuffer = plist;
}
void YangVideoEncoderHandle::onVideoData(YangFrame* pframe){

	if (pframe->nb > 4) {
		m_out_videoBuffer->putEVideo(pframe);
	}
}
void YangVideoEncoderHandle::onAudioData(YangFrame* pframe){

}
void YangVideoEncoderHandle::startLoop() {
	m_isConvert = 1;
	YangVideoInfo para;
	YangYuvConvert yuv;
	memcpy(&para, m_videoInfo, sizeof(YangVideoInfo));

	int32_t isTrans = (para.width != para.outWidth ? 1 : 0);
	int32_t isHw=m_videoInfo->videoEncHwType;

	int32_t bitLen = para.bitDepth == 8 ? 1 : 2;
	int32_t m_in_fileSize = bitLen * para.width * para.height * 3 / 2;
	int32_t m_out_fileSize = bitLen * para.outWidth * para.outHeight * 3 / 2;

	uint8_t *outVideoSrc = NULL;
	if (isTrans) {
		outVideoSrc = new uint8_t[para.outWidth * para.outHeight * 3 / 2];
	}

	if (m_in_videoBuffer != NULL)
		m_in_videoBuffer->resetIndex();
	if (m_out_videoBuffer != NULL)
		m_out_videoBuffer->resetIndex();

	YangEncoderFactory ydf;
	YangVideoEncoder *t_Encoder = ydf.createVideoEncoder(m_videoInfo);

	t_Encoder->init(m_context,m_videoInfo);
	t_Encoder->setVideoMetaData(m_vmd);

	uint8_t* nv12Src=NULL;

	if(isHw) nv12Src=new uint8_t[m_in_fileSize];
	int64_t t_preTimestamp=0;
	int32_t wid=m_videoInfo->width;
	int32_t hei=m_videoInfo->height;
	YangFrame videoFrame;
	memset(&videoFrame,0,sizeof(YangFrame));

	while (m_isConvert == 1) {
		if (m_in_videoBuffer->size() == 0) {
			yang_usleep(20000);
			continue;
		}
		uint8_t *tmp=NULL;
		uint8_t *tmpsrc=NULL;

		tmpsrc=m_in_videoBuffer->getVideoRef(&videoFrame);

		if(!tmpsrc) continue;

		if(isHw){
#ifdef __ANDROID__
			if(para.videoCaptureFormat==YangI420){
				if(para.videoEncoderFormat==YangI420) tmp=tmpsrc;
				if(para.videoEncoderFormat==YangNv12) {
					yuv.i420tonv12(tmpsrc,nv12Src,wid,hei);
					tmp=nv12Src;
				}
				//if(m_encoderVideoFormat==YangArgb) m_yuv.i420torgba(pBuffer,m_buf,m_width,m_height);
			}
#else
			if(para.videoEncoderFormat==YangI420) {
				yuv.i420tonv12(tmpsrc,nv12Src,wid,hei);
				tmp=nv12Src;
			}
			if(para.videoEncoderFormat==YangArgb){
				tmp=tmpsrc;
			}
#endif

		}else{
			tmp=tmpsrc;

		}


		if(t_preTimestamp){
			if(videoFrame.pts<=t_preTimestamp){
				tmp=NULL;
				continue;
			}
		}
		t_preTimestamp=videoFrame.pts;
		if(m_sendMsgToEncoder==1){
			t_Encoder->sendMsgToEncoder(&m_request);
			m_sendMsgToEncoder=0;
		}

		videoFrame.uid=m_uid;

		if (isTrans) {
			yuv.scaleI420(tmp,
					outVideoSrc, para.width, para.height, para.outWidth,
					para.outHeight);
			videoFrame.payload=outVideoSrc;
			videoFrame.nb=m_out_fileSize;

			t_Encoder->encode(&videoFrame,this);
		} else {
			videoFrame.payload=tmp;
			videoFrame.nb=m_in_fileSize;
			t_Encoder->encode(&videoFrame,this);

		}
		tmp=NULL;

	}

	yang_deleteA(outVideoSrc);
	yang_deleteA(nv12Src);
	yang_delete(t_Encoder);


}

void YangVideoEncoderHandle::stopLoop() {
	m_isConvert = 0;

}
