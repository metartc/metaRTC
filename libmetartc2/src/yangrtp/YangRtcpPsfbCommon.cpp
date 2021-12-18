#include <yangrtp/YangRtcpPsfbCommon.h>
#include <yangutil/sys/YangLog.h>


YangRtcpPsfbCommon::YangRtcpPsfbCommon()
{
    m_header.padding = 0;
    m_header.type = YangRtcpType_psfb;
    m_header.rc = 1;
    m_header.version = kRtcpVersion;
    //ssrc_ = sender_ssrc;
}

YangRtcpPsfbCommon::~YangRtcpPsfbCommon()
{

}

uint32_t YangRtcpPsfbCommon::get_media_ssrc() const
{
    return m_media_ssrc;
}

void YangRtcpPsfbCommon::set_media_ssrc(uint32_t ssrc)
{
    m_media_ssrc = ssrc;
}

int32_t YangRtcpPsfbCommon::decode(YangBuffer *buffer)
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
    int32_t len = (m_header.length + 1) * 4 - 12;
    buffer->skip(len);
    return err;
}

uint64_t YangRtcpPsfbCommon::nb_bytes()
{
    return kRtcpPacketSize;
}

int32_t YangRtcpPsfbCommon::encode(YangBuffer *buffer)
{
    return yang_error_wrap(ERROR_RTC_RTCP, "not support");
}
