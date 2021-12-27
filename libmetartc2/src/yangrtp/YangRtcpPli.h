#ifndef SRC_YANGRTP_YANGRTCPPLI_H_
#define SRC_YANGRTP_YANGRTCPPLI_H_
#include <yangrtp/YangRtcpPsfbCommon.h>
class YangRtcpPli : public YangRtcpPsfbCommon
{
public:
    YangRtcpPli(uint32_t sender_ssrc = 0);
    virtual ~YangRtcpPli();


public:
    virtual int32_t decode(YangBuffer *buffer);
    virtual uint64_t nb_bytes();
    virtual int32_t encode(YangBuffer *buffer);
};

#endif /* SRC_YANGRTP_YANGRTCPPLI_H_ */
