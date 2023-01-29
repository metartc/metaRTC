//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGRTP_YANGRTCPCOMPOUND_H_
#define SRC_YANGRTP_YANGRTCPCOMPOUND_H_

#include <yangrtp/YangRtcpCommon.h>
#include <yangutil/sys/YangVector.h>

yang_vector_declare(YangRtcpCommon)

typedef struct YangRtcpCompound{
	 int32_t nb_bytes;
	 int32_t nb_data;
	 char* data;
	 YangRtcpCommonVector rtcpVector;
}YangRtcpCompound;

#ifdef __cplusplus
extern "C"{
#endif
void yang_create_rtcpCompound(YangRtcpCompound* rtcp);
void yang_destroy_rtcpCompound(YangRtcpCompound* rtcp);
int32_t yang_decode_rtcpCompound(YangRtcpCompound* rtcp,YangBuffer *buffer);
int32_t yang_encode_rtcpCompound(YangRtcpCompound* rtcp,YangBuffer *buffer);
uint64_t yang_rtcpCompound_nb_bytes();
int32_t yang_rtcpCompound_add_rtcp(YangRtcpCompound *rtcps, YangRtcpCommon *rtcp);
void yang_rtcpCompound_clear(YangRtcpCompound *rtcps);
#ifdef __cplusplus
}
#endif
#endif /* SRC_YANGRTP_YANGRTCPCOMPOUND_H_ */
