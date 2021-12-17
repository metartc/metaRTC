#ifndef SRC_YANGRTP_YANGRTPRINGBUFFER_H_
#define SRC_YANGRTP_YANGRTPRINGBUFFER_H_
#include <stdint.h>
#include <yangrtp/YangRtpPacket.h>
// For UDP, the packets sequence may present as bellow:
//      [seq1(done)|seq2|seq3 ... seq10|seq11(lost)|seq12|seq13]
//                   \___(head_sequence_)   \               \___(highest_sequence_)
//                                           \___(no received, in nack list)
//      * seq1: The packet is done, we have already got and processed it.
//      * seq2,seq3,...,seq10,seq12,seq13: Theses packets are in queue and wait to be processed.
//      * seq10: This packet is lost or not received, we will put it in the nack list.
// We store the received packets in ring buffer.
class YangRtpRingBuffer
{
public:
    YangRtpRingBuffer(int32_t capacity);
    virtual ~YangRtpRingBuffer();
private:
    // Capacity of the ring-buffer.
    uint16_t m_capacity;
    // Ring bufer.
    YangRtpPacket** m_queue;
    // Increase one when uint16 flip back, for get_extended_highest_sequence.
    uint64_t m_nn_seq_flip_backs;
    // Whether initialized, because we use uint16 so we can't use -1.
    bool m_contexttialized;
public:
    // The begin iterator for ring buffer.
    // For example, when got 1 elems, the begin is 0.
    uint16_t m_begin;
    // The end iterator for ring buffer.
    // For example, when got 1 elems, the end is 1.
    uint16_t m_end;

public:
    // Whether the ring buffer is empty.
    bool empty();
    // Get the count of elems in ring buffer.
    int32_t size();
    // Move the low position of buffer to seq.
    void advance_to(uint16_t seq);
    // Free the packet at position.
    void set(uint16_t at, YangRtpPacket* pkt);
    void remove(uint16_t at);
    // The highest sequence number, calculate the flip back base.
    uint32_t get_extended_highest_sequence();
    // Update the sequence, got the nack range by [first, last).
    // @return If false, the seq is too old.
    bool update(uint16_t seq, uint16_t& nack_first, uint16_t& nack_last);
    // Get the packet by seq.
    YangRtpPacket* at(uint16_t seq);
public:
    // TODO: FIXME: Refine it?
    void notify_nack_list_full();
    void notify_drop_seq(uint16_t seq);
public:
    void clear_histroy(uint16_t seq);
    void clear_all_histroy();
};
#endif /* SRC_YANGRTP_YANGRTPRINGBUFFER_H_ */
