#include <yangrtp/YangRtcpApp.h>
#include <yangutil/sys/YangLog.h>

YangRtcpApp::YangRtcpApp()
{
    m_ssrc = 0;
    m_header.padding = 0;
    m_header.type = YangRtcpType_app;
    m_header.rc = 0;
    m_header.version = kRtcpVersion;
}

YangRtcpApp::~YangRtcpApp()
{
}

bool YangRtcpApp::is_rtcp_app(uint8_t *data, int32_t nb_data)
{
    if (!data || nb_data <12) {
        return false;
    }

    YangRtcpHeader *header = (YangRtcpHeader*)data;
    if (header->version == kRtcpVersion
            && header->type == YangRtcpType_app
            && ntohs(header->length) >= 2) {
        return true;
    }

    return false;
}

uint8_t YangRtcpApp::type() const
{
    return YangRtcpType_app;
}

uint8_t YangRtcpApp::get_subtype() const
{
    return m_header.rc;
}

string YangRtcpApp::get_name() const
{
    return string((char*)m_name, strnlen((char*)m_name, 4));
}

int32_t YangRtcpApp::get_payload(uint8_t*& payload, int& len)
{
    len = m_payload_len;
    payload = m_payload;

    return Yang_Ok;
}

int32_t YangRtcpApp::set_subtype(uint8_t type)
{
    if(31 < type) {
        return yang_error_wrap(ERROR_RTC_RTCP, "invalid type: %d", type);
    }

    m_header.rc = type;

    return Yang_Ok;
}

int32_t YangRtcpApp::set_name(std::string name)
{
    if(name.length() > 4) {
        return yang_error_wrap(ERROR_RTC_RTCP, "invalid name length %zu", name.length());
    }

    memset(m_name, 0, sizeof(m_name));
    memcpy(m_name, name.c_str(), name.length());

    return Yang_Ok;
}

int32_t YangRtcpApp::set_payload(uint8_t* payload, int32_t len)
{
    if(len > (kRtcpPacketSize - 12)) {
        return yang_error_wrap(ERROR_RTC_RTCP, "invalid payload length %d", len);
    }

    m_payload_len = (len + 3)/ 4 * 4;;
    memcpy(m_payload, payload, len);
    if (m_payload_len > len) {
        memset(&m_payload[len], 0, m_payload_len - len); //padding
    }
    m_header.length = m_payload_len/4 + 3 - 1;

    return Yang_Ok;
}

int32_t YangRtcpApp::decode(YangBuffer *buffer)
{
    /*
    @doc: https://tools.ietf.org/html/rfc3550#section-6.7
    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |V=2|P| subtype |   PT=APP=204  |             length            |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                           SSRC/CSRC                           |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                          name (ASCII)                         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                   application-dependent data                ...
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    */
    int32_t err = Yang_Ok;
    m_data = buffer->head();
    m_nb_data = buffer->left();

    if(Yang_Ok != (err = decode_header(buffer))) {
        return yang_error_wrap(err, "decode header");
    }

    if (m_header.type != YangRtcpType_app || !buffer->require(4)) {
        return yang_error_wrap(ERROR_RTC_RTCP, "not rtcp app");
    }

    buffer->read_bytes((char *)m_name, sizeof(m_name));

    // TODO: FIXME: Should check size?
    m_payload_len = (m_header.length + 1) * 4 - 8 - sizeof(m_name);
    if (m_payload_len > 0) {
        buffer->read_bytes((char *)m_payload, m_payload_len);
    }

    return Yang_Ok;
}

uint64_t YangRtcpApp::nb_bytes()
{
    return sizeof(YangRtcpHeader) + sizeof(m_ssrc) + sizeof(m_name) + m_payload_len;
}

int32_t YangRtcpApp::encode(YangBuffer *buffer)
{
    /*
    @doc: https://tools.ietf.org/html/rfc3550#section-6.7
    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |V=2|P| subtype |   PT=APP=204  |             length            |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                           SSRC/CSRC                           |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                          name (ASCII)                         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                   application-dependent data                ...
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    */
    int32_t err = Yang_Ok;

    if(!buffer->require(nb_bytes())) {
        return yang_error_wrap(ERROR_RTC_RTCP, "requires %d bytes", nb_bytes());
    }

    if(Yang_Ok != (err = encode_header(buffer))) {
        return yang_error_wrap(err, "encode header");
    }

    buffer->write_bytes((char*)m_name, sizeof(m_name));
    buffer->write_bytes((char*)m_payload, m_payload_len);

    return Yang_Ok;
}
