#ifndef SRC_YANGRTP_YANGAUDIOPAYLOAD_H_
#define SRC_YANGRTP_YANGAUDIOPAYLOAD_H_

#include <yangrtp/YangCodecPayload.h>
class YangAudioPayload : public YangCodecPayload
{
    struct SrsOpusParameter
    {
        int32_t minptime;
        bool use_inband_fec;
        bool usedtx;

        SrsOpusParameter() {
            minptime = 0;
            use_inband_fec = false;
            usedtx = false;
        }
    };

public:
    int32_t m_channel;
    SrsOpusParameter m_opus_param;
public:
    YangAudioPayload();
    YangAudioPayload(uint8_t pt, std::string encode_name, int32_t sample, int32_t channel);
    virtual ~YangAudioPayload();
public:
    virtual YangAudioPayload* copy();
    virtual YangMediaPayloadType generate_media_payload_type();
public:
    int32_t set_opus_param_desc(std::string fmtp);
};


#endif /* SRC_YANGRTP_YANGAUDIOPAYLOAD_H_ */
