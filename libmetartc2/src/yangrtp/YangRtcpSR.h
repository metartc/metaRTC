#ifndef SRC_YANGRTP_YANGRTCPSR_H_
#define SRC_YANGRTP_YANGRTCPSR_H_

#include <yangrtp/YangRtcpCommon.h>



class YangRtcpSR : public YangRtcpCommon
{
private:
    uint64_t m_ntp;
    uint32_t m_rtp_ts;
    uint32_t m_send_rtp_packets;
    uint32_t m_send_rtp_bytes;

public:
    YangRtcpSR();
    virtual ~YangRtcpSR();

    uint8_t get_rc() const;
    // overload SrsRtcpCommon
    virtual uint8_t type() const;
    uint64_t get_ntp() const;
    uint32_t get_rtp_ts() const;
    uint32_t get_rtp_send_packets() const;
    uint32_t get_rtp_send_bytes() const;

    void set_ntp(uint64_t ntp);
    void set_rtp_ts(uint32_t ts);
    void set_rtp_send_packets(uint32_t packets);
    void set_rtp_send_bytes(uint32_t bytes);

public:
    virtual int32_t decode(YangBuffer *buffer);
    virtual uint64_t nb_bytes();
    virtual int32_t encode(YangBuffer *buffer);
};

#endif /* SRC_YANGRTP_YANGRTCPSR_H_ */
