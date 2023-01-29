//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtp/YangReceiveNackBuffer.h>
#include <yangrtp/YangRtpPacket.h>


void yang_create_nackBuffer(YangReceiveNackBuffer* nackbuf,int32_t capacity){
	if(nackbuf==NULL) return;
    nackbuf->nn_seq_flip_backs = 0;
    nackbuf->begin = nackbuf->end = 0;
    nackbuf->capacity = (uint16_t)capacity;
    nackbuf->contexttialized = yangfalse;

    nackbuf->queue = (int32_t*)yang_calloc(1,nackbuf->capacity*sizeof(int32_t));

}

void yang_destroy_nackBuffer(YangReceiveNackBuffer* nackbuf){
	if(nackbuf==NULL) return;
	yang_free(nackbuf->queue);
}

int32_t yang_nackbuffer_empty(YangReceiveNackBuffer* nackbuf)
{
    return nackbuf->begin == nackbuf->end?1:0;
}

int32_t yang_nackbuffer_size(YangReceiveNackBuffer* nackbuf)
{
    int32_t size = yang_rtp_seq_distance(nackbuf->begin, nackbuf->end);
    return size;
}

void yang_nackbuffer_advance_to(YangReceiveNackBuffer* nackbuf,uint16_t seq)
{
	if(nackbuf==NULL) return;
    nackbuf->begin = seq;
}

void yang_nackbuffer_set(YangReceiveNackBuffer* nackbuf,uint16_t at, uint16_t pkt)
{
	if(nackbuf==NULL) return;
    nackbuf->queue[at % nackbuf->capacity] = pkt;
}

void yang_nackbuffer_remove(YangReceiveNackBuffer* nackbuf,uint16_t at)
{
	if(nackbuf==NULL) return;
	 nackbuf->queue[at % nackbuf->capacity] = -1;
}

uint32_t yang_nackbuffer_get_extended_highest_sequence(YangReceiveNackBuffer* nackbuf)
{
    return nackbuf->nn_seq_flip_backs * 65536 + nackbuf->end - 1;
}

int32_t yang_nackbuffer_update(YangReceiveNackBuffer* nackbuf,uint16_t seq){
	if(nackbuf==NULL) return 1;
	if (!nackbuf->contexttialized) {
	        nackbuf->contexttialized = yangtrue;
	        nackbuf->begin = seq;
	        nackbuf->end = seq + 1;
	        return Yang_Ok;
	    }

	    if (yang_rtp_seq_distance(nackbuf->end, seq) >= 0) {

	        if (seq < nackbuf->end) {
	            nackbuf->nn_seq_flip_backs++;
	        }
	        nackbuf->end = seq + 1;


	        return Yang_Ok;
	    }

	    // Out-of-order sequence, seq before low_.
	    if (yang_rtp_seq_distance(seq, nackbuf->begin) > 0) {
	        nackbuf->begin = seq;
	        return ERROR_RTC_NACK_BUFFER;
	    }

	    return ERROR_RTC_NACK_BUFFER;

}

int32_t yang_nackbuffer_update2(YangReceiveNackBuffer* nackbuf,uint16_t seq, uint16_t* nack_first, uint16_t* nack_last)
{
	if(nackbuf==NULL) return ERROR_RTC_NACK_BUFFER;
    if (!nackbuf->contexttialized) {
        nackbuf->contexttialized = yangtrue;
        nackbuf->begin = seq;
        nackbuf->end = seq + 1;
        return Yang_Ok;
    }

    // Normal sequence, seq follows high_.
    if (yang_rtp_seq_distance(nackbuf->end, seq) >= 0) {
        *nack_first = nackbuf->end;
        *nack_last = seq;


        if (seq < nackbuf->end) {
            ++nackbuf->nn_seq_flip_backs;
        }
        nackbuf->end = seq + 1;


        return Yang_Ok;
    }

    // Out-of-order sequence, seq before low_.
    if (yang_rtp_seq_distance(seq, nackbuf->begin) > 0) {
        *nack_first = seq;
        *nack_last = nackbuf->begin;
        nackbuf->begin = seq;
        return ERROR_RTC_NACK_BUFFER;
    }

    return ERROR_RTC_NACK_BUFFER;
}

int32_t yang_nackbuffer_at(YangReceiveNackBuffer* nackbuf,uint16_t seq) {
    return nackbuf->queue[seq % nackbuf->capacity];
}

void yang_nackbuffer_clear_histroy(YangReceiveNackBuffer* nackbuf,uint16_t seq)
{

    for (uint16_t i = 0; i < nackbuf->capacity; i++) {

        if ( nackbuf->queue[i] < seq) {

            nackbuf->queue[i] = -1;
        }
    }
}

void yang_nackbuffer_clear_all_histroy(YangReceiveNackBuffer* nackbuf)
{
    for (uint16_t i = 0; i < nackbuf->capacity; i++) {

            nackbuf->queue[i] = -1;

    }
}

void yang_nackbuffer_notify_nack_list_full(YangReceiveNackBuffer* nackbuf)
{
	yang_nackbuffer_clear_all_histroy(nackbuf);

    nackbuf->begin = nackbuf->end = 0;
    nackbuf->contexttialized = yangfalse;
}

void yang_nackbuffer_notify_drop_seq(YangReceiveNackBuffer* nackbuf,uint16_t seq)
{
	yang_nackbuffer_remove(nackbuf,seq);
	yang_nackbuffer_advance_to(nackbuf,seq+1);
}

