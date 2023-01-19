//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangandroid/MetaPlayer.h>

#include <yangutil/sys/YangLog.h>
#include <yangutil/yangavinfotype.h>
#include <yangavutil/video/YangYuvConvert.h>

// TODO 异步 函数指针
void * customTaskStartThread(void * pVoid) {
	MetaPlayer * player = static_cast<MetaPlayer *>(pVoid);
	player->startPlayerTask();
	return 0;
}

MetaPlayer::MetaPlayer(YangContext* pcontext) {
	m_context=pcontext;
	m_player=NULL;
	m_player=new YangPlayerAndroid(pcontext);
	m_gl=NULL;
	m_render_width=0;
	m_render_height=0;


	m_width = 0;
	m_height = 0;
	this->m_isloop = false;
	m_isSoftdecoder=pcontext->avinfo.video.videoDecHwType==Yang_Hw_Soft?true:false;

}

MetaPlayer::~MetaPlayer() {
	m_gl->eglClose();
	if(m_player->m_isStartplay) m_player->stopPlay();
	yang_delete(m_gl);
	yang_delete(m_player);
	yang_delete(m_context);
}

void MetaPlayer::init(ANativeWindow* pwindows){
	if(m_gl==NULL) m_gl=new YangYuvGl();
	m_gl->m_context.nativeWindow=pwindows;
}
int MetaPlayer::startPlayer(std::string url) {
	yang_trace("\nurl=%s\n",url.c_str());
	if(m_isloop) {
		yang_trace("startPlayer, m_isloop == true");
		return Yang_Ok;
	}

	m_isloop = true;
	if(m_player&&m_player->startPlay(url)==Yang_Ok){
		yang_thread_create(&m_pid_player, 0, customTaskStartThread, this);
		return Yang_Ok;
	}
	return 1;
}

void MetaPlayer::stopPlayer() {
	yang_trace("stopPlayer in");
	this->m_isloop = false;
	if(m_player) m_player->stopPlay();
	yang_trace("stopPlayer out");
}

void MetaPlayer::startPlayerTask() {
	yang_trace("startPlayerTask in");
	if(m_gl) m_gl->eglOpen();
	m_isloop=true;
	uint8_t* data=NULL;
	uint8_t* render_data=NULL;
	uint8_t* yuvData=NULL;
	int64_t timestamp=0;

	YangYuvConvert yuv;
	while (m_isloop) {
		usleep(20 * 1000);
		data=NULL;
		if(m_player)            data=m_player->getVideoRef(&timestamp);
		if(data==NULL) continue;
		if(m_width==0){
			m_width=m_player->getWidth();
			m_height=m_player->getHeight();
			adjustVideoScaleResolution();
			yang_trace("width=%d,height=%d,render width=%d,render height=%d",m_width,m_height,m_render_width,m_render_height);
			m_gl->m_width=m_render_width;
			m_gl->m_height=m_render_height;
			if((m_render_width!=m_width||m_render_height!=m_height)&&render_data==NULL) render_data=new uint8_t[m_render_width * m_render_height*3/2];


			m_gl->createProgram();
		}

		if((m_render_width == m_width) && (m_render_height == m_height)) {

			m_gl->render(data);
		} else {
			if(render_data==NULL) render_data=new uint8_t[m_render_width * m_render_height*3/2];
			yuv.scaleI420(data, render_data,m_width, m_height,m_render_width , m_render_height, kFilterNone);
			m_gl->render(render_data);
		}


	}
	yang_deleteA( render_data);
	yang_deleteA( yuvData);
	m_isloop = false;
	yang_trace("startPlayerTask out");
}




void MetaPlayer::adjustVideoScaleResolution() {


		if(m_width % 64 != 0) {
			m_render_width = YANGALIGN(m_width,64);
		} else {
			m_render_width = this->m_width;
		}


	if(m_height % 2 != 0) {
		m_render_height = ((this->m_height / 2) + 1) * 2;
	} else {
		m_render_height = this->m_height;
	}

}

