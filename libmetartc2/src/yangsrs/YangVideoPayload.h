#ifndef SRC_YANGSRS_YANGVIDEOPAYLOAD_H_
#define SRC_YANGSRS_YANGVIDEOPAYLOAD_H_

#include <yangsrs/YangCodecPayload.h>

// TODO: FIXME: Rename it.
class YangVideoPayload : public YangCodecPayload
{
public:
    struct H264SpecificParameter
    {
        std::string profile_level_id;
        std::string packetization_mode;
        std::string level_asymmerty_allow;
    };
    H264SpecificParameter m_h264_param;

public:
    YangVideoPayload();
    YangVideoPayload(uint8_t pt, std::string encode_name, int32_t sample);
    virtual ~YangVideoPayload();
public:
    virtual YangVideoPayload* copy();
    virtual YangMediaPayloadType generate_media_payload_type();
public:
    int32_t set_h264_param_desc(std::string fmtp);
};

#endif /* SRC_YANGSRS_YANGVIDEOPAYLOAD_H_ */
