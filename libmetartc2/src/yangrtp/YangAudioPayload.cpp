#include <yangrtp/YangAudioPayload.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangString.h>
#include <sstream>
YangAudioPayload::YangAudioPayload()
{
    m_channel = 0;
}

YangAudioPayload::YangAudioPayload(uint8_t pt, std::string encode_name, int32_t sample, int32_t channel)
    :YangCodecPayload(pt, encode_name, sample)
{
    m_type = "audio";
    m_channel = channel;
    m_opus_param.minptime = 0;
    m_opus_param.use_inband_fec = false;
    m_opus_param.usedtx = false;
}

YangAudioPayload::~YangAudioPayload()
{
}

YangAudioPayload* YangAudioPayload::copy()
{
    YangAudioPayload* cp = new YangAudioPayload();

    cp->m_type = m_type;
    cp->m_pt = m_pt;
    cp->m_pt_of_publisher = m_pt_of_publisher;
    cp->m_name = m_name;
    cp->m_sample = m_sample;
    cp->m_rtcp_fbs = m_rtcp_fbs;
    cp->m_channel = m_channel;
    cp->m_opus_param = m_opus_param;

    return cp;
}


YangMediaPayloadType YangAudioPayload::generate_media_payload_type()
{
    YangMediaPayloadType media_payload_type(m_pt);

    media_payload_type.m_encoding_name = m_name;
    media_payload_type.m_clock_rate = m_sample;
    if (m_channel != 0) {
        media_payload_type.m_encoding_param = yang_int2str(m_channel);
    }
    media_payload_type.m_rtcp_fb = m_rtcp_fbs;

    std::ostringstream format_specific_param;
    if (m_opus_param.minptime) {
        format_specific_param << "minptime=" << m_opus_param.minptime;
    }
    if (m_opus_param.use_inband_fec) {
        format_specific_param << ";useinbandfec=1";
    }
    if (m_opus_param.usedtx) {
        format_specific_param << ";usedtx=1";
    }
    media_payload_type.m_format_specific_param = format_specific_param.str();

    return media_payload_type;
}

int32_t YangAudioPayload::set_opus_param_desc(std::string fmtp)
{
    int32_t err = Yang_Ok;
    std::vector<std::string> vec = yang_splits(fmtp, ";");
    for (size_t i = 0; i < vec.size(); ++i) {
        std::vector<std::string> kv = yang_splits(vec[i], "=");
        if (kv.size() == 2) {
            if (kv[0] == "minptime") {
                m_opus_param.minptime = (int)::atol(kv[1].c_str());
            } else if (kv[0] == "useinbandfec") {
                m_opus_param.use_inband_fec = (kv[1] == "1") ? true : false;
            } else if (kv[0] == "usedtx") {
                m_opus_param.usedtx = (kv[1] == "1") ? true : false;
            }
        } else {
            return yang_error_wrap(ERROR_RTC_SDP_DECODE, "invalid opus param=%s", vec[i].c_str());
        }
    }

    return err;
}
