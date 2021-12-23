
#include "YangStreamSrt.h"
#include <yangsrt/YangTsdemux.h>

#include "yangutil/sys/YangLog.h"
#define YangSrtUnitSize 1316
YangStreamSrt::YangStreamSrt(int32_t puid,YangContext* pcontext):YangStreamHandle(puid,pcontext) {
	m_bufLen = 0;
	m_buffer = NULL;
	m_bufReceiveLen = 0, m_bufRemainLen = 0;
	m_srt = NULL;
	memset(&m_audioFrame,0,sizeof(YangFrame));
	memset(&m_videoFrame,0,sizeof(YangFrame));
}

YangStreamSrt::~YangStreamSrt() {
	if (m_buffer)
		delete[] m_buffer;
	m_buffer = NULL;
}

int32_t YangStreamSrt::isconnected(){
	if(m_srt)
		return m_srt->getSrtSocketStatus()==SRTS_CONNECTED;
	else
		return 0;
}


int32_t YangStreamSrt::getConnectState(){
	int32_t ret=m_srt->getSrtSocketStatus();
	if(ret==SRTS_CONNECTED)
		return Yang_Ok;
	else
		return Yang_SRTS_SocketBase+ret;
	//return ERROR_SOCKET;
}
int32_t YangStreamSrt::reconnect() {
	m_bufLen=0;
	char url[1024]={0};
	sprintf(url,"%s.sls.com/%s/%s",m_conf.streamOptType?"uplive":"live",m_conf.app.c_str(),m_conf.stream.c_str());
	int32_t ret=m_srt->initConnect(url);
			if(ret) return ret;
	return m_srt->connectServer();

}



int32_t YangStreamSrt::connectMediaServer(){
	if(isconnected()) return Yang_Ok;
	if(!m_buffer) m_buffer=new char[Yang_Srt_CacheSize];
	if(m_srt==NULL) m_srt=new YangSrtBase();
	if(!m_srt->m_contextt){
		m_bufLen=0;
		char s[512]={0};
		sprintf(s,"%s.sls.com/%s/%s",m_conf.streamOptType==Yang_Stream_Publish?"uplive":"live",m_conf.app.c_str(),m_conf.stream.c_str());
		m_conf.url=s;
		m_srt->init((char*)m_conf.serverIp.c_str(),m_conf.serverPort);
	}
	int32_t ret=m_srt->initConnect((char*)m_conf.url.c_str());
	if(ret) return ret;
	return m_srt->connectServer();

}

int32_t YangStreamSrt::disConnectMediaServer(){
	m_srt->closeSrt();
	m_netState=1;
	return Yang_Ok;
}


int32_t YangStreamSrt::publishAudioData(YangStreamCapture* audioFrame) {
	vector<YangTsBuffer> sb ;
	m_ts.encode(audioFrame->getAudioData(), audioFrame->getAudioLen(), 1, audioFrame->getAudioTimestamp(), audioFrame->getAudioType() == 0 ? TS_AAC : TS_OPUS, &sb);
	int32_t ret=Yang_Ok;
	for (size_t i = 0; i < sb.size(); i++) {
		memcpy(m_buffer+m_bufLen,sb.at(i).data(),188);
		m_bufLen+=188;
		if(m_bufLen==YangSrtUnitSize){
			ret=m_srt->publish((char*) m_buffer, YangSrtUnitSize);
			m_bufLen=0;
		}

		if (ret){
			yang_error("publish audio error..%d",ret);
			m_netState=ret;
			return ret;
		}

	}
	return Yang_Ok;
}


int32_t YangStreamSrt::publishVideoData(YangStreamCapture* videoFrame) {
	int32_t ret=Yang_Ok;
	vector<YangTsBuffer> sb;
	m_ts.encode(videoFrame->getVideoData(), videoFrame->getVideoLen(), videoFrame->getVideoFrametype(), videoFrame->getVideoTimestamp(), TS_H264, &sb);
	for (size_t i = 0; i < sb.size(); i++) {
		memcpy(m_buffer+m_bufLen,sb.at(i).data(),188);
		m_bufLen+=188;
		if(m_bufLen==YangSrtUnitSize){
			ret=m_srt->publish((char*) m_buffer, YangSrtUnitSize);
			m_bufLen=0;
		}
		if (ret)	{
			return ret;
			m_netState=ret;
		}
	}
	return Yang_Ok;
}


void YangStreamSrt::on_data_callback(SRT_DATA_MSG_PTR data_ptr,
		uint32_t  media_type, uint64_t dts, uint64_t pts) {
	uint8_t *temp = data_ptr->get_data();
	int32_t len = data_ptr->data_len();

	if ((media_type == Yang_H264_PID || media_type == Yang_H265_PID)
			&& m_videoStream) {
		//int64_t t_timestamp = dts;
		m_videoFrame.timestamp=dts;
		m_videoFrame.uid=m_uid;
		m_videoFrame.payload=temp;
		m_videoFrame.nb=len;
		if (m_data)			m_data->receiveVideo(&m_videoFrame);
	} else if (media_type == Yang_AAC_PID && m_audioStream) {
		m_audioFrame.uid=m_uid;
		m_audioFrame.nb=len;
		m_audioFrame.payload=temp;
		if (m_data)			m_data->receiveAudio(&m_audioFrame);
	} else if (media_type == Yang_OPUS_PID && m_audioStream) {
		m_audioFrame.uid=m_uid;
		m_audioFrame.nb=len;
		m_audioFrame.payload=temp;
		if (m_data)			m_data->receiveAudio(&m_audioFrame);
	}
	temp = NULL;
}

int32_t YangStreamSrt::receiveData(int32_t *plen) {

	if (!m_srt)
		return Yang_Ok;
	m_bufReceiveLen = 0;
	if (m_srt->receive(m_buffer + m_bufRemainLen, &m_bufReceiveLen))
		return ERROR_SRT_PushFailure;
	m_bufLen = m_bufReceiveLen + m_bufRemainLen;
	if (m_bufLen < YangSrtUnitLen) {
		m_bufRemainLen = m_bufLen;
		return Yang_Ok;
	}
	m_bufRemainLen = m_bufLen % YangSrtUnitLen;
	*plen = m_bufLen;
	auto input_ptr = std::make_shared<SRT_DATA_MSG>((uint8_t*) m_buffer,
			m_bufLen - m_bufRemainLen);
	demux.decode(input_ptr, this);
	if (m_bufRemainLen > 0)
		memcpy(m_buffer, m_buffer + (m_bufLen - m_bufRemainLen),
				m_bufRemainLen);
	return Yang_Ok;
}


int32_t YangStreamSrt::sendPmt(){
	vector<YangTsBuffer> sb;
	m_ts.encodePmtWithoutData(&sb);
	int32_t ret=Yang_Ok;
	for (size_t i = 0; i < sb.size(); i++) {
		ret=m_srt->publish((char*) sb.at(i).data(), 188);
		if (ret){
			yang_error("publish audio error..%d",i);
			return ret;
		}

	}
	sb.clear();
	return Yang_Ok;
}
