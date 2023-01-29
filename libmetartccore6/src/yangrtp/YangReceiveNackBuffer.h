//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGRTP_YANGRECEIVENACKBUFFER_H_
#define SRC_YANGRTP_YANGRECEIVENACKBUFFER_H_

#include <yangutil/yangtype.h>

typedef struct YangReceiveNackBuffer{
    uint16_t begin;
     uint16_t end;
	 uint16_t capacity;
	 uint16_t contexttialized;
	 int32_t* queue;
	 uint64_t nn_seq_flip_backs;
}YangReceiveNackBuffer;

#ifdef __cplusplus
extern "C"{
#endif
void yang_create_nackBuffer(YangReceiveNackBuffer* nackbuf,int32_t capacity);
void yang_destroy_nackBuffer(YangReceiveNackBuffer* nackbuf);
int32_t yang_nackbuffer_empty(YangReceiveNackBuffer* nackbuf);
void yang_nackbuffer_notify_nack_list_full(YangReceiveNackBuffer* nackbuf);
void yang_nackbuffer_notify_drop_seq(YangReceiveNackBuffer* nackbuf,uint16_t seq);
int32_t yang_nackbuffer_update(YangReceiveNackBuffer* nackbuf,uint16_t seq);
int32_t yang_nackbuffer_update2(YangReceiveNackBuffer* nackbuf,uint16_t seq, uint16_t* nack_first, uint16_t* nack_last);
void yang_nackbuffer_set(YangReceiveNackBuffer* nackbuf,uint16_t at, uint16_t pkt);
uint32_t yang_nackbuffer_get_extended_highest_sequence(YangReceiveNackBuffer* nackbuf);
#ifdef __cplusplus
}
#endif
#endif /* SRC_YANGRTP_YANGRECEIVENACKBUFFER_H_ */
