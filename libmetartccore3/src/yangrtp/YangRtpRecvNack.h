//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGRTP_YANGRTPRECVNACK_H_
#define SRC_YANGRTP_YANGRTPRECVNACK_H_

#include <stdint.h>
#include <yangrtp/YangReceiveNackBuffer.h>
#include <yangrtp/YangRtcpNack.h>



typedef struct YangNackOption
{
    int32_t max_req_count;
    int64_t max_alive_time;
    int64_t first_nack_interval;
    int64_t nack_interval;

    int64_t max_nack_interval;
    int64_t min_nack_interval;
    int64_t nack_check_interval;
}YangNackOption;

typedef struct YangRtpNackInfo
{
    // Use to control the time of first nack req and the life of seq.
    int64_t generate_time_;
    // Use to control nack interval.
    int64_t pre_req_nack_time_;
    // Use to control nack times.
    int32_t req_nack_count_;
    uint16_t sn;

}YangRtpNackInfo;

typedef struct {
	uint32_t vsize;
	uint32_t capacity;
	YangRtpNackInfo** nackinfos;

}YangRtpNackInfoList;

typedef struct YangRtpRecvNack{
	 // Nack queue, seq order, oldest to newest.

		YangRtpNackInfoList queue;
	    // Max nack count.
	    size_t max_queue_size;

	    YangReceiveNackBuffer* rtp;
	    YangNackOption opts;
	    int64_t pre_check_time;
	    int32_t rtt;
}YangRtpRecvNack;
#ifdef __cplusplus
extern "C"{
#endif
void yang_create_nackinfo(YangRtpNackInfo* nackinfo,uint16_t seq);
void yang_nackinfo_erase(YangRtpRecvNack* nack,uint16_t seq);

void yang_create_nackinfovec(YangRtpNackInfoList* nackvec,uint32_t capacity);
void yang_destroy_nackinfovec(YangRtpNackInfoList* nackvec);

void yang_create_recvnack(YangRtpRecvNack* nack, YangReceiveNackBuffer* rtp,size_t queue_size);
void yang_destroy_recvnack(YangRtpRecvNack* nack);
void yang_recvnack_insert(YangRtpRecvNack* nack,uint16_t first, uint16_t last);
YangRtpNackInfo* yang_recvnack_find(YangRtpRecvNack* nack,uint16_t seq);
void yang_recvnack_update_rtt(YangRtpRecvNack* nack,int32_t rtt);
void yang_recvnack_remove(YangRtpRecvNack* nack,uint16_t seq);
int32_t yang_recvnack_isEmpty(YangRtpRecvNack* nack);
void yang_recvnack_get_nack_seqs(YangRtpRecvNack* recvnack,YangRtcpNack* nack, uint32_t* timeout_nacks);
void yang_recvnack_check_queue_size(YangRtpRecvNack* nack);
#ifdef __cplusplus
}
#endif
#endif /* SRC_YANGRTP_YANGRTPRECVNACK_H_ */
