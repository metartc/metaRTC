//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGRTP_YANGRTCPPSFBCOMMON_H_
#define SRC_YANGRTP_YANGRTCPPSFBCOMMON_H_

#include <yangrtp/YangRtcpCommon.h>

#ifdef __cplusplus
extern "C"{
#endif
void yang_create_rtcpPsfb(YangRtcpCommon* comm);
int32_t yang_decode_rtcpPsfb(YangRtcpCommon* comm,YangBuffer *buffer);
int32_t yang_encode_rtcpPsfb(YangRtcpCommon* comm,YangBuffer *buffer);
uint64_t yang_rtcpPsfb_nb_bytes(YangRtcpCommon* comm,YangBuffer *buffer);
#ifdef __cplusplus
}
#endif
#endif /* SRC_YANGRTP_YANGRTCPPSFBCOMMON_H_ */
