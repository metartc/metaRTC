#include <yangrtp/YangRtpRingBuffer.h>
#include <yangutil/yangtype.h>

YangRtpRingBuffer::YangRtpRingBuffer(int32_t capacity)
{
    m_nn_seq_flip_backs = 0;
    m_begin = m_end = 0;
    m_capacity = (uint16_t)capacity;
    m_contexttialized = false;

    m_queue = new YangRtpPacket*[m_capacity];
    memset(m_queue, 0, sizeof(YangRtpPacket*) * capacity);
}

YangRtpRingBuffer::~YangRtpRingBuffer()
{
    for (int32_t i = 0; i < m_capacity; ++i) {
        YangRtpPacket* pkt = m_queue[i];
        yang_delete(pkt);
    }
    yang_deleteA(m_queue);
}

bool YangRtpRingBuffer::empty()
{
    return m_begin == m_end;
}

int32_t YangRtpRingBuffer::size()
{
    int32_t size = yang_rtp_seq_distance(m_begin, m_end);
    //srs_assert(size >= 0);
    return size;
}

void YangRtpRingBuffer::advance_to(uint16_t seq)
{
    m_begin = seq;
}

void YangRtpRingBuffer::set(uint16_t at, YangRtpPacket* pkt)
{
    YangRtpPacket* p = m_queue[at % m_capacity];
    yang_delete(p);

    m_queue[at % m_capacity] = pkt;
}

void YangRtpRingBuffer::remove(uint16_t at)
{
    set(at, NULL);
}

uint32_t YangRtpRingBuffer::get_extended_highest_sequence()
{
    return m_nn_seq_flip_backs * 65536 + m_end - 1;
}

bool YangRtpRingBuffer::update(uint16_t seq, uint16_t& nack_first, uint16_t& nack_last)
{
    if (!m_contexttialized) {
        m_contexttialized = true;
        m_begin = seq;
        m_end = seq + 1;
        return true;
    }

    // Normal sequence, seq follows high_.
    if (yang_rtp_seq_distance(m_end, seq) >= 0) {
        //TODO: FIXME: if diff_upper > limit_max_size clear?
        // int16_t diff_upper = yang_rtp_seq_distance(end, seq)
        // notify_nack_list_full()
        nack_first = m_end;
        nack_last = seq;

        // When distance(seq,high_)>0 and seq<high_, seq must flip back,
        // for example, high_=65535, seq=1, distance(65535,1)>0 and 1<65535.
        // TODO: FIXME: The first flip may be dropped.
        if (seq < m_end) {
            ++m_nn_seq_flip_backs;
        }
        m_end = seq + 1;
        // TODO: FIXME: check whether is neccessary?
        // yang_rtp_seq_distance(begin, end) > max_size
        // advance_to(), srs_rtp_seq_distance(begin, end) < max_size;
        return true;
    }

    // Out-of-order sequence, seq before low_.
    if (yang_rtp_seq_distance(seq, m_begin) > 0) {
        nack_first = seq;
        nack_last = m_begin;
        m_begin = seq;

        // TODO: FIXME: Maybe should support startup drop.
        return true;
        // When startup, we may receive packets in chaos order.
        // Because we don't know the ISN(initiazlie sequence number), the first packet
        // we received maybe no the first packet client sent.
        // @remark We only log a warning, because it seems ok for publisher.
        //return false;
    }

    return true;
}

YangRtpPacket* YangRtpRingBuffer::at(uint16_t seq) {
    return m_queue[seq % m_capacity];
}

void YangRtpRingBuffer::notify_nack_list_full()
{
    clear_all_histroy();

    m_begin = m_end = 0;
    m_contexttialized = false;
}

void YangRtpRingBuffer::notify_drop_seq(uint16_t seq)
{
    remove(seq);
    advance_to(seq+1);
}

void YangRtpRingBuffer::clear_histroy(uint16_t seq)
{
    // TODO FIXME Did not consider loopback
    for (uint16_t i = 0; i < m_capacity; i++) {
        YangRtpPacket* p = m_queue[i];
        if (p && p->m_header.sequence < seq) {
            yang_delete(p);
            m_queue[i] = NULL;
        }
    }
}

void YangRtpRingBuffer::clear_all_histroy()
{
    for (uint16_t i = 0; i < m_capacity; i++) {
        YangRtpPacket* p = m_queue[i];
        if (p) {
            yang_delete(p);
            m_queue[i] = NULL;
        }
    }
}
