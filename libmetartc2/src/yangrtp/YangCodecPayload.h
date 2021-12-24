#ifndef SRC_YANGRTP_YANGCODECPAYLOAD_H_
#define SRC_YANGRTP_YANGCODECPAYLOAD_H_
#include <yangrtp/YangMediaPayloadType.h>
#include <stdint.h>
#include <string>
#include <vector>
// TODO: FIXME: Rename it.
class YangCodecPayload
{
public:
    std::string m_type;
    uint8_t m_pt;
    // for publish, equals to PT of itself;
    // for subscribe, is the PT of publisher;
    uint8_t m_pt_of_publisher;
    std::string m_name;
    int32_t m_sample;

    std::vector<std::string> m_rtcp_fbs;
public:
    YangCodecPayload();
    YangCodecPayload(uint8_t pt, std::string encode_name, int32_t sample);
    virtual ~YangCodecPayload();
public:
    virtual YangCodecPayload* copy();
    virtual YangMediaPayloadType generate_media_payload_type();
};
#endif /* SRC_YANGRTP_YANGCODECPAYLOAD_H_ */
