#ifndef SRC_YANGRTP_YANGRTCPCOMMON_H_
#define SRC_YANGRTP_YANGRTCPCOMMON_H_

#include <yangrtp/YangRtcp.h>
#include <yangutil/buffer/YangBuffer.h>
#include <yangrtp/YangRtpConstant.h>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#endif

struct YangRtcpRB
{
    uint32_t ssrc;
    uint8_t fraction_lost;
    uint32_t lost_packets;
    uint32_t highest_sn;
    uint32_t jitter;
    uint32_t lsr;
    uint32_t dlsr;

    YangRtcpRB() {
        ssrc = 0;
        fraction_lost = 0;
        lost_packets = 0;
        highest_sn = 0;
        jitter = 0;
        lsr = 0;
        dlsr = 0;
    }
};

class YangRtcpCommon
{
protected:
    YangRtcpHeader m_header;
    uint32_t m_ssrc;
    uint8_t m_payload[kRtcpPacketSize];
    int32_t m_payload_len;

    char* m_data;
    int32_t m_nb_data;
protected:
    int32_t decode_header(YangBuffer *buffer);
    int32_t encode_header(YangBuffer *buffer);
public:
    YangRtcpCommon();
    virtual ~YangRtcpCommon();
    virtual uint8_t type() const;
    virtual uint8_t get_rc() const;

    uint32_t get_ssrc();
    void set_ssrc(uint32_t ssrc);

    char* data();
    int32_t size();
// interface ISrsCodec
public:
    virtual int32_t decode(YangBuffer *buffer);
    virtual uint64_t nb_bytes();
    virtual int32_t encode(YangBuffer *buffer);
};
#endif /* SRC_YANGRTP_YANGRTCPCOMMON_H_ */
