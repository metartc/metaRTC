
#include <stdio.h>
#include <memory.h>
#include <yangstream/YangVideoStreamCapture.h>
#include <yangutil/sys/YangEndian.h>
YangVideoStreamCapture::YangVideoStreamCapture() {

	m_videoBufs = NULL;
	m_videoBuffer = NULL;
	videoTimestamp = 0, baseTimestamp = 0;
	curVideotimestamp = 0;
	preTimestamp = 0;

	m_src=NULL;

	m_transType=0;
	m_videoLen=0;
	m_frametype=1;
	m_metaTime=0;

	m_spsppsConf=NULL;

}

YangVideoStreamCapture::~YangVideoStreamCapture() {
	m_videoBuffer = NULL;
	yang_deleteA(m_videoBufs);
	if(m_spsppsConf){
		yang_deleteA(m_spsppsConf->sps);
		yang_deleteA(m_spsppsConf->pps);
		yang_delete(m_spsppsConf);
	}



}

void YangVideoStreamCapture::init(int32_t transtype) {
	m_transType=transtype;
	if(m_transType!=Yang_Webrtc){
        if(m_videoBufs==NULL) m_videoBufs = new uint8_t[1024 * 1024];
		m_videoBuffer = m_videoBufs + 20;
		m_videoBuffer[1] = 0x01;
		m_videoBuffer[2] = 0x00;
		m_videoBuffer[3] = 0x00;
		m_videoBuffer[4] = 0x00;
	}
}
void YangVideoStreamCapture::initEncType(YangVideoEncDecType videoType,
		int32_t frametype) {
	if (frametype == 1) {
		m_videoBuffer[0] = (videoType == Yang_VED_265 ? 0x1c : 0x17);

	} else {
		m_videoBuffer[0] = (videoType == Yang_VED_265 ? 0x2c : 0x27);
	}
	m_videoBuffer[1] = 0x01;
	m_videoBuffer[2] = 0x00;
	m_videoBuffer[3] = 0x00;
	m_videoBuffer[4] = 0x00;
}
void YangVideoStreamCapture::initSpspps(uint8_t *buf){

	if(m_spsppsConf==NULL){
		m_spsppsConf=new YangSpsppsConf();
		int32_t spsLen = *(buf + 12) + 1;
		uint8_t *sps = buf + 13;
		int32_t ppsLen = *(sps + spsLen + 1) + 1;
		uint8_t *pps = buf + 13 + spsLen + 2;
		//spsLen++;
		//ppsLen++;
		m_spsppsConf->sps=new uint8_t[spsLen];
		m_spsppsConf->pps=new uint8_t[ppsLen];
		m_spsppsConf->spsLen=spsLen;
		m_spsppsConf->ppsLen=ppsLen;
		memcpy(m_spsppsConf->sps,sps,spsLen);
		memcpy(m_spsppsConf->pps,pps,ppsLen);

	}

}
void YangVideoStreamCapture::setVideoData(YangFrame* videoFrame,	YangVideoEncDecType videoType) {
	if(m_transType==Yang_Webrtc){

			m_src=videoFrame->payload;
			m_videoLen = videoFrame->nb;

	}else{
		initEncType(videoType, videoFrame->frametype);
		yang_put_be32((char*) m_videoBuffer + 5, videoFrame->nb);
		memcpy(m_videoBuffer + 9, videoFrame->payload, videoFrame->nb);
		m_videoLen= videoFrame->nb + 9;

	}

	if (preTimestamp == 0) {
		baseTimestamp = videoTimestamp;
		curVideotimestamp = 0;

	} else {
		if (videoTimestamp <= preTimestamp)
			return;

		curVideotimestamp = videoTimestamp - baseTimestamp;
	}
	preTimestamp = videoTimestamp;
	m_frametype=videoFrame->frametype;
	//return m_videoBuffer;
}
void YangVideoStreamCapture::setVideoMeta(uint8_t *p, int32_t plen, YangVideoEncDecType videoType) {

	if(m_transType==Yang_Webrtc){
		m_src=p;
		m_videoLen= plen;
		initSpspps(p);
	}else{
		memcpy(m_videoBuffer, p, plen);
		m_videoLen= plen;
	}
}
void YangVideoStreamCapture::setMetaTimestamp(int64_t timestamp){
	m_metaTime=timestamp;
}
void YangVideoStreamCapture::setFrametype(int32_t frametype){
	 m_frametype=frametype;
}

YangSpsppsConf* YangVideoStreamCapture::getSpsppsConf(){
	return m_spsppsConf;
}

int32_t YangVideoStreamCapture::getFrametype(){
	return m_frametype;
}
int32_t YangVideoStreamCapture::getVideoLen(){
	return m_videoLen;
}

int64_t YangVideoStreamCapture::getRtmpTimestamp() {
	return curVideotimestamp / 1000;
}
int64_t YangVideoStreamCapture::getTimestamp(){
	if(m_transType==0){
		return curVideotimestamp / 1000;
	}
		return curVideotimestamp * 9 / 100;
}
int64_t YangVideoStreamCapture::getMetaTimestamp(){
	return m_metaTime;
}
int64_t YangVideoStreamCapture::getTsTimestamp() {
	return curVideotimestamp * 9 / 100;
}
uint8_t* YangVideoStreamCapture::getVideoData() {

	return m_transType==Yang_Webrtc?m_src:m_videoBuffer;
}
