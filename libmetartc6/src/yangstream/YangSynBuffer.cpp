//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangstream/YangSynBuffer.h>
#include <yangutil/sys/YangLog.h>
#include<cmath>


typedef struct{
    int32_t uid;
    int32_t width;
    int32_t height;
    int32_t maxAudioMinus;
     int32_t maxVideoMinus;
     yangbool isFirstVideo;
     yangbool isFirstAudio;
     int32_t video_time_state;

     YangSynType synType;
     int32_t paused;
     int64_t baseClock;

     int64_t audio_startClock;
     int32_t audioClock;
     int64_t audioBase;
     int64_t audioTime;
     int32_t audioMinus;
     int32_t audioDelay;
     int32_t lostAudioCount;
     int32_t audioNegativeCount;
     int32_t audioTimeoutCount;


     int64_t videoBase;
     int32_t videoClock;
     int64_t videoTime;
     int32_t videoMinus;
     int32_t videoNegativeCount;
     int32_t videoTimeoutCount;
     int32_t videoCacheTime;
     int64_t video_startClock;

     int64_t pre_audioTime;
     int64_t pre_videoTime;

     int32_t transtype;

     YangVideoBuffer *videoBuffer;
     YangAudioPlayBuffer* audioBuffer;


}YangSynBufferSession;


void yang_synBuf_updateVideoBaseTimestamp(void* psession,int64_t pts){
	if(psession==NULL) return ;
	YangSynBufferSession* session=(YangSynBufferSession*)psession;
			session->videoBase = pts;
	        session->video_startClock=yang_get_milli_tick();
}
void yang_synBuf_updateAudioBaseTimestamp(void* psession,int64_t pts){
	if(psession==NULL) return ;
	YangSynBufferSession* session=(YangSynBufferSession*)psession;
	session->audioBase=pts;
	session->audio_startClock = yang_get_milli_tick();
}


int yang_synBuf_playAudioFrame(void* psession,int64_t pts) {
	if(psession==NULL) return 1;
	YangSynBufferSession* session=(YangSynBufferSession*)psession;

	if (session->audioBase == 0) {
        if(!session->isFirstAudio){
            //clear cache
            yang_reindex(session->audioBuffer);
            session->isFirstAudio=true;
            return false;
        }
        yang_synBuf_updateAudioBaseTimestamp(psession,pts);
	}

    if(session->transtype == Yang_Webrtc){
    	//get relative time
        session->audioTime = (pts - session->audioBase) * 1000 / session->audioClock;
    }else{
        session->audioTime = (pts - session->audioBase);
    }

	session->audioMinus = session->audioTime +Yang_Audio_Cache_time- (yang_get_milli_tick() - session->audio_startClock);

	session->pre_audioTime = session->audioTime;

	if(session->audioMinus<0) {
		session->audioNegativeCount++;
		if(session->audioNegativeCount>10){
			yang_synBuf_updateAudioBaseTimestamp(psession,pts);
			session->audioNegativeCount=0;
		}
		return -1;
	}
	if(session->audioMinus <= session->maxAudioMinus) {
		if(session->audioTime>Yang_Audio_Base_Update_Interval) {


			if(session->lostAudioCount>5){
				YangFrame frame;
				memset(&frame,0,sizeof(YangFrame));

				session->audioBuffer->getAudios(&frame);
				session->audioBuffer->getAudios(&frame);
				session->lostAudioCount=0;
			}
			if(session->audioBuffer->size()>2) session->lostAudioCount++;
			yang_synBuf_updateAudioBaseTimestamp(psession,pts);

		}
		return Yang_Ok;
	}
	session->audioTimeoutCount++;
	if(session->audioTimeoutCount>10){
		session->audioTimeoutCount=0;
		yang_synBuf_updateAudioBaseTimestamp(psession,pts);
	}
	return 1;
}

int yang_synBuf_playVideoFrame(void* psession,YangFrame* frame) {
	if(psession==NULL) return 1;
	YangSynBufferSession* session=(YangSynBufferSession*)psession;
    if(session->videoBase==0) {
        if(!session->isFirstVideo){
   	            //clear cache
   	            yang_reindex(session->videoBuffer);
   	            session->isFirstVideo=yangtrue;
   	            return yangfalse;
   	        }

        yang_synBuf_updateVideoBaseTimestamp(psession,frame->pts);
    }
    if(!session->video_time_state&&frame->frametype==YANG_Frametype_I){

    	yang_synBuf_updateVideoBaseTimestamp(psession,frame->pts);
    	session->video_time_state=1;
    }

	if (session->transtype == Yang_Webrtc)
		session->videoTime = (frame->pts - session->videoBase) * 1000 / session->videoClock;
	else
		session->videoTime = frame->pts - session->videoBase;

    session->videoMinus = session->videoTime+session->videoCacheTime - (yang_get_milli_tick() - session->video_startClock);

    if(session->videoMinus<0) {
        session->videoNegativeCount++;
        if(session->videoNegativeCount>6){
        	yang_synBuf_updateVideoBaseTimestamp(psession,frame->pts);
            session->videoNegativeCount=0;
        }
        return -1;
    }
    if(session->videoMinus <= session->maxVideoMinus) {
    	if(frame->frametype==YANG_Frametype_I) yang_synBuf_updateVideoBaseTimestamp(psession,frame->pts);
    	return Yang_Ok;
    }
    session->videoTimeoutCount++;
    if(session->videoTimeoutCount>6){
        session->videoTimeoutCount=0;
        yang_synBuf_updateVideoBaseTimestamp(psession,frame->pts);
    }

    return 1;

}

void yang_synBuf_resetVideoClock(void* psession){
	if(psession==NULL) return ;
	YangSynBufferSession* session=(YangSynBufferSession*)psession;
    session->videoBase = 0;
    session->videoTime = 0;
    session->videoMinus = 0;
    session->pre_videoTime = 0;

    session->video_startClock=0;
    session->videoNegativeCount=0;
    session->videoTimeoutCount=0;

    session->isFirstVideo=yangfalse;
    session->video_time_state=0;

}
void yang_synBuf_resetAudioClock(void* psession){
	if(psession==NULL) return ;
	YangSynBufferSession* session=(YangSynBufferSession*)psession;
    session->audioBase = 0;
    session->audioTime = 0;
    session->audioMinus = 0;
    session->pre_audioTime = 0;
    session->lostAudioCount=0;
    session->audio_startClock = 0;
    session->audioNegativeCount=0;
    session->audioTimeoutCount=0;
    session->audioDelay=0;
    session->isFirstAudio=yangfalse;
}
void yang_synBuf_setAudioClock(void* psession,int paudioclock){
    if(psession==NULL||paudioclock<=0) return;
    YangSynBufferSession* session=(YangSynBufferSession*)psession;
    session->audioClock = paudioclock;
}
void yang_synBuf_setVideoClock(void* psession,int pvideoclock){
	if(psession==NULL) return ;
    if(pvideoclock<=0) return;
    YangSynBufferSession* session=(YangSynBufferSession*)psession;
    session->videoClock = pvideoclock;

}

void yang_synBuf_setVideoCacheTime(void* psession,int pctime){
	if(psession==NULL) return ;
	YangSynBufferSession* session=(YangSynBufferSession*)psession;
    session->videoCacheTime=pctime;
}
void yang_synBuf_setInVideoBuffer(void* psession,YangVideoBuffer *pbuf) {
	if(psession==NULL) return ;
	YangSynBufferSession* session=(YangSynBufferSession*)psession;
	session->videoBuffer = pbuf;
}
void yang_synBuf_setInAudioBuffer(void* psession,YangAudioPlayBuffer *pbuf) {
	if(psession==NULL) return ;
	YangSynBufferSession* session=(YangSynBufferSession*)psession;
	session->audioBuffer = pbuf;
}
void yang_synBuf_setTranstype(void* psession,int transtype) {
	if(psession==NULL) return;
	YangSynBufferSession* session=(YangSynBufferSession*)psession;
	session->transtype = transtype;

	if (session->transtype == Yang_Webrtc) {
		session->videoClock = 90000;
		session->audioClock = 48000;
	} else if (session->transtype == Yang_Rtmp) {
		session->videoClock = 1000;
		session->audioClock = 1000;
	}
}
uint8_t* yang_synBuf_getVideoRef(void* psession,YangFrame *pframe) {
	if(psession==NULL||pframe==NULL) return NULL;
	YangSynBufferSession* session=(YangSynBufferSession*)psession;
	if (!session->videoBuffer || !session->videoBuffer->size())
		return NULL;
	uint8_t *tmp = NULL;
    int err=0;


    if ((err=yang_synBuf_playVideoFrame(psession,session->videoBuffer->getCurVideoFrame()))==Yang_Ok) {

		tmp = session->videoBuffer->getVideoRef(pframe);
		session->width = session->videoBuffer->m_width;
		session->height = session->videoBuffer->m_height;
		return tmp;
    }else  if(err==-1){
        tmp = session->videoBuffer->getVideoRef(pframe);
        session->width = session->videoBuffer->m_width;
        session->height = session->videoBuffer->m_height;
        return NULL;
    }

	return tmp;
}

uint8_t* yang_synBuf_getAudioRef(void* psession,YangFrame *audioFrame) {
	if(psession==NULL||audioFrame==NULL) return NULL;
	YangSynBufferSession* session=(YangSynBufferSession*)psession;
	if (!session->audioBuffer || !session->audioBuffer->size())
		return NULL;

	int err=0;
	if ((err=yang_synBuf_playAudioFrame(psession,session->audioBuffer->getNextTimestamp()))==Yang_Ok){

		return session->audioBuffer->getAudios(audioFrame);
	}
	if(err==-1){
		session->audioBuffer->getAudios(audioFrame);

		return yang_synBuf_getAudioRef(psession,audioFrame);
	}

	return NULL;

}

int32_t yang_synBuf_getAudioSize(void* psession) {
	if(psession==NULL) return 0;
	YangSynBufferSession* session=(YangSynBufferSession*)psession;
	if (session->audioBuffer)
		return session->audioBuffer->size();
	return 0;
}
int32_t yang_synBuf_getVideoSize(void* psession) {
	if(psession==NULL) return 0;
	YangSynBufferSession* session=(YangSynBufferSession*)psession;
	if (session->videoBuffer)
		return session->videoBuffer->size();
	return 0;
}

int32_t yang_synBuf_getUid(void* psession){
	if(psession==NULL) return -1;
	YangSynBufferSession* session=(YangSynBufferSession*)psession;
	return session->uid;
}

void yang_synBuf_setUid(void* psession,int32_t uid){
	if(psession==NULL) return ;
	YangSynBufferSession* session=(YangSynBufferSession*)psession;
	session->uid=uid;
}

void yang_synBuf_setClock(void* psession) {
	if(psession==NULL) return ;
	YangSynBufferSession* session=(YangSynBufferSession*)psession;
	session->audio_startClock = yang_get_milli_tick();
}

int32_t yang_synBuf_width(void* psession){
	if(psession==NULL) return 0;
	YangSynBufferSession* session=(YangSynBufferSession*)psession;
	return session->width;
}
  int32_t yang_synBuf_height(void* psession){
	  if(psession==NULL) return 0;
	  YangSynBufferSession* session=(YangSynBufferSession*)psession;
	  return session->height;
  }


void yang_create_synBuffer(YangSynBuffer* syn) {
	if(syn==NULL) return;
	YangSynBufferSession* session=(YangSynBufferSession*)yang_calloc(sizeof(YangSynBufferSession),1);
	syn->session=session;
    session->transtype = Yang_Webrtc;
    if (session->transtype == Yang_Webrtc) {
        session->videoClock = 90000;
        session->audioClock = 48000;
    } else if (session->transtype == Yang_Rtmp) {
        session->videoClock = 1000;
        session->audioClock = 1000;
    }

    session->videoCacheTime=Yang_Video_Cache_time;

    yang_synBuf_resetAudioClock(session);
    yang_synBuf_resetVideoClock(session);

    session->baseClock = 0;

    session->paused = 0;
    session->uid = 0;
    session->width = 0;
    session->height = 0;

    session->synType = YANG_SYNC_AUDIO_MASTER;
    session->maxAudioMinus = Yang_Max_Audio_Intervaltime;
    session->maxVideoMinus = Yang_Max_Video_Intervaltime;
    session->videoBuffer = NULL;
    session->audioBuffer = NULL;
    session->transtype=Yang_Webrtc;

    syn->setInVideoBuffer=yang_synBuf_setInVideoBuffer;
    syn->setInAudioBuffer=yang_synBuf_setInAudioBuffer;
    syn->getAudioRef=yang_synBuf_getAudioRef;
    syn->getVideoRef=yang_synBuf_getVideoRef;
    syn->setAudioClock=yang_synBuf_setAudioClock;
    syn->setVideoClock=yang_synBuf_setVideoClock;
    syn->setUid=yang_synBuf_setUid;
    syn->getUid=yang_synBuf_getUid;
    syn->getAudioSize=yang_synBuf_getAudioSize;
    syn->getVideoSize=yang_synBuf_getVideoSize;
    syn->resetAudioClock=yang_synBuf_resetAudioClock;
    syn->resetVideoClock=yang_synBuf_resetVideoClock;
    syn->width=yang_synBuf_width;
    syn->height=yang_synBuf_height;


}

void yang_destroy_synBuffer(YangSynBuffer* syn) {
	if(syn==NULL) return;
	yang_free(syn->session);
}


