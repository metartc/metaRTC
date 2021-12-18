#include <yangrtp/YangCodecPayload.h>



YangCodecPayload::YangCodecPayload()
{
    m_pt_of_publisher = m_pt = 0;
    m_sample = 0;
}

YangCodecPayload::YangCodecPayload(uint8_t pt, std::string encode_name, int32_t sample)
{
    m_pt_of_publisher = m_pt = pt;
    m_name = encode_name;
    m_sample = sample;
}

YangCodecPayload::~YangCodecPayload()
{
}

YangCodecPayload* YangCodecPayload::copy()
{
    YangCodecPayload* cp = new YangCodecPayload();

    cp->m_type = m_type;
    cp->m_pt = m_pt;
    cp->m_pt_of_publisher = m_pt_of_publisher;
    cp->m_name = m_name;
    cp->m_sample = m_sample;
    cp->m_rtcp_fbs = m_rtcp_fbs;

    return cp;
}

YangMediaPayloadType YangCodecPayload::generate_media_payload_type()
{
    YangMediaPayloadType media_payload_type(m_pt);

    media_payload_type.m_encoding_name = m_name;
    media_payload_type.m_clock_rate = m_sample;
    media_payload_type.m_rtcp_fb = m_rtcp_fbs;

    return media_payload_type;
}
