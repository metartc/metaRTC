//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGRTP_YANGRTPHEADER_H_
#define SRC_YANGRTP_YANGRTPHEADER_H_

#include <yangrtp/YangRtp.h>

typedef struct YangRtpHeader{
    uint8_t padding_length;
    uint8_t cc;
    uint8_t payload_type;
    uint8_t version;
    uint16_t sequence;
    uint32_t timestamp;
    uint32_t ssrc;
    int32_t marker;
    int32_t ignore_padding;
    YangRtpExtensions* extensions;
    uint32_t csrc[15];
}YangRtpHeader;
#ifdef __cplusplus
extern "C"{
#endif
int32_t yang_encode_rtpHeader(YangBuffer* buf,YangRtpHeader* header);
int32_t yang_decode_rtpHeader(YangBuffer* buf,YangRtpHeader* header);
#ifdef __cplusplus
}
#endif

#endif /* SRC_YANGRTP_YANGRTPHEADER_H_ */
