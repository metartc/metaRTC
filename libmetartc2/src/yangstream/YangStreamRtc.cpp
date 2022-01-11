#include "YangStreamRtc.h"

#include <string.h>
#include <yangutil/yangavinfotype.h>
#include "yangutil/yang_unistd.h"
YangStreamRtc::YangStreamRtc(int32_t puid,YangContext* pcontext) :YangStreamHandle(puid,pcontext) {
	m_rtc = YangRtcHandle::createRtcHandle(pcontext);


}

YangStreamRtc::~YangStreamRtc() {
	yang_delete(m_rtc);

}
int32_t YangStreamRtc::isconnected(){
	if(m_rtc==NULL) return 0;
		return m_rtc->getState();
}
int32_t YangStreamRtc::getConnectState(){

	return m_rtc->getState()?Yang_Ok:ERROR_SOCKET;
}
int32_t YangStreamRtc::disConnectMediaServer(){

	return m_rtc->disconnectServer();
}
int32_t YangStreamRtc::connectMediaServer() {
	if(isconnected()) return Yang_Ok;

	m_rtc->setReceiveCallback(m_data);
	m_rtc->init(&m_conf);

	return reconnect();
}



int32_t YangStreamRtc::publishAudioData(YangStreamCapture *audioFrame) {
	if(m_rtc) return m_rtc->publishAudio(audioFrame);
	return Yang_Ok;
}

int32_t YangStreamRtc::publishVideoData(YangStreamCapture* videoFrame) {
	if(m_rtc) return m_rtc->publishVideo(videoFrame);
	return Yang_Ok;
}

int32_t YangStreamRtc::reconnect() {

	return m_rtc->connectRtcServer();
}


int32_t YangStreamRtc::receiveData(int32_t *plen) {


	return Yang_Ok;
}
