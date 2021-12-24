
#include <yangrtp/YangRtxPayloadDes.h>
#include <sstream>


YangRtxPayloadDes::YangRtxPayloadDes()
{
}

YangRtxPayloadDes::YangRtxPayloadDes(uint8_t pt, uint8_t apt):YangCodecPayload(pt, "rtx", 8000), apt_(apt)
{
}

YangRtxPayloadDes::~YangRtxPayloadDes()
{
}

YangRtxPayloadDes* YangRtxPayloadDes::copy()
{
    YangRtxPayloadDes* cp = new YangRtxPayloadDes();

    cp->m_type = m_type;
    cp->m_pt = m_pt;
    cp->m_pt_of_publisher = m_pt_of_publisher;
    cp->m_name = m_name;
    cp->m_sample = m_sample;
    cp->m_rtcp_fbs = m_rtcp_fbs;
    cp->apt_ = apt_;

    return cp;
}

YangMediaPayloadType YangRtxPayloadDes::generate_media_payload_type()
{
    YangMediaPayloadType media_payload_type(m_pt);

    media_payload_type.m_encoding_name = m_name;
    media_payload_type.m_clock_rate = m_sample;
    std::ostringstream format_specific_param;
    format_specific_param << "fmtp:" << m_pt << " apt="<< apt_;

    media_payload_type.m_format_specific_param = format_specific_param.str();

    return media_payload_type;
}
