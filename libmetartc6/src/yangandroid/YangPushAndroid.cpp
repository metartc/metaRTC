//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangandroid/YangPushAndroid.h>

#include <yangpush/YangPushHandleImpl.h>
YangPushAndroid::YangPushAndroid(YangContext* pcontext,void* pwindow) {
	m_context=pcontext;
	//m_context->streams.m_playBuffer=new YangSynBuffer();

	m_context->nativeWindow=pwindow;
	m_push = NULL;

	m_localPort=16000;
	m_isStartplay=false;

	memset(&m_videoFrame,0,sizeof(YangFrame));
	memset(&m_audioFrame,0,sizeof(YangFrame));

}

YangPushAndroid::~YangPushAndroid() {
	yang_delete(m_push);
	yang_closeLogFile();

}


void YangPushAndroid::startCamera(){
	m_push = new YangPushHandleImpl(1,false,Yang_VideoSrc_Camera,NULL,NULL,m_context,this);
	m_push->changeSrc(Yang_VideoSrc_Camera,false);
}

void YangPushAndroid::success(){

}
void YangPushAndroid::failure(int32_t errcode){

}
int32_t YangPushAndroid::startPush(std::string url){

	return m_push->publish((char*)url.c_str());
}
int32_t YangPushAndroid::stopPush(){
	int32_t err=Yang_Ok;
	if(m_push)  m_push->disconnect();
	return err;
}


