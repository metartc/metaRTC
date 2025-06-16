//
// Copyright (c) 2019-2023 yanggaofeng
//

#include "YangMetaplayer.h"
#include <yangutil/sys/YangSocket.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangMath.h>
#include "YangPlayDef.h"

extern "C"{
	#include <libyuv.h>
}
void* yang_create_player(){
	return new YangMetaplayer();
}
void yang_destroy_player(void* user){
	YangMetaplayer* player=(YangMetaplayer*)user;
	yang_delete(player);
}
int32_t yang_player_playRtc(void* user,char* url){
	if(user==NULL) return 1;
	YangMetaplayer* player=(YangMetaplayer*)user;
	player->playRtc(url);
	return 0;
}
int32_t yang_player_close(void* user){
	if(user==NULL) return 1;
	YangMetaplayer* player=(YangMetaplayer*)user;
	player->close();
	return 0;
}
uint8_t* yang_player_get_buffer(void* user,int32_t dstWidth,int32_t dstHeight){
	YangMetaplayer* player=(YangMetaplayer*)user;
	uint8_t* buffer=NULL;
	int32_t width=0;
	int32_t height=0;
	if(user==NULL) return NULL;
	buffer=player->m_context->synMgr.session->playBuffer->getVideoRef(
		player->m_context->synMgr.session->playBuffer->session,&player->m_frame);
	if(buffer==NULL) return NULL;
	width=player->m_context->synMgr.session->playBuffer->width(player->m_context->synMgr.session->playBuffer->session);
	height=player->m_context->synMgr.session->playBuffer->height(player->m_context->synMgr.session->playBuffer->session);
	if(width!=player->m_srcWidth&&height!=player->m_srcHeight){
		yang_free(player->m_rgbaBuffer);
		player->m_rgbaBuffer=(uint8_t*)malloc(width*height*4);
		player->m_srcWidth=width;
		player->m_srcHeight=height;
	}
	if(player->m_dstWidth!=dstWidth&&player->m_dstHeight!=dstHeight){
		yang_free(player->m_buffer);		
		player->m_dstWidth=dstWidth;
		player->m_dstHeight=dstHeight;
		player->m_buffer=(uint8_t*)malloc(dstWidth*dstHeight*4);	
	}
	//I420ToABGR I420ToARGB
	libyuv::I420ToABGR((const uint8_t*) buffer, width,
			(const uint8_t*) (buffer + (width * height)), (width >> 1),
			(const uint8_t*) (buffer + (width * height) + (int) (width * height / 4)),width >> 1,
			(uint8_t*)player->m_rgbaBuffer,width * 4,
			width, height);
	if(width!=dstWidth){
		libyuv::ARGBScale((const uint8_t*)player->m_rgbaBuffer,width*4,
		width,height,
		player->m_buffer,dstWidth*4,
		dstWidth,dstHeight,
		libyuv::kFilterNone
		);
		return player->m_buffer;
	}
		
	return player->m_rgbaBuffer;
	
	

}
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
	m_srcWidth=640;
	m_srcHeight=480;
	m_dstWidth=640;
	m_dstHeight=480;
	m_buffer=(uint8_t*)yang_malloc(m_dstWidth*m_dstHeight*4);
	m_rgbaBuffer=(uint8_t*)yang_malloc(m_srcWidth*m_srcHeight*4);;
	
	//char s[128]={0};

	//sprintf(s,"webrtc://%s:1985/live/livestream",m_context->avinfo.rtc.localIp);
	//yang_trace("\nurl===%s",s);
	//yang_lvgl_setUrlText(s);

	m_isStartplay=false;


	m_context->avinfo.rtc.iceCandidateType=YangIceHost;
	m_context->avinfo.rtc.turnSocketProtocol=Yang_Socket_Protocol_Udp;

	m_context->avinfo.rtc.enableAudioBuffer=yangtrue; //use audio buffer
	m_context->avinfo.audio.enableAudioFec=yangfalse; //srs not use audio fec


	memset(&m_frame,0,sizeof(m_frame));

}

YangMetaplayer::~YangMetaplayer()
{
	yang_delete(m_player);
	yang_closeLogFile();
	yang_delete(m_context);
	yang_free(m_buffer);
	yang_free(m_rgbaBuffer);

}


void YangMetaplayer::success(){

}
void YangMetaplayer::failure(int32_t errcode){



}


void YangMetaplayer::close(){

	yang_usleep(50 * 1000);
	if (m_player)
		m_player->stopPlay();

	m_isStartplay = !m_isStartplay;
}
void YangMetaplayer::playRtc(char* url)
{
	m_context->synMgr.session->playBuffer->resetVideoClock(m_context->synMgr.session->playBuffer->session);
	int32_t err = Yang_Ok;

	err = m_player->play(url);

	if (err)
	{
		yang_error("play url error!");
	
	}
}






