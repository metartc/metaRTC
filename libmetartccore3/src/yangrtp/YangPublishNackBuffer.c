//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtp/YangPublishNackBuffer.h>
#include <yangutil/sys/YangLog.h>
#include <string.h>
#include <stdlib.h>

void yang_create_pubNackbuffer(YangPublishNackBuffer* nackbuf,int32_t capacity){
	if(nackbuf==NULL) return;
	nackbuf->capacity = capacity;
	nackbuf->queue =(YangSendNackBuffer**) calloc(1,sizeof(YangSendNackBuffer*)*nackbuf->capacity);

}
void yang_destroy_pubNackbuffer(YangPublishNackBuffer* nackbuf){
	if(nackbuf==NULL) return;
	   for (int32_t i = 0; i < nackbuf->capacity; ++i) {
	    	YangSendNackBuffer* pkt = nackbuf->queue[i];
	        yang_free(pkt);
	    }

	yang_free(nackbuf->queue);
}


YangSendNackBuffer* yang_pubnackbuffer_at(YangPublishNackBuffer* nackbuf,uint16_t seq) {
	if(nackbuf==NULL) return NULL;
    return nackbuf->queue[seq % nackbuf->capacity];
}

void yang_pubnackbuffer_set(YangPublishNackBuffer* nackbuf,uint16_t at, char* data,int plen)
{
	if(nackbuf==NULL) return;

    yang_free(nackbuf->queue[at % nackbuf->capacity]);
    if(data){
		YangSendNackBuffer *p1=(YangSendNackBuffer*)calloc(1,sizeof(YangSendNackBuffer));//new YangSendNackBuffer();
		p1->payload=data;
		p1->nb=plen;
		p1->seq=at;
		nackbuf->queue[at % nackbuf->capacity] = p1;
    }else{
    	nackbuf->queue[at % nackbuf->capacity]=NULL;
    }
}

void yang_pubnackbuffer_remove(YangPublishNackBuffer* nackbuf,uint16_t at)
{
	if(nackbuf==NULL) return;
	yang_pubnackbuffer_set(nackbuf,at, NULL,0);
}

