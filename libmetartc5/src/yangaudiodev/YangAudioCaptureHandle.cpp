//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangaudiodev/YangAudioCaptureHandle.h>
#include <yangutil/yang_unistd.h>
#include <yangutil/sys/YangLog.h>

YangAudioCaptureHandle::YangAudioCaptureHandle(YangContext *pcontext)
{

    isBuf=0;
    m_audioList=NULL;
    m_aec=NULL;
    m_aecPlayBuffer=NULL;
    pcm=new short[4096/2];
    m_audioLen=pcontext->avinfo.audio.sample*pcontext->avinfo.audio.channel*2/50;

    hasPlayData=1;
    isFirst=1;
    m_aecBufferFrames=pcontext->avinfo.audio.aecBufferFrames;
    memset(&m_audioFrame,0,sizeof(YangFrame));

    memset(&m_resample,0,sizeof(YangAudioResample));
    yang_create_audioresample(&m_resample);
    m_resample.init(m_resample.context,16000,1,pcontext->avinfo.audio.sample,pcontext->avinfo.audio.channel,20);

}
YangAudioCaptureHandle::~YangAudioCaptureHandle(void)
{
	m_aec=NULL;
	m_aecPlayBuffer=NULL;
	m_audioList=NULL;
	yang_deleteA(pcm);

	yang_destroy_audioresample(&m_resample);
}



void YangAudioCaptureHandle::startRecordWave(char* filename){

}
void YangAudioCaptureHandle::stopRecordWave(){

}

void  YangAudioCaptureHandle::setOutAudioBuffer(YangAudioBuffer *pbuf)
{
    m_audioList=pbuf;
}


    void YangAudioCaptureHandle::putBuffer(uint8_t *pBuffer,int32_t plen)
    {
		if(!isBuf) return;
		if(m_aec) {
			if(hasPlayData)		{
                m_aec->echoCapture(m_aec->session,(short*)pBuffer,pcm);
                m_aec->preprocessRun(m_aec->session,pcm);
			}
			if(m_aecPlayBuffer&&m_aecPlayBuffer->size()>m_aecBufferFrames){
				uint8_t* tmp=m_aecPlayBuffer->getAudioRef(&m_audioFrame);
				if(tmp)
                m_aec->echoPlayback(m_aec->session,(short*)tmp);

				if(isFirst){
					if(m_audioList) m_audioList->resetIndex();
					isFirst=0;
				}
				hasPlayData=1;

			}else
				hasPlayData=0;


			if(hasPlayData){
				if(m_audioList)	{
					m_audioFrame.payload=(uint8_t *)pcm;
					m_audioFrame.nb=plen;
					m_audioList->putAudio(&m_audioFrame);
				}
			}else{
				if(m_audioList)	{
					m_audioFrame.payload=pBuffer;
					m_audioFrame.nb=plen;
					m_audioList->putAudio(&m_audioFrame);
				}
			}
		}else{

			if(m_audioList)	{
				m_audioFrame.payload=pBuffer;
				m_audioFrame.nb=plen;
				m_audioList->putAudio(&m_audioFrame);
			}
		}


    }
    void YangAudioCaptureHandle::putEchoPlay(short* pbuf,int32_t plen){
    	if(!isBuf) return;
        if(m_aec) m_aec->echoPlayback(m_aec->session,pbuf);
    }
    void YangAudioCaptureHandle::putEchoBuffer( uint8_t *pBuffer,int32_t plen){
    	if (!isBuf)		return;
    	if (m_aec) {
            m_aec->echoCapture(m_aec->session, (short*) pBuffer, pcm);
            m_aec->preprocessRun(m_aec->session, pcm);
            m_audioFrame.payload = (uint8_t*)pcm;
    		m_audioFrame.nb = plen;
    		m_resample.resample(m_resample.context,&m_audioFrame);
    		if (m_audioList) {
    		
    			m_audioList->putAudio(&m_audioFrame);
    		}
    	}

    }
    void YangAudioCaptureHandle::putEchoBuffer2( uint8_t *pBuffer,int32_t plen){
    	if (!isBuf)		return;

    	m_audioFrame.payload = pBuffer;
    	m_audioFrame.nb = plen;
    	m_resample.resample(m_resample.context,&m_audioFrame);
    	if (m_audioList) {
    				m_audioList->putAudio(&m_audioFrame);
    	}


    }
    void YangAudioCaptureHandle::putBuffer2( uint8_t *pBuffer,int32_t plen){

	if (!isBuf)
		return;

	if (m_audioList) {
		m_audioFrame.payload = pBuffer;
		m_audioFrame.nb = plen;
		m_audioList->putAudio(&m_audioFrame);
	}

    }

