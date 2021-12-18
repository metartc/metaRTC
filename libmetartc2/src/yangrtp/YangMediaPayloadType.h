#ifndef SRC_YANGRTP_YANGMEDIAPAYLOADTYPE_H_
#define SRC_YANGRTP_YANGMEDIAPAYLOADTYPE_H_
#include <stdint.h>
#include <string>
#include <vector>
class YangMediaPayloadType
{
public:
    YangMediaPayloadType(int32_t payload_type);
    virtual ~YangMediaPayloadType();

    int32_t encode(std::stringstream& os);
public:
    int32_t m_payload_type;

    std::string m_encoding_name;
    int32_t m_clock_rate;
    std::string m_encoding_param;

    std::vector<std::string> m_rtcp_fb;
    std::string m_format_specific_param;
};

#endif /* SRC_YANGRTP_YANGMEDIAPAYLOADTYPE_H_ */
