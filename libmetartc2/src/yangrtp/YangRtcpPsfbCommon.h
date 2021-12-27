#ifndef SRC_YANGRTP_YANGRTCPPSFBCOMMON_H_
#define SRC_YANGRTP_YANGRTCPPSFBCOMMON_H_
#include <yangrtp/YangRtcpCommon.h>

class YangRtcpPsfbCommon : public YangRtcpCommon
{
protected:
    uint32_t m_media_ssrc;
public:
    YangRtcpPsfbCommon();
    virtual ~YangRtcpPsfbCommon();

    uint32_t get_media_ssrc() const;
    void set_media_ssrc(uint32_t ssrc);

public:
    virtual int32_t decode(YangBuffer *buffer);
    virtual uint64_t nb_bytes();
    virtual int32_t encode(YangBuffer *buffer);
};
#endif /* SRC_YANGRTP_YANGRTCPPSFBCOMMON_H_ */
