//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtp/YangPublishNackBuffer.h>
#include <yangutil/sys/YangLog.h>


void yang_create_pubNackbuffer(YangPublishNackBuffer* nackbuf,int32_t capacity){
	if(nackbuf==NULL) return;
	nackbuf->capacity = capacity;
	nackbuf->capacityIndex=capacity-1;
	nackbuf->queue =(YangSendNackBuffer*) yang_calloc(1,sizeof(YangSendNackBuffer)*nackbuf->capacity);

}

void yang_destroy_pubNackbuffer(YangPublishNackBuffer* nackbuf){
	if(nackbuf==NULL) return;
	yang_free(nackbuf->queue);
}


YangSendNackBuffer* yang_pubnackbuffer_at(YangPublishNackBuffer* nackbuf,uint16_t seq) {
	if(nackbuf==NULL) return NULL;
    return &nackbuf->queue[seq&nackbuf->capacityIndex];
}

void yang_pubnackbuffer_set(YangPublishNackBuffer* nackbuf,uint16_t at, char* data,int plen)
{
	if(nackbuf==NULL||data==NULL) return;

	YangSendNackBuffer *p1=&nackbuf->queue[at&nackbuf->capacityIndex];
	p1->payload=data;
	p1->nb=plen;
	p1->seq=at;
}

void yang_pubnackbuffer_remove(YangPublishNackBuffer* nackbuf,uint16_t at)
{
	if(nackbuf==NULL) return;
	yang_pubnackbuffer_set(nackbuf,at, NULL,0);
}

