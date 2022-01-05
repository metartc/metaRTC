#include "yangutil/buffer/YangAudioPlayBuffer.h"

#include <stdlib.h>
#include "stdio.h"


YangAudioPlayBuffer::YangAudioPlayBuffer() {
	resetIndex();
    m_cache_num = 50;
	m_bufLen=0;

}

void YangAudioPlayBuffer::reset() {
	resetIndex();
}

YangAudioPlayBuffer::~YangAudioPlayBuffer(void) {

}

void YangAudioPlayBuffer::putAudio(YangFrame* pframe) {
	if(m_bufLen==0){
		m_bufLen = pframe->nb;
		initFrames(m_cache_num,pframe->nb);

	}

	putFrame(pframe);
}

void YangAudioPlayBuffer::getAudio(YangFrame* pframe) {

	getFrame(pframe);
}

uint8_t* YangAudioPlayBuffer::getAudios(YangFrame* pframe) {
	if (size() < 1)
		return NULL;
	return getFrameRef(pframe);

}
int32_t YangAudioPlayBuffer::getFrameTimestamp(int64_t *ptimestamp) {

	YangFrame* f=getCurFrameRef();
	if(f) *ptimestamp=f->timestamp;
	return Yang_Ok;
}
int64_t YangAudioPlayBuffer::getNextTimestamp(){
	return getNextFrameTimestamp();
}


