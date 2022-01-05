#include <yangutil/yangavctype.h>
#include <yangutil/yangtype.h>
//#include <yangutil/yangavinfotype.h>

#include <memory.h>
void yang_frame_copy_nobuffer(struct YangFrame *src, struct YangFrame *dst) {
	if (!src || !dst)		return;
	dst->mediaType = src->mediaType;
	dst->uid = src->uid;
	dst->frametype = src->frametype;
	dst->nb = src->nb;
	dst->timestamp = src->timestamp;
}
void yang_frame_copy(struct YangFrame *src, struct YangFrame *dst) {
	if (!src || !dst)		return;
	dst->mediaType = src->mediaType;
	dst->uid = src->uid;
	dst->frametype = src->frametype;
	dst->nb = src->nb;
	dst->timestamp = src->timestamp;
	dst->payload = src->payload;
}
void yang_frame_copy_buffer(struct YangFrame *src, struct YangFrame *dst) {
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
 void yang_frame_init(struct YangFrame* dst){
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
