//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangstream/YangSynBuffer.h>
#include <yangutil/sys/YangLog.h>
#include<cmath>
#include <string>
#include <string.h>


YangSynBuffer::YangSynBuffer() {
    m_transtype = Yang_Webrtc;
    if (m_transtype == Yang_Webrtc) {
        m_videoClock = 90000;
        m_audioClock = 48000;
    } else if (m_transtype == Yang_Rtmp) {
        m_videoClock = 1000;
        m_audioClock = 1000;
    }

    m_videoCacheTime=Yang_Video_Cache_time;

    resetAudioClock();
    resetVideoClock();


    m_baseClock = 0;

    m_paused = 0;
    m_uid = 0;
    m_width = 0;
    m_height = 0;

    m_synType = YANG_SYNC_AUDIO_MASTER;
    m_maxAudioMinus = Yang_Max_Audio_Intervaltime;
    m_maxVideoMinus = Yang_Max_Video_Intervaltime;
    m_videoBuffer = NULL;
    m_audioBuffer = NULL;
}

YangSynBuffer::~YangSynBuffer() {
    m_videoBuffer = NULL;
    m_audioBuffer = NULL;
}
void YangSynBuffer::resetVideoClock(){
    m_videoBase = 0;
    m_videoTime = 0;
    m_videoMinus = 0;
    m_pre_videoTime = 0;

    m_video_startClock=0;
    m_videoNegativeCount=0;
    m_videoTimeoutCount=0;

    m_isFirstVideo=false;
    m_video_time_state=0;

}
void YangSynBuffer::resetAudioClock(){
    m_audioBase = 0;
    m_audioTime = 0;
    m_audioMinus = 0;
    m_pre_audioTime = 0;
    m_lostAudioCount=0;
    m_audio_startClock = 0;
    m_audioNegativeCount=0;
    m_audioTimeoutCount=0;
    m_audioDelay=0;
    m_isFirstAudio=false;
}
void YangSynBuffer::setAudioClock(int paudioclock){
    if(paudioclock<=0) return;
    m_audioClock = paudioclock;
}
void YangSynBuffer::setVideoClock(int pvideoclock){
    if(pvideoclock<=0) return;
    m_videoClock = pvideoclock;

}

void YangSynBuffer::setVideoCacheTime(int pctime){
    m_videoCacheTime=pctime;
}
void YangSynBuffer::setInVideoBuffer(YangVideoBuffer *pbuf) {
	m_videoBuffer = pbuf;
}
void YangSynBuffer::setInAudioBuffer(YangAudioPlayBuffer *pbuf) {
	m_audioBuffer = pbuf;
}
void YangSynBuffer::setTranstype(int transtype) {
	m_transtype = transtype;

	if (m_transtype == Yang_Webrtc) {
		m_videoClock = 90000;
		m_audioClock = 48000;
	} else if (m_transtype == Yang_Rtmp) {
		m_videoClock = 1000;
		m_audioClock = 1000;
	}
}
uint8_t* YangSynBuffer::getVideoRef(YangFrame *pframe) {
	if (!m_videoBuffer || !m_videoBuffer->size())
		return NULL;
	uint8_t *tmp = NULL;
    int err=0;


    if ((err=playVideoFrame(m_videoBuffer->getCurVideoFrame()))==Yang_Ok) {

		tmp = m_videoBuffer->getVideoRef(pframe);
		m_width = m_videoBuffer->m_width;
		m_height = m_videoBuffer->m_height;
		return tmp;
    }else  if(err==-1){
        tmp = m_videoBuffer->getVideoRef(pframe);
        m_width = m_videoBuffer->m_width;
        m_height = m_videoBuffer->m_height;
        return NULL;
    }

	return tmp;
}

uint8_t* YangSynBuffer::getAudioRef(YangFrame *audioFrame) {
	if (!m_audioBuffer || !m_audioBuffer->size())
		return NULL;

	//return m_audioBuffer->getAudios(audioFrame);

	int err=0;
	if ((err=playAudioFrame(m_audioBuffer->getNextTimestamp()))==Yang_Ok){

		return m_audioBuffer->getAudios(audioFrame);
	}
	if(err==-1){
		m_audioBuffer->getAudios(audioFrame);

		return getAudioRef(audioFrame);
	}

	return NULL;

}

int32_t YangSynBuffer::getAudioSize() {
	if (m_audioBuffer)
		return m_audioBuffer->size();
	return 0;
}
int32_t YangSynBuffer::getVideoSize() {
	if (m_videoBuffer)
		return m_videoBuffer->size();
	return 0;
}

int YangSynBuffer::playAudioFrame(int64_t pts) {

	if (m_audioBase == 0) {
        if(!m_isFirstAudio){
            //clear cache
            yang_reindex(m_audioBuffer);
            m_isFirstAudio=true;
            return false;
        }
        updateAudioBaseTimestamp(pts);
	}

    if(m_transtype == Yang_Webrtc){
    	//get relative time
        m_audioTime = (pts - m_audioBase) * 1000 / m_audioClock;
    }else{
        m_audioTime = (pts - m_audioBase);
    }

	m_audioMinus = m_audioTime +Yang_Audio_Cache_time- (yang_get_milli_tick() - m_audio_startClock);

	m_pre_audioTime = m_audioTime;

	if(m_audioMinus<0) {
		m_audioNegativeCount++;
		if(m_audioNegativeCount>10){
			updateAudioBaseTimestamp(pts);
			m_audioNegativeCount=0;
		}
		return -1;
	}
	if(m_audioMinus <= m_maxAudioMinus) {
		if(m_audioTime>Yang_Audio_Base_Update_Interval) {


			if(m_lostAudioCount>5){
				YangFrame frame;
				memset(&frame,0,sizeof(YangFrame));

				m_audioBuffer->getAudios(&frame);
				m_audioBuffer->getAudios(&frame);
				m_lostAudioCount=0;
			}
			if(m_audioBuffer->size()>2) m_lostAudioCount++;
			updateAudioBaseTimestamp(pts);

		}
		return Yang_Ok;
	}
	m_audioTimeoutCount++;
	if(m_audioTimeoutCount>10){
		m_audioTimeoutCount=0;
		updateAudioBaseTimestamp(pts);
	}
	return 1;
}

int YangSynBuffer::playVideoFrame(YangFrame* frame) {

    if(m_videoBase==0) {
        if(!m_isFirstVideo){
   	            //clear cache
   	            yang_reindex(m_videoBuffer);
   	            m_isFirstVideo=true;
   	            return false;
   	        }

    	updateVideoBaseTimestamp(frame->pts);
    }
    if(!m_video_time_state&&frame->frametype==YANG_Frametype_I){

    	updateVideoBaseTimestamp(frame->pts);
    	m_video_time_state=1;
    }

	if (m_transtype == Yang_Webrtc)
		m_videoTime = (frame->pts - m_videoBase) * 1000 / m_videoClock;
	else
		m_videoTime = frame->pts - m_videoBase;

    m_videoMinus = m_videoTime+m_videoCacheTime - (yang_get_milli_tick() - m_video_startClock);

    if(m_videoMinus<0) {
        m_videoNegativeCount++;
        if(m_videoNegativeCount>6){
            updateVideoBaseTimestamp(frame->pts);
            m_videoNegativeCount=0;
        }
        return -1;
    }
    if(m_videoMinus <= m_maxVideoMinus) {
    	if(frame->frametype==YANG_Frametype_I) updateVideoBaseTimestamp(frame->pts);
    	return Yang_Ok;
    }
    m_videoTimeoutCount++;
    if(m_videoTimeoutCount>6){
        m_videoTimeoutCount=0;
        updateVideoBaseTimestamp(frame->pts);
    }

    return 1;

}

void YangSynBuffer::updateVideoBaseTimestamp(int64_t pts){
			m_videoBase = pts;
	        m_video_startClock=yang_get_milli_tick();
}
void YangSynBuffer::updateAudioBaseTimestamp(int64_t pts){
	m_audioBase=pts;
	m_audio_startClock = yang_get_milli_tick();
}
void YangSynBuffer::setClock() {
	m_audio_startClock = yang_get_milli_tick();
}

