#include <yangrtp/YangRtcpXr.h>
#include <yangutil/sys/YangLog.h>



YangRtcpXr::YangRtcpXr(uint32_t ssrc/*= 0*/)
{
    m_header.padding = 0;
    m_header.type = YangRtcpType_xr;
    m_header.rc = 0;
    m_header.version = kRtcpVersion;
    m_ssrc = ssrc;
}

YangRtcpXr::~YangRtcpXr()
{
}

int32_t YangRtcpXr::decode(YangBuffer *buffer)
{
/*
    @doc: https://tools.ietf.org/html/rfc3611#section-2
    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |V=2|P|reserved |   PT=XR=207   |             length            |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                              SSRC                             |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   :                         report blocks                         :
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   */

    int32_t err = Yang_Ok;
    m_data = buffer->head();
    m_nb_data = buffer->left();

    if(Yang_Ok != (err = decode_header(buffer))) {
        return yang_error_wrap(err, "decode header");
    }

    int32_t len = (m_header.length + 1) * 4 - 8;
    buffer->skip(len);
    return err;
}

uint64_t YangRtcpXr::nb_bytes()
{
    return kRtcpPacketSize;
}

int32_t YangRtcpXr::encode(YangBuffer *buffer)
{
    return yang_error_wrap(ERROR_RTC_RTCP, "not support");
}
