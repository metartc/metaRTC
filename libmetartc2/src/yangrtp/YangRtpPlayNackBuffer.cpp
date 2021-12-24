#include <yangrtp/YangRtpPlayNackBuffer.h>
#include <yangrtp/YangRtpPacket.h>
#include <yangutil/yangtype.h>

YangRtpPlayNackBuffer::YangRtpPlayNackBuffer(int32_t capacity)
{
    m_nn_seq_flip_backs = 0;
    m_begin = m_end = 0;
    m_capacity = (uint16_t)capacity;
    m_contexttialized = false;

    m_queue = new int32_t[m_capacity];
    memset(m_queue, 0, sizeof(int32_t) * capacity);
}

YangRtpPlayNackBuffer::~YangRtpPlayNackBuffer()
{

    yang_deleteA(m_queue);
}

bool YangRtpPlayNackBuffer::empty()
{
    return m_begin == m_end;
}

int32_t YangRtpPlayNackBuffer::size()
{
    int32_t size = yang_rtp_seq_distance(m_begin, m_end);
    //srs_assert(size >= 0);
    return size;
}

void YangRtpPlayNackBuffer::advance_to(uint16_t seq)
{
    m_begin = seq;
}

void YangRtpPlayNackBuffer::set(uint16_t at, uint16_t pkt)
{


    m_queue[at % m_capacity] = pkt;
}

void YangRtpPlayNackBuffer::remove(uint16_t at)
{
	 m_queue[at % m_capacity] = -1;
  //  set(at, -1);
}

uint32_t YangRtpPlayNackBuffer::get_extended_highest_sequence()
{
    return m_nn_seq_flip_backs * 65536 + m_end - 1;
}
bool YangRtpPlayNackBuffer::update(uint16_t seq){
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
bool YangRtpPlayNackBuffer::update(uint16_t seq, uint16_t& nack_first, uint16_t& nack_last)
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

int32_t YangRtpPlayNackBuffer::at(uint16_t seq) {
    return m_queue[seq % m_capacity];
}

void YangRtpPlayNackBuffer::notify_nack_list_full()
{
    clear_all_histroy();

    m_begin = m_end = 0;
    m_contexttialized = false;
}

void YangRtpPlayNackBuffer::notify_drop_seq(uint16_t seq)
{
    remove(seq);
    advance_to(seq+1);
}

void YangRtpPlayNackBuffer::clear_histroy(uint16_t seq)
{
    // TODO FIXME Did not consider loopback
    for (uint16_t i = 0; i < m_capacity; i++) {

        if ( m_queue[i] < seq) {

            m_queue[i] = -1;
        }
    }
}

void YangRtpPlayNackBuffer::clear_all_histroy()
{
    for (uint16_t i = 0; i < m_capacity; i++) {

            m_queue[i] = -1;

    }
}
