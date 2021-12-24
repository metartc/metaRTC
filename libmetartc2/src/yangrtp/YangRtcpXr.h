
#ifndef SRC_YANGRTP_YANGRTCPXR_H_
#define SRC_YANGRTP_YANGRTCPXR_H_

#include <yangrtp/YangRtcpCommon.h>

class YangRtcpXr : public YangRtcpCommon
{
public:
    YangRtcpXr (uint32_t ssrc = 0);
    virtual ~YangRtcpXr();

   // interface ISrsCodec
public:
    virtual int32_t decode(YangBuffer *buffer);
    virtual uint64_t nb_bytes();
    virtual int32_t encode(YangBuffer *buffer);
};

#endif /* SRC_YANGRTP_YANGRTCPXR_H_ */
