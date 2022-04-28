//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangandroid/MetaPlayer.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/yangtype.h>
#include <yangavutil/video/YangYuvConvert.h>
// TODO 异步 函数指针
void * customTaskStartThread(void * pVoid) {
	MetaPlayer * player = static_cast<MetaPlayer *>(pVoid);
	player->startPlayerTask();
	return 0;
}

MetaPlayer::MetaPlayer(YangContext* pcontext) {
	m_player=NULL;
	m_player=new YangPlayerAndroid(pcontext);
	m_gl=NULL;
	scale_video_width=0;
	scale_video_height=0;


	m_video_width = 0;
	m_video_height = 0;
	this->m_isloop = false;
	m_isSoftdecoder=pcontext->avinfo.video.videoDecHwType==Yang_Hw_Soft?true:false;

}

MetaPlayer::~MetaPlayer() {

	m_gl->eglClose();
	if(m_player->m_isStartplay) m_player->stopPlay();
	yang_delete(m_gl);
	yang_delete(m_player);




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
		pthread_create(&m_pid_player, 0, customTaskStartThread, this);
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
	uint8_t* scale_frame_data=NULL;
	uint8_t* yuvData=NULL;
	int64_t timestamp=0;

	YangYuvConvert yuv;
	while (m_isloop) {
		usleep(20 * 1000);
		data=NULL;
		if(m_player)            data=m_player->getVideoRef(&timestamp);
		if(data==NULL) continue;
		if(m_video_width==0){
			m_video_width=m_player->getWidth();
			m_video_height=m_player->getHeight();
			adjustVideoScaleResolution();
			m_gl->m_width=scale_video_width;
			m_gl->m_height=scale_video_height;
			if(scale_video_width!=m_video_width) scale_frame_data=new uint8_t[scale_video_width * scale_video_height*3/2];
			//if(!m_isSoftdecoder&&yuvData==NULL) 	yuvData=new uint8_t[scale_video_width * scale_video_height*3/2];

			m_gl->createProgram();
		}

		if((scale_video_width == m_video_width) && (scale_video_height == m_video_height)) {

			m_gl->render(data);
			/**if(m_isSoftdecoder){
				m_gl->render(data);
			}else{
				yuv.nv21toI420(data, yuvData, scale_video_width, m_video_height);
				m_gl->render(yuvData);
			}**/
		} else {
			yuv.scaleI420(data, scale_frame_data,this->m_video_width, this->m_video_height,scale_video_width , scale_video_height, kFilterNone);
			m_gl->render(scale_frame_data);
		}


	}
	yang_deleteA( scale_frame_data);
	yang_deleteA( yuvData);
	m_isloop = false;
	yang_trace("startPlayerTask out");
}




void MetaPlayer::adjustVideoScaleResolution() {
	if(this->m_video_width % 8 != 0) {
		this->scale_video_width = ((this->m_video_width / 8) + 1) * 8;
	} else {
		this->scale_video_width = this->m_video_width;
	}
	if(this->m_video_height % 2 != 0) {
		this->scale_video_height = ((this->m_video_height / 2) + 1) * 2;
	} else {
		this->scale_video_height = this->m_video_height;
	}
}

