
#ifndef SRC_YANGRTP_YANGRTCPCOMPOUND_H_
#define SRC_YANGRTP_YANGRTCPCOMPOUND_H_
#include <yangrtp/YangRtcpCommon.h>

class YangRtcpCompound //: public IYangCodec
{
private:
    std::vector<YangRtcpCommon*> m_rtcps;
    int32_t m_nb_bytes;
    char* m_data;
    int32_t m_nb_data;
public:
    YangRtcpCompound();
    virtual ~YangRtcpCompound();

    // TODO: FIXME: Should rename it to pop(), because it's not a GET method.
    YangRtcpCommon* get_next_rtcp();
    int32_t add_rtcp(YangRtcpCommon *rtcp);
    void clear();

    char* data();
    int32_t size();

// interface ISrsCodec
public:
    virtual int32_t decode(YangBuffer *buffer);
    virtual uint64_t nb_bytes();
    virtual int32_t encode(YangBuffer *buffer);
};

#endif /* SRC_YANGRTP_YANGRTCPCOMPOUND_H_ */
