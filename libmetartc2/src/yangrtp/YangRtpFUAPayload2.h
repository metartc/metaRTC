#ifndef SRC_YANGRTP_YANGRTPFUAPAYLOAD2_H_
#define SRC_YANGRTP_YANGRTPFUAPAYLOAD2_H_
#include <yangrtp/YangRtp.h>
#include <yangutil/yangavtype_h265.h>
struct Fua2H264DataCache{
    bool start;
    bool end;
    YangAvcNaluType nri;
    YangAvcNaluType nalu_type;
};
struct Fua2H265DataCache{
    bool start;
    bool end;
    YangHevcNaluType nri;
    YangHevcNaluType nalu_type;
};
struct YangFua2H264Data{
    // The FUA header.
    bool start;
    bool end;
    YangAvcNaluType nri;
    YangAvcNaluType nalu_type;
    int32_t nb;
    char* payload;
};
struct YangFua2H265Data{
    // The FUA header.
    bool start;
    bool end;
    YangHevcNaluType nri;
    YangHevcNaluType nalu_type;
    int32_t nb;
    char* payload;
};
int32_t yang_decode_h264_fua2(YangBuffer* buf,Fua2H264DataCache* pkt);
int32_t yang_decode_h265_fua2(YangBuffer* buf,Fua2H265DataCache* pkt);
// FU-A, for one NALU with multiple fragments.
// With only one payload.
int32_t yang_encode_h264_fua2(YangBuffer* buf,YangFua2H264Data* pkt);
int32_t yang_encode_h265_fua2(YangBuffer* buf,YangFua2H265Data* pkt);


#endif /* SRC_YANGRTP_YANGRTPFUAPAYLOAD2_H_ */
