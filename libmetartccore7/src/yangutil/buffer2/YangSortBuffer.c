//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangutil/buffer2/YangSortBuffer.h>
#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangMath.h>



int yang_get_sortBuffer_index(YangSortFrame** a, int64_t value,int32_t alen){
	int low,high,mid;
	low = 0;
	high =alen;
	while(low<=high){
		mid = (high+low)/2;
		if(value==a[mid]->pts) return -1;
		if(a[mid]->pts>value)
			high = mid-1;
		else if(a[mid]->pts>value)
			low = mid+1;
		else
			return mid;
	}
	return low;
}

int32_t yang_sortBuffer_insert(YangSortFrame** vec,int64_t value,YangFrame* pframe,uint32_t* vsize){
	if(vec==NULL) return 1;
	int32_t alen=*vsize;
	int32_t index=-1;

	if(alen==0){
		vec[0]->pts=value;
		yang_frame_copy_buffer(pframe, &vec[0]->frame);
		*vsize=1;
		return Yang_Ok;
	}

	if (value>vec[alen-1]->pts){
		vec[alen]->pts=value;
		yang_frame_copy_buffer(pframe, &vec[alen]->frame);
		*vsize=alen+1;
		return Yang_Ok;
	}

	index=yang_get_sortBuffer_index(vec,value,alen);
	if(index==-1) return 1;
	yang_memmove((char*) vec + (index + 1) * sizeof(YangSortFrame*),
				(char*) vec + index * sizeof(YangSortFrame*),
				sizeof(YangSortFrame*) * (alen - index));
	vec[index]->pts=value;
	yang_frame_copy_buffer(pframe, &vec[index]->frame);
	*vsize=alen+1;

	return Yang_Ok;
}


void yang_sortBuffer_initFrames(YangSortBufferSession* session,int32_t pnum, int unitsize) {
    session->capacity=pnum;
	if (session->bufferManager == NULL)		session->bufferManager = (uint8_t*)yang_calloc(unitsize ,pnum);
	if (session->frames == NULL) {
		session->frames =(YangSortFrame**) yang_calloc(sizeof(YangSortFrame*),pnum);
		for (int32_t i = 0; i < pnum; i++) {
			session->frames[i] = (YangSortFrame*)yang_calloc(sizeof(YangSortFrame),1);
			session->frames[i]->frame.payload = session->bufferManager+i*unitsize;
		}
	}
}

void yang_sortBuffer_putFrame(YangSortBufferSession* session,int64_t pts,YangFrame *pframe) {
	if (!pframe||session->vsize>=session->capacity)		return;
	if(pts<=session->lastPts) return;
	yang_sortBuffer_insert(session->frames,pts,pframe,&session->vsize);

}



void yang_sortBuffer_removeFirst(YangSortBufferSession* session){
    if (session->vsize-session->cacheNum<1 || session->vsize<1)				return ;
	session->lastPts=session->frames[0]->pts;
	yang_memmove((char*)session->frames,(char*)session->frames+sizeof(YangSortFrame*),sizeof(YangSortFrame*)*(session->vsize-1));
	session->vsize--;
}

YangFrame* yang_sortBuffer_getCurFrameRef(YangSortBufferSession* session) {
	if(session->vsize-session->cacheNum<1) return NULL;

    return &session->frames[0]->frame;
}



void yang_create_sortBuffer(YangSortBuffer* buf,int cacheNum){
	if(buf==NULL) return;
	YangSortBufferSession* session=&buf->session;

	session->capacity = 5;
	session->cacheNum=cacheNum;
	session->vsize=0;
	session->frames = NULL;
	session->bufferManager = NULL;

	buf->initFrames=yang_sortBuffer_initFrames;
	buf->getCurFrameRef=yang_sortBuffer_getCurFrameRef;

	buf->putFrame=yang_sortBuffer_putFrame;

	buf->removeFirst=yang_sortBuffer_removeFirst;
}
void yang_destroy_sortBuffer(YangSortBuffer* buf){
	if(buf==NULL) return;
	YangSortBufferSession* session=&buf->session;
	if (session->frames) {
        for (uint32_t i = 0; i < session->capacity; i++) {
			yang_free(session->frames[i]);
		}
		yang_free(session->frames);
	}
	yang_free(session->bufferManager);

}
