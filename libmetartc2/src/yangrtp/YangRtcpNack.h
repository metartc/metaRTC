#ifndef SRC_YANGRTP_YANGRTCPNACK_H_
#define SRC_YANGRTP_YANGRTCPNACK_H_
#include <yangrtp/YangRtcpCommon.h>
#include <yangrtp/YangRtp.h>
#include <set>
class YangRtcpNack : public YangRtcpCommon
{
private:
    struct YangPidBlp {
        uint16_t pid;
        uint16_t blp;
        bool in_use;
    };

    uint32_t m_media_ssrc_;
    std::set<uint16_t, YangSeqCompareLess> m_lost_sns;
public:
    YangRtcpNack(uint32_t sender_ssrc = 0);
    virtual ~YangRtcpNack();

    uint32_t get_media_ssrc() const;
    std::vector<uint16_t> get_lost_sns() const;
    bool empty();

    void set_media_ssrc(uint32_t ssrc);
    void add_lost_sn(uint16_t sn);

public:
    virtual int32_t decode(YangBuffer *buffer);
    virtual uint64_t nb_bytes();
    virtual int32_t encode(YangBuffer *buffer);
};


#endif /* SRC_YANGRTP_YANGRTCPNACK_H_ */
