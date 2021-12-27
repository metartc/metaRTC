#include <yangrtp/YangRtcpSR.h>
#include <yangutil/sys/YangLog.h>


YangRtcpSR::YangRtcpSR()
{
    m_header.padding = 0;
    m_header.type = YangRtcpType_sr;
    m_header.rc = 0;
    m_header.version = kRtcpVersion;
    m_header.length = 6;

    m_ssrc = 0;
    m_ntp = 0;
    m_rtp_ts = 0;
    m_send_rtp_packets = 0;
    m_send_rtp_bytes = 0;
    m_send_rtp_bytes = 0;
}

YangRtcpSR::~YangRtcpSR()
{
}

uint8_t YangRtcpSR::get_rc() const
{
    return m_header.rc;
}

uint8_t YangRtcpSR::type() const
{
    return YangRtcpType_sr;
}

uint64_t YangRtcpSR::get_ntp() const
{
    return m_ntp;
}

uint32_t YangRtcpSR::get_rtp_ts() const
{
    return m_rtp_ts;
}

uint32_t YangRtcpSR::get_rtp_send_packets() const
{
    return m_send_rtp_packets;
}

uint32_t YangRtcpSR::get_rtp_send_bytes() const
{
    return m_send_rtp_bytes;
}

void YangRtcpSR::set_ntp(uint64_t ntp)
{
    m_ntp = ntp;
}

void YangRtcpSR::set_rtp_ts(uint32_t ts)
{
    m_rtp_ts = ts;
}

void YangRtcpSR::set_rtp_send_packets(uint32_t packets)
{
    m_send_rtp_packets = packets;
}

void YangRtcpSR::set_rtp_send_bytes(uint32_t bytes)
{
    m_send_rtp_bytes = bytes;
}

int32_t YangRtcpSR::decode(YangBuffer *buffer)
{
    /* @doc: https://tools.ietf.org/html/rfc3550#section-6.4.1
        0                   1                   2                   3
        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
header |V=2|P|    RC   |   PT=SR=200   |             length            |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                         SSRC of sender                        |
       +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
sender |              NTP timestamp, most significant word             |
info   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |             NTP timestamp, least significant word             |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                         RTP timestamp                         |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                     sender's packet count                     |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                      sender's octet count                     |
       +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
report |                 SSRC_1 (SSRC of first source)                 |
block  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  1    | fraction lost |       cumulative number of packets lost       |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |           extended highest sequence number received           |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                      interarrival jitter                      |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                         last SR (LSR)                         |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                   delay since last SR (DLSR)                  |
       +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
report |                 SSRC_2 (SSRC of second source)                |
block  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  2    :                               ...                             :
       +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
       |                  profile-specific extensions                  |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    */
    int32_t err = Yang_Ok;
    m_data = buffer->head();
    m_nb_data = buffer->left();

    if(Yang_Ok != (err = decode_header(buffer))) {
        return yang_error_wrap(err, "decode header");
    }

    m_ntp = buffer->read_8bytes();
    m_rtp_ts = buffer->read_4bytes();
    m_send_rtp_packets = buffer->read_4bytes();
    m_send_rtp_bytes = buffer->read_4bytes();

    if(m_header.rc > 0) {
        char buf[1500];
        buffer->read_bytes(buf, m_header.rc * 24);
    }

    return err;
}

uint64_t YangRtcpSR::nb_bytes()
{
    return (m_header.length + 1) * 4;
}

int32_t YangRtcpSR::encode(YangBuffer *buffer)
{
    int32_t err = Yang_Ok;
 /* @doc: https://tools.ietf.org/html/rfc3550#section-6.4.1
        0                   1                   2                   3
        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
header |V=2|P|    RC   |   PT=SR=200   |             length            |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                         SSRC of sender                        |
       +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
sender |              NTP timestamp, most significant word             |
info   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |             NTP timestamp, least significant word             |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                         RTP timestamp                         |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                     sender's packet count                     |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                      sender's octet count                     |
       +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
report |                 SSRC_1 (SSRC of first source)                 |
block  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  1    | fraction lost |       cumulative number of packets lost       |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |           extended highest sequence number received           |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                      interarrival jitter                      |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                         last SR (LSR)                         |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                   delay since last SR (DLSR)                  |
       +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
report |                 SSRC_2 (SSRC of second source)                |
block  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  2    :                               ...                             :
       +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
       |                  profile-specific extensions                  |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    */
    if(!buffer->require(nb_bytes())) {
        return yang_error_wrap(ERROR_RTC_RTCP, "requires %d bytes", nb_bytes());
    }

    if(Yang_Ok != (err = encode_header(buffer))) {
        return yang_error_wrap(err, "encode header");
    }

    buffer->write_8bytes(m_ntp);
    buffer->write_4bytes(m_rtp_ts);
    buffer->write_4bytes(m_send_rtp_packets);
    buffer->write_4bytes(m_send_rtp_bytes);

    return err;
}

