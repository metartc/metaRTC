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
    player->startYUVPlayerTask();
    return 0;
}

MetaPlayer::MetaPlayer(YangGlobalContexts *p_global_context) {

    m_player=new YangPlayerAndroid();
    global_context = p_global_context;
    this->m_isloop = false;

}

MetaPlayer::~MetaPlayer() {

    if (this->eglDisplayYuv != NULL) {
        delete this->eglDisplayYuv;
        this->eglDisplayYuv = NULL;
    }
    if (this->shaderYuv != NULL) {
        delete this->shaderYuv;
        this->shaderYuv = NULL;
    }




}

int MetaPlayer::startPlayer(std::string url) {
	yang_trace("\nurl=%s\n",url.c_str());
    if(this->m_isloop) {
        yang_trace("startPlayer, m_isloop == true");
        return Yang_Ok;
    }

    this->m_isloop = true;
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

void MetaPlayer::startYUVPlayerTask() {
    yang_trace("startYUVPlayerTask in");
    if(NULL != eglDisplayYuv) {
        delete eglDisplayYuv;
        eglDisplayYuv = NULL;
    }

    eglDisplayYuv = new YangEGLDisplayYUV(this->global_context->nativeWindow, this->global_context);
    eglDisplayYuv->eglOpen();


    if(NULL != shaderYuv) {
        delete shaderYuv;
        shaderYuv = NULL;
    }



    m_isloop=true;
    uint8_t* data=NULL;
    uint8_t* scale_frame_data=NULL;
    int64_t timestamp=0;

    YangYuvConvert yuv;
   // int width=0;
  //  int height=0;
    while (m_isloop) {
        usleep(20 * 1000);
        data=NULL;
        if(m_player)            data=m_player->getVideoRef(&timestamp);
        if(data==NULL) continue;
        if(video_width==0){
        	video_width=m_player->getWidth();
        	video_height=m_player->getHeight();
        	 adjustVideoScaleResolution();
        	 global_context->gl_video_width=scale_video_width;
        	 global_context->gl_video_height=scale_video_height;
        	 if(scale_video_width!=video_width) scale_frame_data=new uint8_t[scale_video_width * scale_video_height*3/2];
        	    shaderYuv = new YangShaderYUV(this->global_context);
        	    shaderYuv->createProgram();
        }
        shaderYuv->render(data);
        if((scale_video_width == video_width)
            && (this->scale_video_height == this->video_height)) {
            shaderYuv->render(data);
        } else {

        	yuv.scaleI420(data, scale_frame_data,this->video_width, this->video_height,scale_video_width , scale_video_height, kFilterNone);
            shaderYuv->render(scale_frame_data);
        }


    }
    yang_deleteA( scale_frame_data);

    this->m_isloop = false;
    yang_trace("startYUVPlayerTask out");
}




void MetaPlayer::adjustVideoScaleResolution() {
    if(this->video_width % 8 != 0) {
        this->scale_video_width = ((this->video_width / 8) + 1) * 8;
    } else {
        this->scale_video_width = this->video_width;
    }
    if(this->video_height % 2 != 0) {
        this->scale_video_height = ((this->video_height / 2) + 1) * 2;
    } else {
        this->scale_video_height = this->video_height;
    }
}

