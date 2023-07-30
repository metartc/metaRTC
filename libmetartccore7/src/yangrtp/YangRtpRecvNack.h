//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGRTP_YANGRTPRECVNACK_H_
#define SRC_YANGRTP_YANGRTPRECVNACK_H_


#include <yangrtp/YangReceiveNackBuffer.h>
#include <yangrtp/YangRtcpNack.h>



typedef struct YangNackOption
{
    int32_t maxReqCount;
    int64_t maxAliveTime;
    int64_t firstNackInterval;
    int64_t nackInterval;

    int64_t maxNackInterval;
    int64_t minNackInterval;
    int64_t nackCheckInterval;
}YangNackOption;

typedef struct YangRtpNackInfo
{
	 uint16_t sn;
    int64_t generateTime;
    // control nack interval.
    int64_t preReqNackTime;
    // control nack times.
    int32_t reqNackCount;

}YangRtpNackInfo;

typedef struct {
	uint32_t vsize;
	uint32_t capacity;
	YangRtpNackInfo* nackinfos;

}YangRtpNackInfoList;

typedef struct YangRtpRecvNack{
	int32_t rtt;
	int64_t preCheckTime;
	size_t maxQueueSize;		      // Max nack count.
	YangReceiveNackBuffer* rtp;
	YangNackOption opts;
	YangRtcpCommon rtcp;
	YangRtpNackInfoList queue;
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
void yang_recvnack_initvec(YangRtpRecvNack* nack,uint32_t ssrc);
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
