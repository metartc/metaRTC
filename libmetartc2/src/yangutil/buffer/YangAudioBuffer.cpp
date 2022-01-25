#include "yangutil/buffer/YangAudioBuffer.h"

#include <stdlib.h>
#include "stdio.h"



YangAudioBuffer::YangAudioBuffer(int32_t pcacheNum)
{
	resetIndex();
	m_cache_num=pcacheNum;
	m_bufLen=0;
}

void YangAudioBuffer::reset(){
	resetIndex();
}

YangAudioBuffer::~YangAudioBuffer(void)
{

}

void YangAudioBuffer::putAudio(YangFrame* pframe)
{
	if(m_bufLen==0){
		m_bufLen=pframe->nb;
		initFrames(m_cache_num,pframe->nb);
	}
	putFrame(pframe);

}

int32_t YangAudioBuffer::getAudio(YangFrame* pframe)
{
    if(size()>0){
    	getFrame(pframe);
        return 0;
    }else
        return 1;
}


uint8_t *YangAudioBuffer::getAudioRef(YangFrame* pframe)
{
    if(size()>0){
    	return getFrameRef(pframe);
    }else{
        return NULL;
    }
}

