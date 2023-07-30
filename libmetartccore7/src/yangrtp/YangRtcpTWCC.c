//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangrtp/YangRtcpTWCC.h>
#include <yangrtp/YangRtpConstant.h>

#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangTime.h>
#include <yangutil/sys/YangEndian.h>

yang_vector_impl(uint16_t)

void yang_twcc_clear(YangRecvTWCC* twcc)
{
	yang_clear_uint16_tVector(&twcc->encodedChucks);
	yang_clear_uint16_tVector(&twcc->pktDeltas);
	twcc->nextBaseSn = 0;
}

int64_t yang_twcc_calculate_delta_us(int64_t ts, int64_t last)
{
	int64_t divisor = kTwccFbReferenceTimeDivisor;
	int64_t delta_us = (ts - last) % divisor;

	if (delta_us > (divisor >> 1))
		delta_us -= divisor;

	delta_us += (delta_us < 0) ? (-kTwccFbDeltaUnit / 2) : (kTwccFbDeltaUnit / 2);
	delta_us /= kTwccFbDeltaUnit;

	return delta_us;
}

yangbool yang_twcc_can_add_to_chunk(YangRecvTWCC* twcc,YangRtcpTWCCChunk* chunk, int32_t delta_size)
{

	if (chunk->size < kTwccFbTwoBitElements) {
		return yangtrue;
	}

	if (chunk->size < kTwccFbOneBitElements && !chunk->has_large_delta && delta_size != kTwccFbLargeRecvDeltaBytes) {
		return yangtrue;
	}

	if (chunk->size < kTwccFbMaxRunLength && chunk->all_same && chunk->deltaSizes[0] == delta_size) {
		yang_info("< %d && all_same && delta_size[0] %d == %d", kTwccFbMaxRunLength, chunk->deltaSizes[0], delta_size);
		return yangtrue;
	}

	return yangfalse;
}

void yang_twcc_add_to_chunk(YangRecvTWCC* twcc,YangRtcpTWCCChunk* chunk, int32_t delta_size)
{
	if (chunk->size < kTwccFbMaxBitElements) {
		chunk->deltaSizes[chunk->size] = delta_size;
	}

	chunk->size += 1;
	chunk->all_same = chunk->all_same && delta_size == chunk->deltaSizes[0];
	chunk->has_large_delta = chunk->has_large_delta || delta_size >= kTwccFbLargeRecvDeltaBytes;
}

int32_t yang_twcc_encode_chunk_run_length(YangRecvTWCC* twcc,YangRtcpTWCCChunk* chunk)
{
	if (!chunk->all_same || chunk->size > kTwccFbMaxRunLength) {
		return yang_error_wrap(ERROR_RTC_RTCP, "invalid run all_same:%d, size:%d", chunk->all_same, chunk->size);
	}

	uint16_t encoded_chunk = (chunk->deltaSizes[0] << 13) | chunk->size;
	yang_insert_uint16_tVector(&twcc->encodedChucks,&encoded_chunk);

	twcc->pktLen += sizeof(encoded_chunk);

	return Yang_Ok;
}

int32_t yang_twcc_encode_chunk_one_bit(YangRecvTWCC* twcc,YangRtcpTWCCChunk* chunk)
{
	int32_t i = 0;
	if (chunk->has_large_delta) {
		return yang_error_wrap(ERROR_RTC_RTCP, "invalid large delta");
	}

	uint16_t encoded_chunk = 0x8000;
	for (i = 0; i < chunk->size; ++i) {
		encoded_chunk |= (chunk->deltaSizes[i] << (kTwccFbOneBitElements - 1 - i));
	}


	yang_insert_uint16_tVector(&twcc->encodedChucks,&encoded_chunk);
	twcc->pktLen += sizeof(encoded_chunk);

	// 1 0 symbol_list
	return Yang_Ok;
}

int32_t yang_twcc_encode_chunk_two_bit(YangRecvTWCC* twcc,YangRtcpTWCCChunk* chunk, size_t size, yangbool shift)
{
	uint32_t  i = 0;
	uint8_t delta_size = 0;

	uint16_t encoded_chunk = 0xc000;
	// 1 1 symbol_list
	for (i = 0; i < size; ++i) {
		encoded_chunk |= (chunk->deltaSizes[i] << (2 * (kTwccFbTwoBitElements - 1 - i)));
	}
	yang_insert_uint16_tVector(&twcc->encodedChucks,&encoded_chunk);
	twcc->pktLen += sizeof(encoded_chunk);

	if (shift) {
		chunk->size -= size;
		chunk->all_same = yangtrue;
		chunk->has_large_delta = yangfalse;
		for (i = 0; i < chunk->size; ++i) {
			delta_size = chunk->deltaSizes[i + size];
			chunk->deltaSizes[i] = delta_size;
			chunk->all_same = (chunk->all_same && delta_size == chunk->deltaSizes[0]);
			chunk->has_large_delta = chunk->has_large_delta || delta_size == kTwccFbLargeRecvDeltaBytes;
		}
	}

	return Yang_Ok;
}

void yang_twcc_reset_chunk(YangRecvTWCC* twcc,YangRtcpTWCCChunk* chunk)
{
	chunk->size = 0;

	chunk->all_same = yangtrue;
	chunk->has_large_delta = yangfalse;
}

int32_t yang_twcc_encode_chunk(YangRecvTWCC* twcc,YangRtcpTWCCChunk* chunk)
{
	int32_t err = Yang_Ok;

	if (yang_twcc_can_add_to_chunk(twcc,chunk, 0) && yang_twcc_can_add_to_chunk(twcc,chunk, 1) && yang_twcc_can_add_to_chunk(twcc,chunk, 2)) {
		return yang_error_wrap(ERROR_RTC_RTCP, "TWCC chunk");
	}

	if (chunk->all_same) {
		if ((err = yang_twcc_encode_chunk_run_length(twcc,chunk)) != Yang_Ok) {
			return yang_error_wrap(err, "encode run");
		}
		yang_twcc_reset_chunk(twcc,chunk);
		return err;
	}

	if (chunk->size == kTwccFbOneBitElements) {
		if ((err = yang_twcc_encode_chunk_one_bit(twcc,chunk)) != Yang_Ok) {
			return yang_error_wrap(err, "encode chunk");
		}
		yang_twcc_reset_chunk(twcc,chunk);
		return err;
	}

	if ((err = yang_twcc_encode_chunk_two_bit(twcc,chunk, kTwccFbTwoBitElements, yangtrue)) != Yang_Ok) {
		return yang_error_wrap(err, "encode chunk");
	}

	return err;
}

int32_t yang_twcc_encode_remaining_chunk(YangRecvTWCC* twcc,YangRtcpTWCCChunk* chunk)
{
	if (chunk->all_same) {
		return yang_twcc_encode_chunk_run_length(twcc,chunk);
	} else if (chunk->size <= kTwccFbTwoBitElements) {
		// FIXME, TRUE or FALSE
		return yang_twcc_encode_chunk_two_bit(twcc,chunk, chunk->size, yangfalse);
	}
	return yang_twcc_encode_chunk_one_bit(twcc,chunk);
}

int32_t yang_twcc_process_pkt_chunk(YangRecvTWCC* twcc,YangRtcpTWCCChunk* chunk, int32_t delta_size)
{
	int32_t err = Yang_Ok;

	size_t needed_chunk_size = chunk->size == 0 ? kTwccFbChunkBytes : 0;

	size_t might_occupied = twcc->pktLen + needed_chunk_size + delta_size;
	if (might_occupied > kRtcpPacketSize) {
		return yang_error_wrap(ERROR_RTC_RTCP, "might_occupied %zu", might_occupied);
	}

	if (yang_twcc_can_add_to_chunk(twcc,chunk, delta_size)) {

		yang_twcc_add_to_chunk(twcc,chunk, delta_size);
		return err;
	}
	if ((err = yang_twcc_encode_chunk(twcc,chunk)) != Yang_Ok) {
		return yang_error_wrap(err, "encode chunk, delta_size %u", delta_size);
	}
	yang_twcc_add_to_chunk(twcc,chunk, delta_size);
	return err;
}
int32_t yang_twcc_recv_decode(YangRecvTWCC* twcc,YangBuffer *buffer){
	return Yang_Ok;
}



int32_t yang_twcc_recv_encode(YangRecvTWCC* twcc,YangBuffer *buffer){
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

	if(!yang_buffer_require(buffer,kMaxUDPDataSize)) {
		return yang_error_wrap(ERROR_RTC_RTCP, "requires %d bytes", kMaxUDPDataSize);
	}

	twcc->pktLen = kTwccFbPktHeaderSize;

	int i=0;

	if (!twcc->nextBaseSn) {
        twcc->baseSn = twcc->sns.payload[0];
	} else {
		twcc->baseSn = twcc->nextBaseSn;

	}


	int64_t ts =twcc->tss[YANG_GET_RECV_BUFFER_INDEX(twcc->baseSn)];

	twcc->referenceTime = (ts % kTwccFbReferenceTimeDivisor) / kTwccFbTimeMultiplier;
	int64_t last_ts = (int64_t)(twcc->referenceTime) * kTwccFbTimeMultiplier;

	uint16_t last_sn = twcc->baseSn;
	uint16_t packet_count = 0;

	// encode chunk
	YangRtcpTWCCChunk chunk;
	uint16_t current_sn=0;
	int64_t current_ts=0;
	for(i=0; i<twcc->sns.vsize; i++) {
		// check whether exceed buffer len
		if (twcc->pktLen + 2 >= yang_buffer_left(buffer)) {
			break;
		}
		twcc->newSnsIndex=i;
        current_sn = twcc->sns.payload[i];
		current_ts = twcc->tss[YANG_GET_RECV_BUFFER_INDEX(current_sn)];
		// calculate delta
		packet_count++;
		int64_t delta_us = yang_twcc_calculate_delta_us(current_ts, last_ts);
		int16_t delta = delta_us;
		if(delta != delta_us) {
			return yang_error_wrap(ERROR_RTC_RTCP, "twcc: delta:%lld, exceeds the 16bits", delta_us);
		}

        if(current_sn > (last_sn + 1)) {
            // lost packet
            for(uint16_t lost_sn = last_sn + 1; lost_sn < current_sn; ++lost_sn) {
                yang_twcc_process_pkt_chunk(twcc,&chunk, 0);
                packet_count++;
            }
        }

		int32_t recv_delta_size = (delta >= 0 && delta <= 0xff) ? 1 : 2;
		if ((err = yang_twcc_process_pkt_chunk(twcc,&chunk, recv_delta_size)) != Yang_Ok) {
			return yang_error_wrap(err, "delta_size %d, failed to append_recv_delta", recv_delta_size);
		}

		uint16_t tdelta=(uint16_t)delta;
		yang_insert_uint16_tVector(&twcc->pktDeltas,&tdelta);
		last_ts += delta * kTwccFbDeltaUnit;
		twcc->pktLen += recv_delta_size;
		last_sn = current_sn;
	}

	twcc->nextBaseSn = 0;

	if(0 < chunk.size) {
		if((err = yang_twcc_encode_remaining_chunk(twcc,&chunk)) != Yang_Ok) {
			return yang_error_wrap(err, "encode chunk");
		}
	}

	// encode rtcp twcc packet
	if((twcc->pktLen % 4) == 0) {
		twcc->header.length = twcc->pktLen / 4;
	} else {
		twcc->header.length = (twcc->pktLen + 4 - (twcc->pktLen%4)) / 4;
	}
	twcc->header.length -= 1;

	if(Yang_Ok != (err = yang_encode_header_rtcpHeader(&twcc->header,twcc->ssrc,buffer))) {
		return yang_error_wrap(err, "encode header");
	}
	yang_write_4bytes(buffer,twcc->mediaSsrc);
	yang_write_2bytes(buffer,twcc->baseSn);
	yang_write_2bytes(buffer,packet_count);
	yang_write_3bytes(buffer,twcc->referenceTime);
	yang_write_1bytes(buffer,twcc->fb_pkt_count);

	for(i=0; i<twcc->encodedChucks.vsize; i++) {
		yang_write_2bytes(buffer,twcc->encodedChucks.payload[i]);
	}
	for(i=0; i<twcc->pktDeltas.vsize; i++) {
		uint16_t it=twcc->pktDeltas.payload[i];
		if(0 <= it && 0xFF >= it) {
			// small delta
			uint8_t delta = it;
			yang_write_1bytes(buffer,delta);
		} else {
			// large or negative delta
			yang_write_2bytes(buffer,it);
		}
	}
	while((twcc->pktLen % 4) != 0) {
		yang_write_1bytes(buffer,0);
		twcc->pktLen++;
	}

	yang_clear_uint16_tVector(&twcc->encodedChucks);
	yang_clear_uint16_tVector(&twcc->pktDeltas);
	if (err != Yang_Ok || twcc->nextBaseSn == 0) {
		yang_twcc_clear(twcc);
	}

	return err;
}
int32_t yang_twcc_rtcp_decode(YangRtcpCommon* comm,YangBuffer *buffer){

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
	comm->data = buffer->head;
	comm->nb_data = yang_buffer_left(buffer);

	if(Yang_Ok != (err = yang_decode_header_rtcpCommon(comm,buffer))) {
		return yang_error_wrap(err, "twcc decode header");
	}

    comm->payloadLen = (comm->header.length + 1) * 4 - Yang_RtcpHeader_Length - 4;
    yang_read_bytes(buffer,(char *)comm->payload, comm->payloadLen);

	return err;
}
void yang_create_recvtwcc(YangRecvTWCC* twcc){
	if(twcc==NULL) return;
	twcc->header.padding = 0;
	twcc->header.type = YangRtcpType_rtpfb;
	twcc->header.rc = 15;
	twcc->header.version = kRtcpVersion;
	yang_create_uint16Sort2(&twcc->sns,YANG_RTC_RECV_BUFFER_COUNT);
	yang_create_uint16_tVector(&twcc->encodedChucks);
	yang_create_uint16_tVector(&twcc->pktDeltas);
}
void yang_destroy_recvtwcc(YangRecvTWCC* twcc){
	if(twcc==NULL) return;
	yang_destroy_uint16Sort(&twcc->sns);
	yang_destroy_uint16_tVector(&twcc->encodedChucks);
	yang_destroy_uint16_tVector(&twcc->pktDeltas);
}
void yang_create_rtcptwcc(YangRtcpCommon* comm){
	if(comm==NULL) return;
	comm->header.padding = 0;
	comm->header.type = YangRtcpType_rtpfb;
	comm->header.rc = 15;
	comm->header.version = kRtcpVersion;
	comm->twcc=yang_calloc(sizeof(YangRecvTWCC),1);
	yang_create_recvtwcc((YangRecvTWCC*)comm->twcc);
}
void yang_destroy_rtcptwcc(YangRtcpCommon* comm){
	if(comm==NULL||comm->twcc==NULL) return;
	YangRecvTWCC* twcc=(YangRecvTWCC*)comm->twcc;
	yang_destroy_recvtwcc(twcc);
	yang_free(comm->twcc);
}
