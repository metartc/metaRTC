//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGRTP_YANGRTPSTAPPAYLOAD_H_
#define SRC_YANGRTP_YANGRTPSTAPPAYLOAD_H_

#include <yangrtp/YangRtp.h>
#include <yangutil/yangavtype.h>
#include <yangutil/sys/YangVector.h>

yang_vector_declare(YangSample)

typedef struct YangRtpSTAPData{
    int32_t nri;
    uint32_t nb;
    char* payload;
    YangSampleVector nalus;
}YangRtpSTAPData;

#ifdef __cplusplus
extern "C"
#endif

void yang_create_stap(YangRtpSTAPData* stap);
void yang_destroy_stap(YangRtpSTAPData* stap);

void yang_reset_h2645_stap(YangRtpSTAPData* data);
YangSample* yang_get_h264_stap_sps(YangRtpSTAPData* data);
YangSample* yang_get_h265_stap_sps(YangRtpSTAPData* data);
YangSample* yang_get_h264_stap_pps(YangRtpSTAPData* data);
YangSample* yang_get_h265_stap_pps(YangRtpSTAPData* data);
YangSample* yang_get_h265_stap_vps(YangRtpSTAPData* data);
int32_t yang_decode_h264_stap(YangBuffer* buf,YangRtpSTAPData* data);
int32_t yang_encode_h264_stap(YangBuffer* buf,YangRtpSTAPData* data);
int32_t yang_decode_h265_stap(YangBuffer* buf,YangRtpSTAPData* data);
int32_t yang_encode_h265_stap(YangBuffer* buf,YangRtpSTAPData* data);
#ifdef __cplusplus
}
#endif
#endif /* SRC_YANGRTP_YANGRTPSTAPPAYLOAD_H_ */
