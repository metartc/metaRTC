#include <yangrtp/YangRedPayload.h>
#include <yangutil/sys/YangString.h>

YangRedPayload::YangRedPayload()
{
    m_channel = 0;
}

YangRedPayload::YangRedPayload(uint8_t pt, std::string encode_name, int32_t sample, int32_t channel)
    :YangCodecPayload(pt, encode_name, sample)
{
    m_channel = channel;
}

YangRedPayload::~YangRedPayload()
{
}

YangRedPayload* YangRedPayload::copy()
{
    YangRedPayload* cp = new YangRedPayload();

    cp->m_type = m_type;
    cp->m_pt = m_pt;
    cp->m_pt_of_publisher = m_pt_of_publisher;
    cp->m_name = m_name;
    cp->m_sample = m_sample;
    cp->m_rtcp_fbs = m_rtcp_fbs;
    cp->m_channel = m_channel;

    return cp;
}

YangMediaPayloadType YangRedPayload::generate_media_payload_type()
{
    YangMediaPayloadType media_payload_type(m_pt);

    media_payload_type.m_encoding_name = m_name;
    media_payload_type.m_clock_rate = m_sample;
    if (m_channel != 0) {
        media_payload_type.m_encoding_param = yang_int2str(m_channel);
    }
    media_payload_type.m_rtcp_fb = m_rtcp_fbs;

    return media_payload_type;
}

