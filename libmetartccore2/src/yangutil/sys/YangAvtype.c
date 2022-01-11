#include <yangutil/yangavctype.h>
#include <yangutil/yangtype.h>
//#include <yangutil/yangavinfotype.h>

#include <memory.h>
void yang_frame_copy_nobuffer( YangFrame *src,  YangFrame *dst) {
	if (!src || !dst)		return;
	dst->mediaType = src->mediaType;
	dst->uid = src->uid;
	dst->frametype = src->frametype;
	dst->nb = src->nb;
	dst->timestamp = src->timestamp;
}
void yang_frame_copy( YangFrame *src,  YangFrame *dst) {
	if (!src || !dst)		return;
	dst->mediaType = src->mediaType;
	dst->uid = src->uid;
	dst->frametype = src->frametype;
	dst->nb = src->nb;
	dst->timestamp = src->timestamp;
	dst->payload = src->payload;
}
void yang_frame_copy_buffer( YangFrame *src,  YangFrame *dst) {
	if (!src || !dst)		return;
	dst->mediaType = src->mediaType;
	dst->uid = src->uid;
	dst->frametype = src->frametype;
	dst->nb = src->nb;
	dst->timestamp = src->timestamp;
	if (src->payload && dst->payload)
	memcpy(dst->payload, src->payload, src->nb);
}



