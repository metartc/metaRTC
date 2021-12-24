#include <yangrtp/YangRtcpRpsi.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangutil/sys/YangLog.h>

YangRtcpRpsi::YangRtcpRpsi(uint32_t sender_ssrc/* = 0*/)
{
    m_pb = 0;
    m_payload_type = 0;
    m_native_rpsi = NULL;
    m_nb_native_rpsi = 0;

    m_header.padding = 0;
    m_header.type = YangRtcpType_psfb;
    m_header.rc = kRPSI;
    m_header.version = kRtcpVersion;
    m_ssrc = sender_ssrc;
}

YangRtcpRpsi::~YangRtcpRpsi()
{
}

int32_t YangRtcpRpsi::decode(YangBuffer *buffer)
{
/*
    @doc: https://tools.ietf.org/html/rfc4585#section-6.1
        0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |V=2|P|   FMT   |       PT      |          length               |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                  SSRC of packet sender                        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                  SSRC of media source                         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   :            Feedback Control Information (FCI)                 :
   :                                                               :


    @doc: https://tools.ietf.org/html/rfc4585#section-6.3.3
    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |      PB       |0| Payload Type|    Native RPSI bit string     |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |   defined per codec          ...                | Padding (0) |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   */

    int32_t err = Yang_Ok;
    m_data = buffer->head();
    m_nb_data = buffer->left();

    if(Yang_Ok != (err = decode_header(buffer))) {
        return yang_error_wrap(err, "decode header");
    }

    m_media_ssrc = buffer->read_4bytes();
    int32_t len = (m_header.length + 1) * 4 - 12;
    buffer->skip(len);
    return err;
}

uint64_t YangRtcpRpsi::nb_bytes()
{
    return kRtcpPacketSize;
}

int32_t YangRtcpRpsi::encode(YangBuffer *buffer)
{
    int32_t err = Yang_Ok;

    return err;
}
