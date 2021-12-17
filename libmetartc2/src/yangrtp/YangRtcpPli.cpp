#include <yangrtp/YangRtcpPli.h>
#include <yangutil/sys/YangLog.h>


YangRtcpPli::YangRtcpPli(uint32_t sender_ssrc/*= 0*/)
{
    m_header.padding = 0;
    m_header.type = YangRtcpType_psfb;
    m_header.rc = kPLI;
    m_header.version = kRtcpVersion;
    m_ssrc = sender_ssrc;
}

YangRtcpPli::~YangRtcpPli()
{
}

int32_t YangRtcpPli::decode(YangBuffer *buffer)
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
   */

    int32_t err = Yang_Ok;
    m_data = buffer->head();
    m_nb_data = buffer->left();

    if(Yang_Ok != (err = decode_header(buffer))) {
        return yang_error_wrap(err, "decode header");
    }

    m_media_ssrc = buffer->read_4bytes();
    return err;
}

uint64_t YangRtcpPli::nb_bytes()
{
    return 12;
}

int32_t YangRtcpPli::encode(YangBuffer *buffer)
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
    */
    int32_t err = Yang_Ok;
    if(!buffer->require(nb_bytes())) {
        return yang_error_wrap(ERROR_RTC_RTCP, "requires %d bytes", nb_bytes());
    }

    m_header.length = 2;
    if(Yang_Ok != (err = encode_header(buffer))) {
        return yang_error_wrap(err, "encode header");
    }

    buffer->write_4bytes(m_media_ssrc);

    return err;
}
