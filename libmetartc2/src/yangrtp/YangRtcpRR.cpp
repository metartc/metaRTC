#include <yangrtp/YangRtcpRR.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangutil/sys/YangLog.h>


YangRtcpRR::YangRtcpRR(uint32_t sender_ssrc)
{
    m_header.padding = 0;
    m_header.type = YangRtcpType_rr;
    m_header.rc = 0;
    m_header.version = kRtcpVersion;
    m_header.length = 7;
    m_ssrc = sender_ssrc;
    // TODO: FIXME: Fix warning.
   // memset(&m_rb, 0, sizeof(YangRtcpRB));
}
YangRtcpRR::~YangRtcpRR()
{
}

uint8_t YangRtcpRR::type() const
{
    return YangRtcpType_rr;
}

uint32_t YangRtcpRR::get_rb_ssrc() const
{
    return m_rb.ssrc;
}

float YangRtcpRR::get_lost_rate() const
{
    return m_rb.fraction_lost / 256;
}

uint32_t YangRtcpRR::get_lost_packets() const
{
    return m_rb.lost_packets;
}

uint32_t YangRtcpRR::get_highest_sn() const
{
    return m_rb.highest_sn;
}

uint32_t YangRtcpRR::get_jitter() const
{
    return m_rb.jitter;
}

uint32_t YangRtcpRR::get_lsr() const
{
    return m_rb.lsr;
}

uint32_t YangRtcpRR::get_dlsr() const
{
    return m_rb.dlsr;
}

void YangRtcpRR::set_rb_ssrc(uint32_t ssrc)
{
    m_rb.ssrc = ssrc;
}

void YangRtcpRR::set_lost_rate(float rate)
{
    m_rb.fraction_lost = rate * 256;
}

void YangRtcpRR::set_lost_packets(uint32_t count)
{
    m_rb.lost_packets = count;
}

void YangRtcpRR::set_highest_sn(uint32_t sn)
{
    m_rb.highest_sn = sn;
}

void YangRtcpRR::set_jitter(uint32_t jitter)
{
    m_rb.jitter = jitter;
}

void YangRtcpRR::set_lsr(uint32_t lsr)
{
    m_rb.lsr = lsr;
}

void YangRtcpRR::set_dlsr(uint32_t dlsr)
{
    m_rb.dlsr = dlsr;
}

void YangRtcpRR::set_sender_ntp(uint64_t ntp)
{
    uint32_t lsr = (uint32_t)((ntp >> 16) & 0x00000000FFFFFFFF);
    m_rb.lsr = lsr;
}

int32_t YangRtcpRR::decode(YangBuffer *buffer)
{
    /*
    @doc: https://tools.ietf.org/html/rfc3550#section-6.4.2

        0                   1                   2                   3
        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
header |V=2|P|    RC   |   PT=RR=201   |             length            |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                     SSRC of packet sender                     |
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

    // @doc https://tools.ietf.org/html/rfc3550#section-6.4.2
    // An empty RR packet (RC = 0) MUST be put at the head of a compound
    // RTCP packet when there is no data transmission or reception to
    // report. e.g. {80 c9 00 01 00 00 00 01}
    if(m_header.rc == 0) {
        return yang_error_wrap(ERROR_RTC_RTCP_EMPTY_RR, "rc=0");
    }

    // TODO: FIXME: Security check for read.
    m_rb.ssrc = buffer->read_4bytes();
    m_rb.fraction_lost = buffer->read_1bytes();
    m_rb.lost_packets = buffer->read_3bytes();
    m_rb.highest_sn = buffer->read_4bytes();
    m_rb.jitter = buffer->read_4bytes();
    m_rb.lsr = buffer->read_4bytes();
    m_rb.dlsr = buffer->read_4bytes();

    // TODO: FIXME: Security check for read.
    if(m_header.rc > 1) {
        char buf[1500];
        buffer->read_bytes(buf, (m_header.rc -1 ) * 24);
    }

    return err;
}

uint64_t YangRtcpRR::nb_bytes()
{
    return (m_header.length + 1) * 4;
}

int32_t YangRtcpRR::encode(YangBuffer *buffer)
{
    /*
    @doc: https://tools.ietf.org/html/rfc3550#section-6.4.2

        0                   1                   2                   3
        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
header |V=2|P|    RC   |   PT=RR=201   |             length            |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                     SSRC of packet sender                     |
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

    if(!buffer->require(nb_bytes())) {
        return yang_error_wrap(ERROR_RTC_RTCP, "requires %d bytes", nb_bytes());
    }

    m_header.rc = 1;
    if(Yang_Ok != (err = encode_header(buffer))) {
        return yang_error_wrap(err, "encode header");
    }

    buffer->write_4bytes(m_rb.ssrc);
    buffer->write_1bytes(m_rb.fraction_lost);
    buffer->write_3bytes(m_rb.lost_packets);
    buffer->write_4bytes(m_rb.highest_sn);
    buffer->write_4bytes(m_rb.jitter);
    buffer->write_4bytes(m_rb.lsr);
    buffer->write_4bytes(m_rb.dlsr);

    return err;
}
