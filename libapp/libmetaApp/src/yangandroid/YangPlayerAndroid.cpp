//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangandroid/YangPlayerAndroid.h>
#include <yangutil/sys/YangLog.h>



YangPlayerAndroid::YangPlayerAndroid(YangContext* pcontext) {
	m_context=pcontext;
	m_player= YangPlayerHandle::createPlayerHandle(m_context,this);


	m_localPort=16000;
	m_isStartplay=false;

	memset(&m_videoFrame,0,sizeof(YangFrame));
	memset(&m_audioFrame,0,sizeof(YangFrame));
}

YangPlayerAndroid::~YangPlayerAndroid() {

	yang_delete(m_player);
	yang_closeLogFile();

}

void YangPlayerAndroid::success(){

}
void YangPlayerAndroid::failure(int32_t errcode){



}

int32_t YangPlayerAndroid::startPlay(std::string url)
{
	if(m_isStartplay) return Yang_Ok;

	if(m_player&&m_player->play((char*)url.c_str())==Yang_Ok){
		m_isStartplay=true;
	}else{
		m_isStartplay=false;
		return yang_error_wrap(1, "Error:play url error!");
	}
	return Yang_Ok;
}
int32_t YangPlayerAndroid::getWidth(){
	return m_context->streams.m_playBuffer->m_width;
}
int32_t YangPlayerAndroid::getHeight(){
	return m_context->streams.m_playBuffer->m_height;
}
int32_t YangPlayerAndroid::stopPlay(){
	if(!m_isStartplay) return Yang_Ok;
	if(m_player) m_player->stopPlay();
	m_isStartplay=false;
	return Yang_Ok;
}

uint8_t* YangPlayerAndroid::getVideoRef(int64_t* timestamp){
	uint8_t* ret=m_context->streams.m_playBuffer->getVideoRef(&m_videoFrame);//
	if(ret!=NULL) {
		*timestamp=m_videoFrame.pts;
		return ret;
	}
	return NULL;
}
