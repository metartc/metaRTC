/*
 * YangCRtpSTAPPayload.h
 *
 *  Created on: 2021年12月29日
 *      Author: yang
 */

#ifndef SRC_YANGRTP_YANGCRTPSTAPPAYLOAD_H_
#define SRC_YANGRTP_YANGCRTPSTAPPAYLOAD_H_

#include <yangrtp/YangRtp.h>
#include <yangutil/yangavtype.h>
// STAP-A, for multiple NALUs.
// FU-A, for one NALU with multiple fragments.
// With only one payload.
typedef struct YangSampleVector{
	uint32_t vlen;
	uint32_t allLen;
	char* samples;
}YangSampleVector;

typedef struct YangRtpSTAPData{
    int32_t nri;
    uint32_t nb;
    char* payload;
    // The NALU samples, we will manage the samples.
        // @remark We only refer to the memory, user must free its bytes.
    YangSampleVector nalus;
}YangRtpSTAPData;
void yang_inti_sampleVector(YangSampleVector* vec);
void yang_destroy_sampleVector(YangSampleVector* vec);
void yang_push_sampleVector(YangSampleVector* vec,YangSample* sample);
YangSample* yang_get_sampleVector(YangSampleVector* vec,int32_t index);
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



#endif /* SRC_YANGRTP_YANGCRTPSTAPPAYLOAD_H_ */
