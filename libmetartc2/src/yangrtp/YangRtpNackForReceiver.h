#ifndef SRC_YANGRTP_YANGRTPNACKFORRECEIVER_H_
#define SRC_YANGRTP_YANGRTPNACKFORRECEIVER_H_
#include <stdint.h>
#include <yangrtp/YangRtcpNack.h>
#include <yangrtp/YangRtpRingBuffer.h>
#include <yangrtp/YangRtpPlayNackBuffer.h>
#include <map>

struct YangNackOption
{
    int32_t max_req_count;
    int64_t max_alive_time;
    int64_t first_nack_interval;
    int64_t nack_interval;

    int64_t max_nack_interval;
    int64_t min_nack_interval;
    int64_t nack_check_interval;

    YangNackOption();
};

struct YangRtpNackInfo
{
    // Use to control the time of first nack req and the life of seq.
    int64_t generate_time_;
    // Use to control nack interval.
    int64_t pre_req_nack_time_;
    // Use to control nack times.
    int32_t req_nack_count_;

    YangRtpNackInfo();
};

class YangRtpNackForReceiver
{
private:
    // Nack queue, seq order, oldest to newest.
    std::map<uint16_t, YangRtpNackInfo, YangSeqCompareLess> m_queue;
    // Max nack count.
    size_t m_max_queue_size;
    //YangRtpRingBuffer* m_rtp;
    YangRtpPlayNackBuffer* m_rtp;
    YangNackOption m_opts;
private:
    int64_t m_pre_check_time;
private:
    int32_t m_rtt;
public:
    YangRtpNackForReceiver(YangRtpPlayNackBuffer* rtp, size_t queue_size);
    virtual ~YangRtpNackForReceiver();
public:
    void insert(uint16_t first, uint16_t last);
    void remove(uint16_t seq);
    YangRtpNackInfo* find(uint16_t seq);
    void check_queue_size();
    bool isEmpty();
public:
    void get_nack_seqs(YangRtcpNack& seqs, uint32_t& timeout_nacks);
public:
    void update_rtt(int32_t rtt);
};
#endif /* SRC_YANGRTP_YANGRTPNACKFORRECEIVER_H_ */
