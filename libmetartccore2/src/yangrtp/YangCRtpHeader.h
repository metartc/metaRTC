/*
 * YangCRtpHeader.h
 *
 *  Created on: 2021年12月29日
 *      Author: yang
 */

#ifndef SRC_YANGRTP_YANGCRTPHEADER_H_
#define SRC_YANGRTP_YANGCRTPHEADER_H_


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

int32_t yang_encode_rtpHeader(YangBuffer* buf,YangRtpHeader* header);
int32_t yang_decode_rtpHeader(YangBuffer* buf,YangRtpHeader* header);


#endif /* SRC_YANGRTP_YANGCRTPHEADER_H_ */
