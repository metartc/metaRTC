//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGRTP_YANGRTCPAPP_H_
#define SRC_YANGRTP_YANGRTCPAPP_H_
#include <stdint.h>
#include <yangrtp/YangRtcpCommon.h>
#define Yang_Using_RtcpApp 0
#if Yang_Using_RtcpApp
class YangRtcpApp : public YangRtcpCommon
{
private:
    uint8_t m_name[4];
public:
    YangRtcpApp();
    virtual ~YangRtcpApp();

    static bool is_rtcp_app(uint8_t *data, int32_t nb_data);

    virtual uint8_t type() const;

    uint8_t get_subtype() const;
    std::string get_name() const;
    int32_t get_payload(uint8_t*& payload, int& len);

    int32_t set_subtype(uint8_t type);
    int32_t set_name(std::string name);
    int32_t set_payload(uint8_t* payload, int32_t len);
// interface ISrsCodec
public:
    virtual int32_t decode(YangBuffer *buffer);
    virtual uint64_t nb_bytes();
    virtual int32_t encode(YangBuffer *buffer);
};
#endif

#endif /* SRC_YANGRTP_YANGRTCPAPP_H_ */
