
#include <yangencoder/YangVideoEncoderHandle.h>
#include <yangencoder/YangEncoderFactory.h>

#include <yangutil/yang_unistd.h>
#include <yangavutil/video/YangYuvConvert.h>
#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangLog.h>

#include <stdlib.h>
#include <malloc.h>



YangVideoEncoderHandle::YangVideoEncoderHandle(YangVideoInfo *pcontext,
		YangVideoEncInfo *enc) {
	m_isInit = 0;
	m_isStart = 0;
	m_isConvert = 1;
	m_in_videoBuffer = NULL;
	m_out_videoBuffer = NULL;
	m_para = pcontext;
	m_enc = enc;
	m_vmd = NULL;
	m_sendKeyframe=0;
	m_uid=0;
}

YangVideoEncoderHandle::~YangVideoEncoderHandle(void) {
	if (m_isConvert) {
		stop();
		while (m_isStart) {
			yang_usleep(1000);
		}
	}
	m_para = NULL;
	m_enc = NULL;
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
void YangVideoEncoderHandle::sendKeyframe(){
	m_sendKeyframe=1;
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
           // yang_trace("\n%d:",pframe->nb);
            //for(int i=0;i<50;i++) yang_trace("%02x,",pframe->payload[i]);
			m_out_videoBuffer->putEVideo(pframe);
	}
}
void YangVideoEncoderHandle::onAudioData(YangFrame* pframe){

}
void YangVideoEncoderHandle::startLoop() {
	m_isConvert = 1;
	YangVideoInfo para;
	YangYuvConvert yuv;
	memcpy(&para, m_para, sizeof(YangVideoInfo));
	//int32_t is12bit = yvp.videoCaptureFormat > 0 ? 1 : 0;
	int32_t isTrans = (para.width != para.outWidth ? 1 : 0);
	int32_t isHw=m_para->videoEncHwType;
	//int64_t videoTimestamp = 0;
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
	YangVideoEncoder *t_Encoder = ydf.createVideoEncoder(m_para);

	t_Encoder->init(&para, m_enc);
	t_Encoder->setVideoMetaData(m_vmd);

	uint8_t* nv12Src=NULL;
	//uint8_t* tmpNv12=NULL;
	if(isHw) nv12Src=new uint8_t[m_in_fileSize];
	int64_t t_preTimestamp=0;
	int32_t wid=m_para->width;
	int32_t hei=m_para->height;
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

			if(tmpsrc){
			//tmpNv12=m_in_videoBuffer->getVideoIn(&videoTimestamp);
				if(para.videoEncoderFormat==YangI420) {
					yuv.i420tonv12(tmpsrc,nv12Src,wid,hei);
					tmp=nv12Src;
				}
				if(para.videoEncoderFormat==YangArgb){
					tmp=tmpsrc;
				}
			}

		}else{
			tmp=tmpsrc;
			//tmp=m_in_videoBuffer->getVideoIn(&videoTimestamp);
		}


		if(t_preTimestamp){
				if(videoFrame.timestamp<=t_preTimestamp){
					tmp=NULL;
					continue;
				}
			}
		t_preTimestamp=videoFrame.timestamp;
		if(m_sendKeyframe==1){
			t_Encoder->sendKeyFrame();
			m_sendKeyframe=0;
		}
		//videoFrame.timestamp=frame->timestamp;
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
	//tmpNv12=NULL;
	yang_deleteA(outVideoSrc);
	yang_deleteA(nv12Src);
	yang_delete(t_Encoder);


}

void YangVideoEncoderHandle::stopLoop() {
	m_isConvert = 0;

}
