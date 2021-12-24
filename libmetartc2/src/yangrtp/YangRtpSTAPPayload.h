#ifndef SRC_YANGRTP_YANGRTPSTAPPAYLOAD_H_
#define SRC_YANGRTP_YANGRTPSTAPPAYLOAD_H_
#include <yangrtp/YangRtp.h>

// STAP-A, for multiple NALUs.
// FU-A, for one NALU with multiple fragments.
// With only one payload.
struct YangRtpSTAPData{
    int32_t nri;
    int32_t nb;
    char* payload;
    // The NALU samples, we will manage the samples.
        // @remark We only refer to the memory, user must free its bytes.
    std::vector<YangSample*>* nalus;
};

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

#endif /* SRC_YANGRTP_YANGRTPSTAPPAYLOAD_H_ */
