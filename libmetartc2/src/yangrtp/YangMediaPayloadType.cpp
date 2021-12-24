#include <yangrtp/YangMediaPayloadType.h>
#include <yangutil/yangtype.h>
#include <sstream>
#define kCRLF  "\r\n"
YangMediaPayloadType::YangMediaPayloadType(int32_t payload_type)
{
    m_payload_type = payload_type;
    m_clock_rate = 0;
}

YangMediaPayloadType::~YangMediaPayloadType()
{
}

int32_t YangMediaPayloadType::encode(std::stringstream& os)
{
    int32_t err = Yang_Ok;

    os << "a=rtpmap:" << m_payload_type << " " << m_encoding_name << "/" << m_clock_rate;
    if (!m_encoding_param.empty()) {
        os << "/" << m_encoding_param;
    }
    os << kCRLF;

    for (std::vector<std::string>::iterator iter = m_rtcp_fb.begin(); iter != m_rtcp_fb.end(); ++iter) {
        os << "a=rtcp-fb:" << m_payload_type << " " << *iter << kCRLF;
    }

    if (!m_format_specific_param.empty()) {
        os << "a=fmtp:" << m_payload_type << " " << m_format_specific_param
           // TODO: FIXME: Remove the test code bellow.
           // << ";x-google-max-bitrate=6000;x-google-min-bitrate=5100;x-google-start-bitrate=5000"
           << kCRLF;
    }

    return err;
}

