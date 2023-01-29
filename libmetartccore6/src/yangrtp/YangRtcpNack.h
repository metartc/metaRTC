//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGRTP_YANGRTCPNACK_H_
#define SRC_YANGRTP_YANGRTCPNACK_H_

#include <yangrtp/YangRtp.h>
#include <yangutil/yangtype.h>
#include <yangrtp/YangRtcpCommon.h>

typedef struct YangPidBlp {
	uint16_t pid;
	uint16_t blp;
	int32_t in_use;
} YangPidBlp;

#ifdef __cplusplus
extern "C"{
#endif
void yang_create_rtcpNack(YangRtcpCommon *comm, uint32_t pssrc);
void yang_destroy_rtcpNack(YangRtcpCommon *comm);
void yang_rtcpNack_init(YangRtcpCommon *comm, uint32_t pssrc);
void yang_rtcpNack_clear(YangRtcpNack *nack);
void yang_rtcpNack_addSn(YangRtcpNack *nack, uint16_t sn);

int32_t yang_decode_rtcpNack(YangRtcpCommon *comm, YangBuffer *buffer);
int32_t yang_encode_rtcpNack(YangRtcpCommon *comm, YangBuffer *buffer);
uint64_t yang_rtcpNack_nb_bytes();

#ifdef __cplusplus
}
#endif
#endif /* SRC_YANGRTP_YANGRTCPNACK_H_ */
