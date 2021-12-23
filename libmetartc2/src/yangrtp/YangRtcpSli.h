#ifndef SRC_YANGRTP_YANGRTCPSLI_H_
#define SRC_YANGRTP_YANGRTCPSLI_H_

#include <yangrtp/YangRtcpPsfbCommon.h>
#include <yangutil/buffer/YangBuffer.h>
class YangRtcpSli : public YangRtcpPsfbCommon
{
private:
    uint16_t m_first;
    uint16_t m_number;
    uint8_t m_picture;
public:
    YangRtcpSli(uint32_t sender_ssrc = 0);
    virtual ~YangRtcpSli();

 // interface ISrsCodec
public:
    virtual int32_t decode(YangBuffer *buffer);
    virtual uint64_t nb_bytes();
    virtual int32_t encode(YangBuffer *buffer);
};


#endif /* SRC_YANGRTP_YANGRTCPSLI_H_ */
