#include <yangaudiodev/YangAudioCaptureHandle.h>
#include "yangutil/yang_unistd.h"


YangAudioCaptureHandle::YangAudioCaptureHandle(YangContext *pcontext)
{

    isBuf=0;
    m_audioList=NULL;
    m_aec=NULL;
    m_aecPlayBuffer=NULL;
    pcm=new short[4096/2];
    hasPlayData=1;
    isFirst=1;
    m_aecBufferFrames=pcontext->audio.aecBufferFrames;
    memset(&m_audioFrame,0,sizeof(YangFrame));

}
YangAudioCaptureHandle::~YangAudioCaptureHandle(void)
{
	m_aec=NULL;
	m_aecPlayBuffer=NULL;
	m_audioList=NULL;
	yang_deleteA(pcm);
}



void YangAudioCaptureHandle::startRecordWave(char* filename){
	//wavRecord.start(1,filename);
	//isRecordAudio=1;
}
void YangAudioCaptureHandle::stopRecordWave(){
	//isRecordAudio=0;
	//wavRecord.stop();

}

void  YangAudioCaptureHandle::setOutAudioBuffer(YangAudioBuffer *pbuf)
{
    m_audioList=pbuf;
}

//int32_t tcou=0;
    void YangAudioCaptureHandle::putBuffer(uint8_t *pBuffer,int32_t plen)
    {
		if(!isBuf) return;
		if(m_aec) {
			if(hasPlayData)		{
				m_aec->echo_capture((short*)pBuffer,pcm);
				m_aec->preprocess_run(pcm);
			}
			if(m_aecPlayBuffer&&m_aecPlayBuffer->size()>m_aecBufferFrames){
				uint8_t* tmp=m_aecPlayBuffer->getAudioRef(&m_audioFrame);
				if(tmp)
				m_aec->echo_playback((short*)tmp);
				//printf("%d,",m_aecPlayBuffer->size());
				if(isFirst){
					if(m_audioList) m_audioList->resetIndex();
					isFirst=0;
				}
				hasPlayData=1;
				//m_aec->echo_cancellation((short*)pBuffer,(short*)m_aecPlayBuffer->getAudio(),pcm);
				//speex_echo_cancellation()
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
			//printf("2,");
			if(m_audioList)	{
				m_audioFrame.payload=pBuffer;
				m_audioFrame.nb=plen;
				m_audioList->putAudio(&m_audioFrame);
			}
		}
		//aeaac->encoder(pBuffer,BufferLen);

    }
    void YangAudioCaptureHandle::putEchoPlay(short* pbuf,int32_t plen){
    	if(!isBuf) return;
    	if(m_aec) m_aec->echo_playback(pbuf);
    }
    void YangAudioCaptureHandle::putEchoBuffer( uint8_t *pBuffer,int32_t plen){
    	if(!isBuf) return;
    			if(m_aec) {
                    m_aec->echo_capture((short*)pBuffer,pcm);
    				m_aec->preprocess_run(pcm);

    				if(m_audioList){
    					m_audioFrame.payload=pBuffer;
    					m_audioFrame.nb=plen;
    					m_audioList->putAudio(&m_audioFrame);
    				}
    			}

    }
    void YangAudioCaptureHandle::putBuffer1( uint8_t *pBuffer,int32_t plen){

    	if(!isBuf) return;
    				if(m_audioList)	{
    					m_audioFrame.payload=pBuffer;
    				    m_audioFrame.nb=plen;
    					m_audioList->putAudio(&m_audioFrame);
    				}

    }

