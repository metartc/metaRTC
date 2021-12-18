#ifndef SRC_YANGRTP_YANGRTCPRR_H_
#define SRC_YANGRTP_YANGRTCPRR_H_

#include <yangrtp/YangRtcpCommon.h>
#include <yangutil/buffer/YangBuffer.h>
class YangRtcpRR : public YangRtcpCommon
{
private:
    YangRtcpRB m_rb;
public:
    YangRtcpRR(uint32_t sender_ssrc = 0);
    virtual ~YangRtcpRR();

    // overload SrsRtcpCommon
    virtual uint8_t type() const;

    uint32_t get_rb_ssrc() const;
    float get_lost_rate() const;
    uint32_t get_lost_packets() const;
    uint32_t get_highest_sn() const;
    uint32_t get_jitter() const;
    uint32_t get_lsr() const;
    uint32_t get_dlsr() const;

    void set_rb_ssrc(uint32_t ssrc);
    void set_lost_rate(float rate);
    void set_lost_packets(uint32_t count);
    void set_highest_sn(uint32_t sn);
    void set_jitter(uint32_t jitter);
    void set_lsr(uint32_t lsr);
    void set_dlsr(uint32_t dlsr);
    void set_sender_ntp(uint64_t ntp);

public:
    virtual int32_t decode(YangBuffer *buffer);
    virtual uint64_t nb_bytes();
    virtual int32_t encode(YangBuffer *buffer);

};
#endif /* SRC_YANGRTP_YANGRTCPRR_H_ */
