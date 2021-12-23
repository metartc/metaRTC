#include <stdio.h>
#include <string.h>
#include <yangstream/YangStreamHandle.h>
#include "yangutil/sys/YangLog.h"
YangStreamHandle::YangStreamHandle(int32_t puid,YangContext* pcontext) {
	m_context=pcontext;
	m_streamInit = 0;
	m_audioStream=0;
	m_videoStream=0;
	m_uid=puid;
	m_data=NULL;
	m_netState = 1;
}

YangStreamHandle::~YangStreamHandle() {
	m_data=NULL;
	m_context=NULL;

}
void YangStreamHandle::init(YangStreamConfig* pconf){
		m_conf.serverIp=pconf->serverIp;
		m_conf.localIp=pconf->localIp;
		m_conf.app=pconf->app;
		m_conf.stream=pconf->stream;
		m_conf.serverPort=pconf->serverPort;
		m_conf.localPort=pconf->localPort;
		m_conf.uid=pconf->uid;
		m_conf.streamOptType=pconf->streamOptType;
}
int32_t YangStreamHandle::connectServer(){
	if(isconnected()) return Yang_Ok;
	m_netState=connectMediaServer();
	return m_netState;
}
int32_t YangStreamHandle::disConnectServer(){
	m_netState=1;
	return disConnectMediaServer();
}
void YangStreamHandle::setReceiveCallback(YangReceiveCallback *pdata){
	m_data=pdata;
}
