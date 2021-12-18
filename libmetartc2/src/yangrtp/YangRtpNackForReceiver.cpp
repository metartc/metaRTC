#include <yangrtp/YangRtpNackForReceiver.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangutil/yangtype.h>
#include <yangutil/sys/YangTime.h>
YangNackOption::YangNackOption()
{
    max_req_count = 10;
    max_alive_time = 1000 * YANG_UTIME_MILLISECONDS;
    first_nack_interval = 10 * YANG_UTIME_MILLISECONDS;
    nack_interval = 50 * YANG_UTIME_MILLISECONDS;
    max_nack_interval = 500 * YANG_UTIME_MILLISECONDS;
    min_nack_interval = 15 * YANG_UTIME_MILLISECONDS;

    nack_check_interval = 15 * YANG_UTIME_MILLISECONDS;

    //TODO: FIXME: audio and video using diff nack strategy
    // video:
    // max_alive_time = 1 * YANG_UTIME_SECONDS
    // max_count = 15;
    // nack_interval = 50 * YANG_UTIME_MILLISECONDS
    //
    // audio:
    // DefaultRequestNackDelay = 30; //ms
    // DefaultLostPacketLifeTime = 600; //ms
    // FirstRequestInterval = 50;//ms
}

YangRtpNackInfo::YangRtpNackInfo()
{
    generate_time_ = yang_get_system_time();//yang_update_system_time();
    pre_req_nack_time_ = 0;
    req_nack_count_ = 0;
}

YangRtpNackForReceiver::YangRtpNackForReceiver(YangRtpPlayNackBuffer* rtp, size_t queue_size)
{
    m_max_queue_size = queue_size;
    m_rtp = rtp;
    m_pre_check_time = 0;
    m_rtt = 0;


}

YangRtpNackForReceiver::~YangRtpNackForReceiver()
{
}

void YangRtpNackForReceiver::insert(uint16_t first, uint16_t last)
{
   for (uint16_t s = first; s != last; ++s) {
        m_queue[s] = YangRtpNackInfo();
    }
}

void YangRtpNackForReceiver::remove(uint16_t seq)
{
    m_queue.erase(seq);
}

YangRtpNackInfo* YangRtpNackForReceiver::find(uint16_t seq)
{
    std::map<uint16_t, YangRtpNackInfo>::iterator iter = m_queue.find(seq);

    if (iter == m_queue.end()) {
        return NULL;
    }

    return &(iter->second);
}

void YangRtpNackForReceiver::check_queue_size()
{
    if (m_queue.size() >= m_max_queue_size) {
        m_rtp->notify_nack_list_full();
        m_queue.clear();
    }
}
bool YangRtpNackForReceiver::isEmpty(){
	return m_queue.empty();
}
void YangRtpNackForReceiver::get_nack_seqs(YangRtcpNack& seqs, uint32_t& timeout_nacks)
{

    int64_t now = yang_get_system_time();

    //int64_t interval = now - m_pre_check_time;

    if (now - m_pre_check_time < m_opts.nack_check_interval) {
        return;
    }
    m_pre_check_time = now;

    std::map<uint16_t, YangRtpNackInfo>::iterator iter = m_queue.begin();
    while (iter != m_queue.end()) {
        const uint16_t& seq = iter->first;
        YangRtpNackInfo& nack_info = iter->second;

        int32_t alive_time = now - nack_info.generate_time_;
        if (alive_time > m_opts.max_alive_time || nack_info.req_nack_count_ > m_opts.max_req_count) {
            ++timeout_nacks;
            m_rtp->notify_drop_seq(seq);
            m_queue.erase(iter++);
            continue;
        }

        // TODO:Statistics unorder packet.
        if (now - nack_info.generate_time_ < m_opts.first_nack_interval) {
            break;
        }

        int64_t nack_interval = yang_max(m_opts.min_nack_interval, m_opts.nack_interval / 3);
        if(m_opts.nack_interval < 50 * YANG_UTIME_MILLISECONDS){
            nack_interval = yang_max(m_opts.min_nack_interval, m_opts.nack_interval);
        }

        if (now - nack_info.pre_req_nack_time_ >= nack_interval ) {
            ++nack_info.req_nack_count_;
            nack_info.pre_req_nack_time_ = now;
            seqs.add_lost_sn(seq);
        }

        ++iter;
    }
}

void YangRtpNackForReceiver::update_rtt(int32_t rtt)
{
    m_rtt = rtt * YANG_UTIME_MILLISECONDS;

    if (m_rtt > m_opts.nack_interval) {
        m_opts.nack_interval = m_opts.nack_interval  * 0.8 + m_rtt * 0.2;
    } else {
        m_opts.nack_interval = m_rtt;
    }

    m_opts.nack_interval = yang_min(m_opts.nack_interval, m_opts.max_nack_interval);
}

