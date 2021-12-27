
#ifndef SRC_YANGRTP_YANGRTCPTWCC_H_
#define SRC_YANGRTP_YANGRTCPTWCC_H_
#include <yangrtp/YangRtp.h>
#include <yangrtp/YangRtcpCommon.h>
#include <vector>
#include <map>
#include <set>

// The Message format of TWCC, @see https://tools.ietf.org/html/draft-holmer-rmcat-transport-wide-cc-extensions-01#section-3.1
//       0                   1                   2                   3
//       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      |V=2|P|  FMT=15 |    PT=205     |           length              |
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      |                     SSRC of packet sender                     |
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      |                      SSRC of media source                     |
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      |      base sequence number     |      packet status count      |
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      |                 reference time                | fb pkt. count |
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      |          packet chunk         |         packet chunk          |
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      .                                                               .
//      .                                                               .
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      |         packet chunk          |  recv delta   |  recv delta   |
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      .                                                               .
//      .                                                               .
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      |           recv delta          |  recv delta   | zero padding  |
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#define kTwccFbPktHeaderSize (4 + 8 + 8)
#define kTwccFbChunkBytes (2)
#define kTwccFbPktFormat (15)
#define kTwccFbPayloadType (205)
#define kTwccFbMaxPktStatusCount (0xffff)
#define kTwccFbDeltaUnit (250)	 // multiple of 250us
#define kTwccFbTimeMultiplier (kTwccFbDeltaUnit * (1 << 8)) // multiplicand multiplier/* 250us -> 64ms  (1 << 8) */
#define kTwccFbReferenceTimeDivisor ((1ll<<24) * kTwccFbTimeMultiplier) // dividend divisor

#define kTwccFbMaxRunLength 		0x1fff
#define kTwccFbOneBitElements 		14
#define kTwccFbTwoBitElements 		7
#define kTwccFbLargeRecvDeltaBytes	2
#define kTwccFbMaxBitElements 		kTwccFbOneBitElements
class YangRtcpTWCC : public YangRtcpCommon
{
private:
    uint32_t m_media_ssrc;
    uint16_t m_base_sn;
    int32_t m_reference_time;
    uint8_t m_fb_pkt_count;
    std::vector<uint16_t> m_encoded_chucks;
    std::vector<uint16_t> m_pkt_deltas;

    std::map<uint16_t, int64_t> m_recv_packets;
    std::set<uint16_t, YangSeqCompareLess> m_recv_sns;

    struct SrsRtcpTWCCChunk {
        uint8_t delta_sizes[kTwccFbMaxBitElements];
        uint16_t size;
        bool all_same;
        bool has_large_delta;
        SrsRtcpTWCCChunk();
    };

    int32_t m_pkt_len;
    uint16_t m_next_base_sn;
private:
    void clear();
    int64_t calculate_delta_us(int64_t ts, int64_t last);
    int32_t process_pkt_chunk(SrsRtcpTWCCChunk& chunk, int32_t delta_size);
    bool can_add_to_chunk(SrsRtcpTWCCChunk& chunk, int32_t delta_size);
    void add_to_chunk(SrsRtcpTWCCChunk& chunk, int32_t delta_size);
    int32_t encode_chunk(SrsRtcpTWCCChunk& chunk);
    int32_t encode_chunk_run_length(SrsRtcpTWCCChunk& chunk);
    int32_t encode_chunk_one_bit(SrsRtcpTWCCChunk& chunk);
    int32_t encode_chunk_two_bit(SrsRtcpTWCCChunk& chunk, size_t size, bool shift);
    void reset_chunk(SrsRtcpTWCCChunk& chunk);
    int32_t encode_remaining_chunk(SrsRtcpTWCCChunk& chunk);
public:
    YangRtcpTWCC(uint32_t sender_ssrc = 0);
    virtual ~YangRtcpTWCC();

    uint32_t get_media_ssrc() const;
    uint16_t get_base_sn() const;
    uint32_t get_reference_time() const;
    uint8_t get_feedback_count() const;
    std::vector<uint16_t> get_packet_chucks() const;
    std::vector<uint16_t> get_recv_deltas() const;

    void set_media_ssrc(uint32_t ssrc);
    void set_base_sn(uint16_t sn);
    void set_reference_time(uint32_t time);
    void set_feedback_count(uint8_t count);
    void add_packet_chuck(uint16_t chuck);
    void add_recv_delta(uint16_t delta);

    int32_t recv_packet(uint16_t sn, int64_t ts);
    bool need_feedback();


public:
    virtual int32_t decode(YangBuffer *buffer);
    virtual uint64_t nb_bytes();
    virtual int32_t encode(YangBuffer *buffer);
private:
    int32_t do_encode(YangBuffer *buffer);
};
#endif /* SRC_YANGRTP_YANGRTCPTWCC_H_ */
