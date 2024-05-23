//
// Copyright (c) 2019-2023 yanggaofeng
//

#include "YangMetaplayer.h"
#include <yangutil/sys/YangSocket.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangMath.h>
#include "YangLvglDef.h"
YangMetaplayer::YangMetaplayer()

{

	m_context=new YangContext();
	m_context->init();

	m_context->synMgr.session->playBuffer=(YangSynBuffer*)yang_calloc(sizeof(YangSynBuffer),1);
	yang_create_synBuffer(m_context->synMgr.session->playBuffer);

	m_context->avinfo.sys.mediaServer=Yang_Server_Srs;//Yang_Server_Srs/Yang_Server_Zlm
	m_context->avinfo.rtc.rtcSocketProtocol=Yang_Socket_Protocol_Udp;//

	m_context->avinfo.rtc.rtcLocalPort=10000+yang_random()%15000;
	memset(m_context->avinfo.rtc.localIp,0,sizeof(m_context->avinfo.rtc.localIp));
	yang_getLocalInfo(m_context->avinfo.sys.familyType,m_context->avinfo.rtc.localIp);
	m_player= YangPlayerHandle::createPlayerHandle(m_context,this);
	yang_setLogLevel(5);
	yang_setLogFile(1);

	char s[128]={0};

	sprintf(s,"webrtc://%s:1985/live/livestream",m_context->avinfo.rtc.localIp);
	yang_trace("\nurl===%s",s);
	yang_lvgl_setUrlText(s);

	m_isStartplay=false;

	m_context->avinfo.rtc.iceCandidateType=YangIceHost;
	m_context->avinfo.rtc.turnSocketProtocol=Yang_Socket_Protocol_Udp;

	m_context->avinfo.rtc.enableAudioBuffer=yangtrue; //use audio buffer
	m_context->avinfo.audio.enableAudioFec=yangfalse; //srs not use audio fec

}

YangMetaplayer::~YangMetaplayer()
{

	m_videoThread->m_syn=NULL;

	yang_delete(m_player);
	yang_closeLogFile();
	yang_delete(m_context);

}


void YangMetaplayer::success(){

}
void YangMetaplayer::failure(int32_t errcode){



}

void YangMetaplayer::initVideoThread(YangRecordThread *prt){
	m_videoThread=prt;
	m_videoThread->initPara();
	m_videoThread->m_syn= m_context->synMgr.session->playBuffer;

}

void YangMetaplayer::on_m_b_play_clicked()
{
	if(!m_isStartplay){
		m_videoThread->m_syn=m_context->synMgr.session->playBuffer;
		m_videoThread->m_syn->resetVideoClock(m_videoThread->m_syn->session);
		int32_t err=Yang_Ok;
		char* url=yang_lvgl_get_urlstr();

		err=m_player->play(url);

		if(err==Yang_Ok){
			yang_lvgl_setPlayButtonStr("stop");
			m_isStartplay=!m_isStartplay;
			m_videoThread->m_isRender=true;
		}else{
			yang_error("play url error!");
			m_videoThread->m_isRender=false;
		}

	}else{
		m_videoThread->m_isRender=false;

		yang_lvgl_setPlayButtonStr("play");
		yang_usleep(50*1000);
		if(m_player) m_player->stopPlay();

		m_isStartplay=!m_isStartplay;

	}
}






