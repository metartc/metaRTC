#include <yangrtp/YangRtcpTWCC.h>
#include <yangutil/sys/YangLog.h>

YangRtcpTWCC::SrsRtcpTWCCChunk::SrsRtcpTWCCChunk()
        : size(0), all_same(true), has_large_delta(false)
{
}

YangRtcpTWCC::YangRtcpTWCC(uint32_t sender_ssrc) : m_pkt_len(0)
{
    m_header.padding = 0;
    m_header.type = YangRtcpType_rtpfb;
    m_header.rc = 15;
    m_header.version = kRtcpVersion;
    m_ssrc = sender_ssrc;
    m_media_ssrc = 0;
    m_base_sn = 0;
    m_reference_time = 0;
    m_fb_pkt_count = 0;
    m_next_base_sn = 0;
}

YangRtcpTWCC::~YangRtcpTWCC()
{
}

void YangRtcpTWCC::clear()
{
    m_encoded_chucks.clear();
    m_pkt_deltas.clear();
    m_recv_packets.clear();
    m_recv_sns.clear();
    m_next_base_sn = 0;
}

uint32_t YangRtcpTWCC::get_media_ssrc() const
{
    return m_media_ssrc;
}
uint16_t YangRtcpTWCC::get_base_sn() const
{
    return m_base_sn;
}

uint32_t YangRtcpTWCC::get_reference_time() const
{
    return m_reference_time;
}

uint8_t YangRtcpTWCC::get_feedback_count() const
{
    return m_fb_pkt_count;
}

vector<uint16_t> YangRtcpTWCC::get_packet_chucks() const
{
    return m_encoded_chucks;
}

vector<uint16_t> YangRtcpTWCC::get_recv_deltas() const
{
    return m_pkt_deltas;
}

void YangRtcpTWCC::set_media_ssrc(uint32_t ssrc)
{
    m_media_ssrc = ssrc;
}
void YangRtcpTWCC::set_base_sn(uint16_t sn)
{
    m_base_sn = sn;
}

void YangRtcpTWCC::set_reference_time(uint32_t time)
{
    m_reference_time = time;
}

void YangRtcpTWCC::set_feedback_count(uint8_t count)
{
    m_fb_pkt_count = count;
}

void YangRtcpTWCC::add_packet_chuck(uint16_t chunk)
{
    m_encoded_chucks.push_back(chunk);
}

void YangRtcpTWCC::add_recv_delta(uint16_t delta)
{
    m_pkt_deltas.push_back(delta);
}

int32_t YangRtcpTWCC::recv_packet(uint16_t sn, int64_t ts)
{
    map<uint16_t, int64_t>::iterator it = m_recv_packets.find(sn);
    if(it != m_recv_packets.end()) {
        return yang_error_wrap(ERROR_RTC_RTCP, "TWCC dup seq: %d", sn);
    }

    m_recv_packets[sn] = ts;
    m_recv_sns.insert(sn);

    return Yang_Ok;
}

bool YangRtcpTWCC::need_feedback()
{
    return m_recv_packets.size() > 0;
}

int32_t YangRtcpTWCC::decode(YangBuffer *buffer)
{
    /*
    @doc: https://tools.ietf.org/html/draft-holmer-rmcat-transport-wide-cc-extensions-01#section-3.1
            0                   1                   2                   3
        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |V=2|P|  FMT=15 |    PT=205     |           length              |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                     SSRC of packet sender                     |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                      SSRC of media source                     |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |      base sequence number     |      packet status count      |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                 reference time                | fb pkt. count |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |          packet chunk         |         packet chunk          |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       .                                                               .
       .                                                               .
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |         packet chunk          |  recv delta   |  recv delta   |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       .                                                               .
       .                                                               .
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |           recv delta          |  recv delta   | zero padding  |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    */
    int32_t err = Yang_Ok;
    m_data = buffer->head();
    m_nb_data = buffer->left();

    if(Yang_Ok != (err = decode_header(buffer))) {
        return yang_error_wrap(err, "decode header");
    }

    m_payload_len = (m_header.length + 1) * 4 - sizeof(YangRtcpHeader) - 4;
    buffer->read_bytes((char *)m_payload, m_payload_len);

    return err;
}

uint64_t YangRtcpTWCC::nb_bytes()
{
    return kMaxUDPDataSize;
}

int64_t YangRtcpTWCC::calculate_delta_us(int64_t ts, int64_t last)
{
    int64_t divisor = kTwccFbReferenceTimeDivisor;
    int64_t delta_us = (ts - last) % divisor;

    if (delta_us > (divisor >> 1))
        delta_us -= divisor;

    delta_us += (delta_us < 0) ? (-kTwccFbDeltaUnit / 2) : (kTwccFbDeltaUnit / 2);
    delta_us /= kTwccFbDeltaUnit;

    return delta_us;
}

bool YangRtcpTWCC::can_add_to_chunk(YangRtcpTWCC::SrsRtcpTWCCChunk& chunk, int32_t delta_size)
{
	yang_info("can_add %d chunk->size %u delta_sizes %d %d %d %d %d %d %d %d %d %d %d %d %d %d all_same %d has_large_delta %d",
	    delta_size, chunk.size, chunk.delta_sizes[0], chunk.delta_sizes[1], chunk.delta_sizes[2], chunk.delta_sizes[3],
	    chunk.delta_sizes[4], chunk.delta_sizes[5], chunk.delta_sizes[6], chunk.delta_sizes[7], chunk.delta_sizes[8],
	    chunk.delta_sizes[9], chunk.delta_sizes[10], chunk.delta_sizes[11], chunk.delta_sizes[12], chunk.delta_sizes[13],
	    (int)chunk.all_same, (int)chunk.has_large_delta);

    if (chunk.size < kTwccFbTwoBitElements) {
        return true;
    }

    if (chunk.size < kTwccFbOneBitElements && !chunk.has_large_delta && delta_size != kTwccFbLargeRecvDeltaBytes) {
        return true;
    }

    if (chunk.size < kTwccFbMaxRunLength && chunk.all_same && chunk.delta_sizes[0] == delta_size) {
        yang_info("< %d && all_same && delta_size[0] %d == %d", kTwccFbMaxRunLength, chunk.delta_sizes[0], delta_size);
        return true;
    }

    return false;
}

void YangRtcpTWCC::add_to_chunk(YangRtcpTWCC::SrsRtcpTWCCChunk& chunk, int32_t delta_size)
{
    if (chunk.size < kTwccFbMaxBitElements) {
        chunk.delta_sizes[chunk.size] = delta_size;
    }

    chunk.size += 1;
    chunk.all_same = chunk.all_same && delta_size == chunk.delta_sizes[0];
    chunk.has_large_delta = chunk.has_large_delta || delta_size >= kTwccFbLargeRecvDeltaBytes;
}

int32_t YangRtcpTWCC::encode_chunk_run_length(YangRtcpTWCC::SrsRtcpTWCCChunk& chunk)
{
    if (!chunk.all_same || chunk.size > kTwccFbMaxRunLength) {
        return yang_error_wrap(ERROR_RTC_RTCP, "invalid run all_same:%d, size:%d", chunk.all_same, chunk.size);
    }

    uint16_t encoded_chunk = (chunk.delta_sizes[0] << 13) | chunk.size;

    m_encoded_chucks.push_back(encoded_chunk);
    m_pkt_len += sizeof(encoded_chunk);

    return Yang_Ok;
}

int32_t YangRtcpTWCC::encode_chunk_one_bit(YangRtcpTWCC::SrsRtcpTWCCChunk& chunk)
{
    int32_t i = 0;
    if (chunk.has_large_delta) {
        return yang_error_wrap(ERROR_RTC_RTCP, "invalid large delta");
    }

    uint16_t encoded_chunk = 0x8000;
    for (i = 0; i < chunk.size; ++i) {
        encoded_chunk |= (chunk.delta_sizes[i] << (kTwccFbOneBitElements - 1 - i));
    }

    m_encoded_chucks.push_back(encoded_chunk);
    m_pkt_len += sizeof(encoded_chunk);

    // 1 0 symbol_list
    return Yang_Ok;
}

int32_t YangRtcpTWCC::encode_chunk_two_bit(YangRtcpTWCC::SrsRtcpTWCCChunk& chunk, size_t size, bool shift)
{
    uint32_t  i = 0;
    uint8_t delta_size = 0;

    uint16_t encoded_chunk = 0xc000;
    // 1 1 symbol_list
    for (i = 0; i < size; ++i) {
        encoded_chunk |= (chunk.delta_sizes[i] << (2 * (kTwccFbTwoBitElements - 1 - i)));
    }
    m_encoded_chucks.push_back(encoded_chunk);
    m_pkt_len += sizeof(encoded_chunk);

    if (shift) {
        chunk.size -= size;
        chunk.all_same = true;
        chunk.has_large_delta = false;
        for (i = 0; i < chunk.size; ++i) {
            delta_size = chunk.delta_sizes[i + size];
            chunk.delta_sizes[i] = delta_size;
            chunk.all_same = (chunk.all_same && delta_size == chunk.delta_sizes[0]);
            chunk.has_large_delta = chunk.has_large_delta || delta_size == kTwccFbLargeRecvDeltaBytes;
        }
    }

    return Yang_Ok;
}

void YangRtcpTWCC::reset_chunk(YangRtcpTWCC::SrsRtcpTWCCChunk& chunk)
{
    chunk.size = 0;

    chunk.all_same = true;
    chunk.has_large_delta = false;
}

int32_t YangRtcpTWCC::encode_chunk(YangRtcpTWCC::SrsRtcpTWCCChunk& chunk)
{
    int32_t err = Yang_Ok;

    if (can_add_to_chunk(chunk, 0) && can_add_to_chunk(chunk, 1) && can_add_to_chunk(chunk, 2)) {
        return yang_error_wrap(ERROR_RTC_RTCP, "TWCC chunk");
    }

    if (chunk.all_same) {
        if ((err = encode_chunk_run_length(chunk)) != Yang_Ok) {
            return yang_error_wrap(err, "encode run");
        }
        reset_chunk(chunk);
        return err;
    }

    if (chunk.size == kTwccFbOneBitElements) {
        if ((err = encode_chunk_one_bit(chunk)) != Yang_Ok) {
            return yang_error_wrap(err, "encode chunk");
        }
        reset_chunk(chunk);
        return err;
    }

    if ((err = encode_chunk_two_bit(chunk, kTwccFbTwoBitElements, true)) != Yang_Ok) {
        return yang_error_wrap(err, "encode chunk");
    }

    return err;
}

int32_t YangRtcpTWCC::encode_remaining_chunk(YangRtcpTWCC::SrsRtcpTWCCChunk& chunk)
{
    if (chunk.all_same) {
        return encode_chunk_run_length(chunk);
    } else if (chunk.size <= kTwccFbTwoBitElements) {
        // FIXME, TRUE or FALSE
        return encode_chunk_two_bit(chunk, chunk.size, false);
    }
    return encode_chunk_one_bit(chunk);
}

int32_t YangRtcpTWCC::process_pkt_chunk(YangRtcpTWCC::SrsRtcpTWCCChunk& chunk, int32_t delta_size)
{
    int32_t err = Yang_Ok;

    size_t needed_chunk_size = chunk.size == 0 ? kTwccFbChunkBytes : 0;

    size_t might_occupied = m_pkt_len + needed_chunk_size + delta_size;
    if (might_occupied > kRtcpPacketSize) {
        return yang_error_wrap(ERROR_RTC_RTCP, "might_occupied %zu", might_occupied);
    }

    if (can_add_to_chunk(chunk, delta_size)) {
        //pkt_len += needed_chunk_size;
        add_to_chunk(chunk, delta_size);
        return err;
    }
    if ((err = encode_chunk(chunk)) != Yang_Ok) {
        return yang_error_wrap(err, "encode chunk, delta_size %u", delta_size);
    }
    add_to_chunk(chunk, delta_size);
    return err;
}

int32_t YangRtcpTWCC::encode(YangBuffer *buffer)
{
    int32_t err = Yang_Ok;

    err = do_encode(buffer);

    if (err != Yang_Ok || m_next_base_sn == 0) {
        clear();
    }

    return err;
}

int32_t YangRtcpTWCC::do_encode(YangBuffer *buffer)
{
    /*
    @doc: https://tools.ietf.org/html/draft-holmer-rmcat-transport-wide-cc-extensions-01#section-3.1
            0                   1                   2                   3
        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |V=2|P|  FMT=15 |    PT=205     |           length              |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                     SSRC of packet sender                     |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                      SSRC of media source                     |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |      base sequence number     |      packet status count      |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                 reference time                | fb pkt. count |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |          packet chunk         |         packet chunk          |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       .                                                               .
       .                                                               .
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |         packet chunk          |  recv delta   |  recv delta   |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       .                                                               .
       .                                                               .
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |           recv delta          |  recv delta   | zero padding  |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    */
    int32_t err = Yang_Ok;

    if(!buffer->require(nb_bytes())) {
        return yang_error_wrap(ERROR_RTC_RTCP, "requires %d bytes", nb_bytes());
    }

    m_pkt_len = kTwccFbPktHeaderSize;

    set<uint16_t, YangSeqCompareLess>::iterator it_sn = m_recv_sns.begin();
    if (!m_next_base_sn) {
        m_base_sn = *it_sn;
    } else {
        m_base_sn = m_next_base_sn;
        it_sn = m_recv_sns.find(m_base_sn);
    }

    map<uint16_t, int64_t>::iterator it_ts = m_recv_packets.find(m_base_sn);
    int64_t ts = it_ts->second;

    m_reference_time = (ts % kTwccFbReferenceTimeDivisor) / kTwccFbTimeMultiplier;
    int64_t last_ts = (int64_t)(m_reference_time) * kTwccFbTimeMultiplier;

    uint16_t last_sn = m_base_sn;
    uint16_t packet_count = 0;

    // encode chunk
    YangRtcpTWCC::SrsRtcpTWCCChunk chunk;
    for(; it_sn != m_recv_sns.end(); ++it_sn) {
        // check whether exceed buffer len
        // max recv_delta_size = 2
        if (m_pkt_len + 2 >= buffer->left()) {
            break;
        }

        uint16_t current_sn = *it_sn;
        // calculate delta
        it_ts = m_recv_packets.find(current_sn);
        if (it_ts == m_recv_packets.end()) {
            continue;
        }

        packet_count++;
        int64_t delta_us = calculate_delta_us(it_ts->second, last_ts);
        int16_t delta = delta_us;
        if(delta != delta_us) {
            return yang_error_wrap(ERROR_RTC_RTCP, "twcc: delta:%lld, exceeds the 16bits", delta_us);
        }

        if(current_sn > (last_sn + 1)) {
            // lost packet
            for(uint16_t lost_sn = last_sn + 1; lost_sn < current_sn; ++lost_sn) {
                process_pkt_chunk(chunk, 0);
                packet_count++;
            }
        }

        // FIXME 24-bit base receive delta not supported
        int32_t recv_delta_size = (delta >= 0 && delta <= 0xff) ? 1 : 2;
        if ((err = process_pkt_chunk(chunk, recv_delta_size)) != Yang_Ok) {
            return yang_error_wrap(err, "delta_size %d, failed to append_recv_delta", recv_delta_size);
        }

        m_pkt_deltas.push_back(delta);
        last_ts += delta * kTwccFbDeltaUnit;
        m_pkt_len += recv_delta_size;
        last_sn = current_sn;

        m_recv_packets.erase(it_ts);
    }

    m_next_base_sn = 0;
    if (it_sn != m_recv_sns.end()) {
        m_next_base_sn = *it_sn;
    }

    if(0 < chunk.size) {
        if((err = encode_remaining_chunk(chunk)) != Yang_Ok) {
            return yang_error_wrap(err, "encode chunk");
        }
    }

    // encode rtcp twcc packet
    if((m_pkt_len % 4) == 0) {
        m_header.length = m_pkt_len / 4;
    } else {
        m_header.length = (m_pkt_len + 4 - (m_pkt_len%4)) / 4;
    }
    m_header.length -= 1;

    if(Yang_Ok != (err = encode_header(buffer))) {
        return yang_error_wrap(err, "encode header");
    }
    buffer->write_4bytes(m_media_ssrc);
    buffer->write_2bytes(m_base_sn);
    buffer->write_2bytes(packet_count);
    buffer->write_3bytes(m_reference_time);
    buffer->write_1bytes(m_fb_pkt_count);

    for(vector<uint16_t>::iterator it = m_encoded_chucks.begin(); it != m_encoded_chucks.end(); ++it) {
        buffer->write_2bytes(*it);
    }
    for(vector<uint16_t>::iterator it = m_pkt_deltas.begin(); it != m_pkt_deltas.end(); ++it) {
        if(0 <= *it && 0xFF >= *it) {
            // small delta
            uint8_t delta = *it;
            buffer->write_1bytes(delta);
        } else {
            // large or negative delta
            buffer->write_2bytes(*it);
        }
    }
    while((m_pkt_len % 4) != 0) {
        buffer->write_1bytes(0);
        m_pkt_len++;
    }

    m_encoded_chucks.clear();
    m_pkt_deltas.clear();

    return err;
}
