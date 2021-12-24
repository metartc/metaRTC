
#ifndef SRC_YANGRTP_YANGRTCPRPSI_H_
#define SRC_YANGRTP_YANGRTCPRPSI_H_

#include <yangrtp/YangRtcpPsfbCommon.h>
#include <yangutil/buffer/YangBuffer.h>

class YangRtcpRpsi : public YangRtcpPsfbCommon
{
private:
    uint8_t m_pb;
    uint8_t m_payload_type;
    char* m_native_rpsi;
    int32_t m_nb_native_rpsi;

public:
    YangRtcpRpsi(uint32_t sender_ssrc = 0);
    virtual ~YangRtcpRpsi();

 // interface ISrsCodec
public:
    virtual int32_t decode(YangBuffer *buffer);
    virtual uint64_t nb_bytes();
    virtual int32_t encode(YangBuffer *buffer);
};

#endif /* SRC_YANGRTP_YANGRTCPRPSI_H_ */
