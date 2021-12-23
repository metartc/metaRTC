#include <yangrtp/YangRtcpCommon.h>
#include <yangutil/sys/YangLog.h>
YangRtcpCommon::YangRtcpCommon(): m_ssrc(0), m_data(NULL), m_nb_data(0)
{
    m_payload_len = 0;
}

YangRtcpCommon::~YangRtcpCommon()
{
}

uint8_t YangRtcpCommon::type() const
{
    return m_header.type;
}

uint8_t YangRtcpCommon::get_rc() const
{
    return m_header.rc;
}

uint32_t YangRtcpCommon::get_ssrc()
{
    return m_ssrc;
}

void YangRtcpCommon::set_ssrc(uint32_t ssrc)
{
    m_ssrc = ssrc;
}

char* YangRtcpCommon::data()
{
    return m_data;
}

int32_t YangRtcpCommon::size()
{
    return m_nb_data;
}

int32_t YangRtcpCommon::decode_header(YangBuffer *buffer)
{
    if (!buffer->require(sizeof(YangRtcpHeader) + 4)) {
        return yang_error_wrap(ERROR_RTC_RTCP, "require %d", sizeof(YangRtcpHeader) + 4);
    }

    buffer->read_bytes((char*)(&m_header), sizeof(YangRtcpHeader));
    m_header.length = ntohs(m_header.length);

    int32_t payload_len = m_header.length * 4;
    if (payload_len > buffer->left()) {
        return yang_error_wrap(ERROR_RTC_RTCP,
                "require payload len=%u, buffer left=%u", payload_len, buffer->left());
    }
    m_ssrc = buffer->read_4bytes();

    return Yang_Ok;
}

int32_t YangRtcpCommon::encode_header(YangBuffer *buffer)
{
    if(! buffer->require(sizeof(YangRtcpHeader) + 4)) {
        return yang_error_wrap(ERROR_RTC_RTCP, "require %d", sizeof(YangRtcpHeader) + 4);
    }
    m_header.length = htons(m_header.length);
    buffer->write_bytes((char*)(&m_header), sizeof(YangRtcpHeader));
    buffer->write_4bytes(m_ssrc);

    return Yang_Ok;
}

int32_t YangRtcpCommon::decode(YangBuffer *buffer)
{
    int32_t err = Yang_Ok;
    m_data = buffer->head();
    m_nb_data = buffer->left();

    if(Yang_Ok != (err = decode_header(buffer))) {
        return yang_error_wrap(err, "decode header");
    }

    m_payload_len = (m_header.length + 1) * 4 - sizeof(YangRtcpHeader) - 4;
    buffer->read_bytes((char *)m_payload, m_payload_len);

    return err;
}

uint64_t YangRtcpCommon::nb_bytes()
{
    return sizeof(YangRtcpHeader) + 4 + m_payload_len;
}

int32_t YangRtcpCommon::encode(YangBuffer *buffer)
{
    return yang_error_wrap(ERROR_RTC_RTCP, "not implement");
}
