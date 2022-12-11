//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangaudiodev/YangAudioRenderData.h>
#include <yangavutil/audio/YangAudioUtil.h>
#include <yangutil/sys/YangLog.h>
using namespace std;
#define Yang_Mix_BUF_Len 4096
YangAudioRenderData::YangAudioRenderData() {
    m_cacheLen = 1024 * 4 * 10;
    m_cache = new uint8_t[m_cacheLen];
    m_size = 0;
    m_pos = 0;

    m_aecBufLen = 0;
    m_renderLen=0;

    m_syn = NULL;
    m_in_audioBuffers=NULL;

    m_hasAec=false;
    m_aecBuf=NULL;
    m_mixBuf=NULL;

    m_preProcess=NULL;
    m_mixPos=0;
    memset(&m_res,0,sizeof(YangAudioResample));
    yang_create_audioresample(&m_res);
    m_res.init(m_res.context,48000, 2, 48000, 2, 20);
}

YangAudioRenderData::~YangAudioRenderData() {
    yang_deleteA(m_cache);
    yang_deleteA(m_mixBuf);
    m_syn = NULL;
     m_in_audioBuffers=NULL;

    m_preProcess=NULL;
    yang_destroy_audioresample(&m_res);
}

void YangAudioRenderData::setInAudioBuffer(YangSynBuffer *pal) {
    m_syn = pal;
}

void YangAudioRenderData::initRender(int psample, int pchannel) {
    m_res.initOut(m_res.context,psample, pchannel);
}

void YangAudioRenderData::initPlay(int psample, int pchannel) {
    m_res.initIn(m_res.context,psample, pchannel);
}



void YangAudioRenderData::setAec(){

    if(m_aecBuf==NULL) m_aecBuf=new uint8_t[Yang_Mix_BUF_Len];
    m_hasAec=true;
}
void YangAudioRenderData::setInAudioBuffers(std::vector<YangSynBuffer*> *pal) {
    if(m_in_audioBuffers) return;
    m_in_audioBuffers = pal;
    if(m_mixBuf==NULL){
        m_mixBuf=new uint8_t[960*8];
    }
}

bool YangAudioRenderData::hasData() {
    if (!m_in_audioBuffers||m_in_audioBuffers->size()==0)    return false;
    for (int32_t i = 0; i < (int) m_in_audioBuffers->size(); i++) {

        if (m_in_audioBuffers->at(i)&&m_in_audioBuffers->at(i)->getAudioSize() > 0)
            return true;
    }

    return false;
}


uint8_t* YangAudioRenderData::getAudioRef(YangFrame* pframe){
    if(m_syn) {
    	return m_syn->getAudioRef(pframe);
    }

    if(m_in_audioBuffers&&hasData()){
               uint8_t *tmp = NULL;

                for (size_t i = 0; i < m_in_audioBuffers->size(); i++) {
                    if (m_in_audioBuffers->at(i) && m_in_audioBuffers->at(i)->getAudioSize() > 0) {
                        tmp = m_in_audioBuffers->at(i)->getAudioRef(pframe);

                        if (tmp) {
                           // if (m_preProcess)	m_preProcess->preprocess_run((short*) tmp);
                            if (i == 0) {
                                memcpy(m_mixBuf, tmp, pframe->nb);
                            } else {
                               yang_mixaudio_mix1((short*)m_mixBuf, (short*)tmp, pframe->nb, 128);
                            }
                        }
                        tmp = NULL;

                        // if (m_in_audioBuffer->at(i)->size() > m_audioPlayCacheNum)                m_in_audioBuffer->at(i)->resetIndex();
                    }
                }
                return m_mixBuf;
    }
    return NULL;
}

uint8_t* YangAudioRenderData::getAecAudioData(){
    if(m_renderLen<m_res.context->inBytes||m_aecBufLen<m_res.context->inBytes) return NULL;

    if((m_mixPos+m_res.context->inBytes)>=Yang_Mix_BUF_Len) {
        memmove(m_aecBuf,m_aecBuf+m_mixPos,m_aecBufLen);
        m_mixPos=0;
    }

    uint8_t* p=m_aecBuf+m_mixPos;
    m_aecBufLen-=m_res.context->inBytes;
    m_mixPos+=m_res.context->inBytes;
    m_renderLen=0;
    return p;
}

void YangAudioRenderData::setRenderLen(int plen){
    m_renderLen+=(m_res.context->inBytes*plen)/m_res.context->outBytes;
}

uint8_t* YangAudioRenderData::getAudioData(YangFrame* frame){


    uint8_t* tmp=getAudioRef(frame);

    if(m_hasAec&&tmp){
        if((m_mixPos+m_aecBufLen+m_res.context->inBytes)>=Yang_Mix_BUF_Len) {
            memmove(m_aecBuf,m_aecBuf+m_mixPos,m_aecBufLen);
            m_mixPos=0;
        }

        memcpy(m_aecBuf+m_mixPos+m_aecBufLen,tmp,m_res.context->inBytes);
        m_aecBufLen+=m_res.context->inBytes;
    }

    return tmp;
}

void YangAudioRenderData::setAudioData(YangFrame* frame){
    uint8_t *buf =getAudioData(frame);
    frame->payload=buf;
    m_res.resample(m_res.context,frame);
}

uint8_t* YangAudioRenderData::getRenderAudioData(int len) {
    if ((m_pos + m_size + len) >= m_cacheLen) {
        memmove(m_cache, m_cache + m_pos, m_size);
        m_pos = 0;
    }

    m_audioFrame.payload=NULL;
    m_audioFrame.nb=0;

    if((m_size+(len<<1))<m_cacheLen)    setAudioData(&m_audioFrame);

    if(m_audioFrame.payload&&m_audioFrame.nb>0) {
        if ((m_pos + m_size + m_audioFrame.nb) >= m_cacheLen) {
            memmove(m_cache, m_cache + m_pos, m_size);
            m_pos = 0;
        }
        memcpy(m_cache + m_pos + m_size, m_audioFrame.payload, m_audioFrame.nb);
        m_size += m_audioFrame.nb;
    }

    if (len > m_size)	return NULL;
    uint8_t *p = m_cache + m_pos;

    m_size -= len;
    if(m_size==0)
    	m_pos=0;
    else
    	m_pos += len;
    return p;

}

