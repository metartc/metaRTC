#ifndef SRC_YANGSRS_YANGRTXPAYLOADDES_H_
#define SRC_YANGSRS_YANGRTXPAYLOADDES_H_
#include <yangsrs/YangCodecPayload.h>
class YangRtxPayloadDes : public YangCodecPayload
{
public:
    uint8_t apt_;
public:
    YangRtxPayloadDes();
    YangRtxPayloadDes(uint8_t pt, uint8_t apt);
    virtual ~YangRtxPayloadDes();

public:
    virtual YangRtxPayloadDes* copy();
    virtual YangMediaPayloadType generate_media_payload_type();
};

#endif /* SRC_YANGSRS_YANGRTXPAYLOADDES_H_ */
