
#include "YangStreamRtmp.h"
#include <string.h>
#include "yangutil/yang_unistd.h"
YangStreamRtmp::YangStreamRtmp(int32_t puid,YangContext* pcontext) :YangStreamHandle(puid,pcontext) {
	rtmp = NULL;
	rtmpRet = 0;
	memset(&pac_a, 0, sizeof(pac_a));
	memset(&pac, 0, sizeof(pac));
	m_buf = NULL;
	m_bufLen = 0;
	memset(&m_audioFrame,0,sizeof(YangFrame));
	memset(&m_videoFrame,0,sizeof(YangFrame));

}
int32_t YangStreamRtmp::isconnected(){
	if(rtmp&&rtmp->r)
		return rtmp->r->m_bConnected;
	else
		return 0;
}

YangStreamRtmp::~YangStreamRtmp() {
    pac.m_body=NULL;
    pac_a.m_body=NULL;
	yang_delete(rtmp);
    yang_deleteA(m_buf);
}
int32_t YangStreamRtmp::getConnectState(){
	if(rtmp&&rtmp->r->m_bConnected) return Yang_Ok;
	return ERROR_SOCKET;
}
int32_t YangStreamRtmp::disConnectMediaServer(){
	rtmp->RTMP_Close();
	return Yang_Ok;
}
int32_t YangStreamRtmp::connectMediaServer() {
	if(isconnected()) return Yang_Ok;
	if (rtmp == NULL)
		rtmp = new YangRtmpBase();
	if(m_conf.streamOptType){
		pac.m_packetType = RTMP_PACKET_TYPE_VIDEO;
		pac_a.m_packetType = RTMP_PACKET_TYPE_AUDIO;
	}
	rtmp->initRTMP((char*)m_conf.serverIp.c_str(),(char*) m_conf.app.c_str(), m_conf.serverPort);
	return reconnect();
}

int32_t YangStreamRtmp::publishAudioData(YangStreamCapture* audioFrame) {

	pac_a.m_nBodySize = audioFrame->getAudioLen();
	pac_a.m_headerType = RTMP_PACKET_SIZE_LARGE;
	pac_a.m_nTimeStamp = audioFrame->getAudioTimestamp();
	pac_a.m_nChannel = 0x04;
	pac_a.m_nInfoField2 = rtmp->r->m_stream_id;
	pac_a.m_body=(char*)audioFrame->getAudioData();
	rtmpRet = rtmp->sendPacket(&pac_a, 0);
	if(!rtmpRet) m_netState=ERROR_SOCKET;
	return m_netState;
}

int32_t YangStreamRtmp::publishVideoData(YangStreamCapture* videoFrame) {
	//int32_t ret = Yang_Ok;
	pac.m_nBodySize = videoFrame->getVideoLen();
	pac.m_headerType = RTMP_PACKET_SIZE_LARGE;
	pac.m_body=(char*)videoFrame->getVideoData();
	pac.m_nChannel = 0x04;
	pac.m_nTimeStamp = videoFrame->getVideoTimestamp();	//(unsigned int)videoTimestamp;
	pac.m_nInfoField2 = rtmp->r->m_stream_id;
	rtmpRet = rtmp->sendPacket(&pac, 0);
	if(!rtmpRet) m_netState=ERROR_SOCKET;
	return m_netState;
}

int32_t YangStreamRtmp::reconnect() {
	int32_t ret = rtmp->connectRtmpServer( m_conf.streamOptType, (char*)m_conf.serverIp.c_str(), (char*)m_conf.app.c_str(), m_conf.serverPort);
	if (ret)	{
		rtmp->RTMP_Close();
		return ret;
	}
	return rtmp->HandleStream((char*)m_conf.stream.c_str(),  m_conf.streamOptType, m_uid);
}


int32_t YangStreamRtmp::receiveData(int32_t *plen) {
	if(!rtmp||!rtmp->r->m_bConnected) return 1;
	if (!m_buf)
		m_buf = new char[1024 * 64];
	char *buf = m_buf + 30;
	if (!rtmp->yangReadPacket(&pac, buf)) {
		return 1;
	}
	//while (!RTMPPacket_IsReady(&pac)&&rtmp->yangReadPacket(&pac, buf)) {
	//	yang_usleep(1);
	//}
	if (RTMPPacket_IsReady(&pac)) {
		uint8_t *temp = NULL;
		if ((pac.m_packetType == RTMP_PACKET_TYPE_AUDIO
				|| pac.m_packetType == RTMP_PACKET_TYPE_VIDEO)) {
			temp = (uint8_t*) pac.m_body;
			if (temp) {

				if (pac.m_packetType == RTMP_PACKET_TYPE_AUDIO) {
					m_audioFrame.payload=temp;
					m_audioFrame.uid=m_uid;
					m_audioFrame.nb=pac.m_nBodySize;
					if(m_data) m_data->receiveAudio(&m_audioFrame);
					*plen = pac.m_nBodySize;
				} else {
					m_videoFrame.timestamp = pac.m_nTimeStamp;
					m_videoFrame.payload=temp;
					m_videoFrame.uid=m_uid;
					m_videoFrame.nb=pac.m_nBodySize;
					if(m_data) m_data->receiveVideo(&m_videoFrame);
					*plen = pac.m_nBodySize;
				}
			}

		} else {
			rtmp->clientPacket(&pac);
		}

		memset(m_buf, 0,
				pac.m_nBodySize < RTMP_MAX_BODY_SIZE ?
						pac.m_nBodySize + 36 : RTMP_MAX_BODY_SIZE);
		memset(&pac, 0, sizeof(RTMPPacket));
		pac.m_body = NULL;

	}
	return Yang_Ok;
}
