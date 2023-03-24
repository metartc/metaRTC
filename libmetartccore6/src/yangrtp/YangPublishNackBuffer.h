//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGRTP_YANGPUBLISHNACKBUFFER_H_
#define SRC_YANGRTP_YANGPUBLISHNACKBUFFER_H_

#include <yangutil/yangtype.h>

typedef struct YangSendNackBuffer{
	uint16_t seq;
	int nb;
	char* payload;
}YangSendNackBuffer;

typedef struct YangPublishNackBuffer{
	 uint32_t capacityIndex;
	 uint32_t capacity;
	 YangSendNackBuffer* queue;
}YangPublishNackBuffer;

#define Yang_Video_Publish_NackBuffer_Count 1024
#define Yang_AUDIO_Publish_NackBuffer_Count 128


#ifdef __cplusplus
extern "C"{
#endif
void yang_create_pubNackbuffer(YangPublishNackBuffer* nackbuf,int32_t capacity);
void yang_destroy_pubNackbuffer(YangPublishNackBuffer* nackbuf);
void yang_pubnackbuffer_set(YangPublishNackBuffer* nackbuf,uint16_t at, char* data,int plen);
YangSendNackBuffer* yang_pubnackbuffer_at(YangPublishNackBuffer* nackbuf,uint16_t seq);
#ifdef __cplusplus
}
#endif
#endif /* SRC_YANGRTP_YANGPUBLISHNACKBUFFER_H_ */
