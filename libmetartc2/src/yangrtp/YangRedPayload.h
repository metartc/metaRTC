#ifndef SRC_YANGRTP_YANGREDPAYLOAD_H_
#define SRC_YANGRTP_YANGREDPAYLOAD_H_
#include <yangrtp/YangCodecPayload.h>
class YangRedPayload : public YangCodecPayload
{
public:
    int32_t m_channel;
public:
    YangRedPayload();
    YangRedPayload(uint8_t pt, std::string encode_name, int32_t sample, int32_t channel);
    virtual ~YangRedPayload();
public:
    virtual YangRedPayload* copy();
    virtual YangMediaPayloadType generate_media_payload_type();
};
#endif /* SRC_YANGRTP_YANGREDPAYLOAD_H_ */
