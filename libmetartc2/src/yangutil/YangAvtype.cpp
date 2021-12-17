#include <yangutil/yangavtype.h>
#include <yangutil/yangtype.h>
#include <yangutil/yangavinfotype.h>

#include <memory.h>
void yang_frame_copy_nobuffer(YangFrame *src, YangFrame *dst) {
	if (!src || !dst)		return;
	dst->mediaType = src->mediaType;
	dst->uid = src->uid;
	dst->frametype = src->frametype;
	dst->nb = src->nb;
	dst->timestamp = src->timestamp;
}
void yang_frame_copy(YangFrame *src, YangFrame *dst) {
	if (!src || !dst)		return;
	dst->mediaType = src->mediaType;
	dst->uid = src->uid;
	dst->frametype = src->frametype;
	dst->nb = src->nb;
	dst->timestamp = src->timestamp;
	dst->payload = src->payload;
}
void yang_frame_copy_buffer(YangFrame *src, YangFrame *dst) {
	if (!src || !dst)		return;
	dst->mediaType = src->mediaType;
	dst->uid = src->uid;
	dst->frametype = src->frametype;
	dst->nb = src->nb;
	dst->timestamp = src->timestamp;
	if (src->payload && dst->payload)
	memcpy(dst->payload, src->payload, src->nb);
}
/**
 void yang_frame_init(YangFrame* dst){
 if(!dst) return;
 dst->mediaType=0;
 dst->uid=0;
 dst->frametype=0;
 dst->nb=0;
 dst->timestamp=0;
 dst->payload=NULL;


 }**/

/**
YangSample::YangSample() {
	m_size = 0;
	m_bytes = NULL;

}

YangSample::YangSample(char *b, int32_t s) {
	m_size = s;
	m_bytes = b;

}

YangSample::~YangSample() {
}

YangSample* YangSample::copy() {
	YangSample *p = new YangSample();
	p->m_bytes = m_bytes;
	p->m_size = m_size;

	return p;
}
**/
YangBufferManager::YangBufferManager() {
	m_curindex = 0;
	m_size = 0;
	m_unitsize = 0;
	m_cache = NULL;
}
YangBufferManager::YangBufferManager(int32_t num, int32_t unitsize) {
	m_curindex = 0;
	m_size = 0;
	m_unitsize = 0;
	m_cache = NULL;
	init(num, unitsize);
}
YangBufferManager::~YangBufferManager() {
	yang_deleteA(m_cache);
}

void YangBufferManager::init(int32_t num, int32_t unitsize) {
	m_size = num;
	m_unitsize = unitsize;
	if (m_cache == NULL)
		m_cache = new uint8_t[m_unitsize * m_size];
}
uint8_t* YangBufferManager::getBuffer() {
	if (!m_cache)
		return NULL;
	if (m_curindex >= m_size)
		m_curindex = 0;
	return m_cache + m_unitsize * m_curindex++;
}
