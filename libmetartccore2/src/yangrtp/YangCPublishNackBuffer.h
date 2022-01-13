/*
 * YangCPublishNackBuffer.h
 *
 *  Created on: 2022年1月3日
 *      Author: yang
 */

#ifndef SRC_YANGRTP_YANGCPUBLISHNACKBUFFER_H_
#define SRC_YANGRTP_YANGCPUBLISHNACKBUFFER_H_

#include <stdint.h>

typedef struct YangSendNackBuffer{
	uint16_t seq;
	int nb;
	char* payload;

}YangSendNackBuffer;
typedef struct YangPublishNackBuffer{
	 uint32_t capacity;
	 uint32_t vlen;
		    // Ring bufer.
	  YangSendNackBuffer** queue;
}YangPublishNackBuffer;
void yang_init_pubNackbuffer(YangPublishNackBuffer* nackbuf,int32_t capacity);
void yang_destroy_pubNackbuffer(YangPublishNackBuffer* nackbuf);
void yang_pubnackbuffer_set(YangPublishNackBuffer* nackbuf,uint16_t at, char* data,int plen);
YangSendNackBuffer* yang_pubnackbuffer_at(YangPublishNackBuffer* nackbuf,uint16_t seq);



#endif /* SRC_YANGRTP_YANGCPUBLISHNACKBUFFER_H_ */
