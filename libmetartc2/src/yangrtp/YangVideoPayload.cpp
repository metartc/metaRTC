#include <yangrtp/YangVideoPayload.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangString.h>
#include <sstream>
YangVideoPayload::YangVideoPayload()
{
    m_type = "video";
}

YangVideoPayload::YangVideoPayload(uint8_t pt, std::string encode_name, int32_t sample)
    :YangCodecPayload(pt, encode_name, sample)
{
    m_type = "video";
    m_h264_param.profile_level_id = "";
    m_h264_param.packetization_mode = "";
    m_h264_param.level_asymmerty_allow = "";
}

YangVideoPayload::~YangVideoPayload()
{
}

YangVideoPayload* YangVideoPayload::copy()
{
    YangVideoPayload* cp = new YangVideoPayload();

    cp->m_type = m_type;
    cp->m_pt = m_pt;
    cp->m_pt_of_publisher = m_pt_of_publisher;
    cp->m_name = m_name;
    cp->m_sample = m_sample;
    cp->m_rtcp_fbs = m_rtcp_fbs;
    cp->m_h264_param = m_h264_param;

    return cp;
}


YangMediaPayloadType YangVideoPayload::generate_media_payload_type()
{
    YangMediaPayloadType media_payload_type(m_pt);

    media_payload_type.m_encoding_name = m_name;
    media_payload_type.m_clock_rate = m_sample;
    media_payload_type.m_rtcp_fb = m_rtcp_fbs;

    std::ostringstream format_specific_param;
    if (!m_h264_param.level_asymmerty_allow.empty()) {
        format_specific_param << "level-asymmetry-allowed=" << m_h264_param.level_asymmerty_allow;
    }
    if (!m_h264_param.packetization_mode.empty()) {
        format_specific_param << ";packetization-mode=" << m_h264_param.packetization_mode;
    }
    if (!m_h264_param.profile_level_id.empty()) {
        format_specific_param << ";profile-level-id=" << m_h264_param.profile_level_id;
    }

    media_payload_type.m_format_specific_param = format_specific_param.str();

    return media_payload_type;
}

int32_t YangVideoPayload::set_h264_param_desc(std::string fmtp)
{
    int32_t err = Yang_Ok;

    // For example: level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42e01f
    std::vector<std::string> attributes = yang_splits(fmtp, ";");

    for (size_t i = 0; i < attributes.size(); ++i) {
        std::string attribute = attributes.at(i);

        std::vector<std::string> kv = yang_splits(attribute, "=");
        if (kv.size() != 2) {
            return yang_error_wrap(ERROR_RTC_SDP_DECODE, "invalid h264 param=%s", attribute.c_str());
        }

        if (kv[0] == "profile-level-id") {
            m_h264_param.profile_level_id = kv[1];
        } else if (kv[0] == "packetization-mode") {
            // 6.3.  Non-Interleaved Mode
            // This mode is in use when the value of the OPTIONAL packetization-mode
            // media type parameter is equal to 1.  This mode SHOULD be supported.
            // It is primarily intended for low-delay applications.  Only single NAL
            // unit packets, STAP-As, and FU-As MAY be used in this mode.  STAP-Bs,
            // MTAPs, and FU-Bs MUST NOT be used.  The transmission order of NAL
            // units MUST comply with the NAL unit decoding order.
            // @see https://tools.ietf.org/html/rfc6184#section-6.3
            m_h264_param.packetization_mode = kv[1];
        } else if (kv[0] == "level-asymmetry-allowed") {
            m_h264_param.level_asymmerty_allow = kv[1];
        } else {
            return yang_error_wrap(ERROR_RTC_SDP_DECODE, "invalid h264 param=%s", kv[0].c_str());
        }
    }

    return err;
}

