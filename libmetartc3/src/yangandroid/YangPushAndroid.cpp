//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangandroid/YangPushAndroid.h>
#include <yangutil/sys/YangSocket.h>
#include <yangpush/YangPushHandleImpl.h>
YangPushAndroid::YangPushAndroid(void* pwindow) {
	m_context=new YangContext();
	m_context->init();

	m_context->streams.m_playBuffer=new YangSynBuffer();

	initContext();
	m_context->nativeWindow=pwindow;
	m_push = NULL;
	yang_setLogLevle(5);
	yang_setLogFile(1);

	m_localPort=16000;
	m_isStartplay=false;



	memset(&m_videoFrame,0,sizeof(YangFrame));
	memset(&m_audioFrame,0,sizeof(YangFrame));

}

YangPushAndroid::~YangPushAndroid() {
	yang_delete(m_push);
	yang_closeLogFile();
	yang_delete(m_context);
}

void YangPushAndroid::initContext() {
	m_context->avinfo.rtc.mixAvqueue=0;
	m_context->avinfo.sys.mediaServer=Yang_Server_Srs;//Yang_Server_Srs/Yang_Server_Zlm
	m_context->avinfo.video.videoEncoderType=Yang_VED_264;
	m_context->avinfo.video.videoEncoderFormat=YangI420;

    m_context->avinfo.audio.usingMono=0;
    m_context->avinfo.audio.sample=48000;
    m_context->avinfo.audio.channel=2;
    m_context->avinfo.audio.hasAec=0;
    m_context->avinfo.audio.audioCacheNum=8;
    m_context->avinfo.audio.audioCacheSize=8;
    m_context->avinfo.audio.audioPlayCacheNum=8;

    m_context->avinfo.video.videoCacheNum=10;
    m_context->avinfo.video.evideoCacheNum=10;
    m_context->avinfo.video.videoPlayCacheNum=10;

    m_context->avinfo.audio.audioEncoderType=Yang_AED_OPUS;
    m_context->avinfo.sys.rtcLocalPort=17000;
    m_context->avinfo.enc.enc_threads=4;
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
	//int32_t err=Yang_Ok;
	char ip[256]={0};
	yang_getLocalInfo(ip);
	return m_push->publish(url,ip,m_context->avinfo.sys.rtcLocalPort);


}
int32_t YangPushAndroid::stopPush(){
	int32_t err=Yang_Ok;
	if(m_push)  m_push->disconnect();
	return err;
}


