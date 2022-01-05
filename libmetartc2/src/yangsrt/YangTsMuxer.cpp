/*
 * YangTsMux.cpp
 *
 *  Created on: 2020年8月7日
 *      Author: yang
 */

#include <string.h>
#include <yangutil/yangtype.h>
#include <yangsrt/YangTsMuxer.h>

#include <yangsrt/common.h>
#include <yangsrt/crc.h>
static const uint16_t MPEGTS_NULL_PACKET_PID = 0x1FFF;
static const uint16_t MPEGTS_PAT_PID = 0x00;
static const uint16_t MPEGTS_PMT_PID = 0x100;
static const uint16_t MPEGTS_PCR_PID = 0x110;


YangTsMuxer::YangTsMuxer() {
	m_pmt_pid = MPEGTS_PMT_PID;
    m_stream_pid_map[STREAM_TYPE_VIDEO_H264]=Yang_H264_PID;
    m_stream_pid_map[STREAM_TYPE_VIDEO_HEVC]=Yang_H265_PID;
    m_stream_pid_map[STREAM_TYPE_AUDIO_AAC]=Yang_AAC_PID;
    m_stream_pid_map[STREAM_TYPE_AUDIO_OPUS]=Yang_OPUS_PID;
    m_stream_pid_map[STREAM_TYPE_PRIVATE_DATA]=Yang_PRIVATE_PID;


    /**m_stream_pid_map.insert(
			pair<uint8_t, int>(STREAM_TYPE_VIDEO_H264, Yang_H264_PID));
	m_stream_pid_map.insert(
			pair<uint8_t, int>(STREAM_TYPE_VIDEO_HEVC, Yang_H265_PID));
	m_stream_pid_map.insert(
			pair<uint8_t, int>(STREAM_TYPE_AUDIO_AAC, Yang_AAC_PID));
	m_stream_pid_map.insert(
			pair<uint8_t, int>(STREAM_TYPE_AUDIO_OPUS, Yang_OPUS_PID));
	m_stream_pid_map.insert(
            pair<uint8_t, int>(STREAM_TYPE_PRIVATE_DATA, Yang_PRIVATE_PID));**/
	current_index = 0;

}

YangTsMuxer::~YangTsMuxer() {

}

void YangTsMuxer::create_pat(YangTsBuffer *sb, uint16_t pmt_pid, uint8_t cc) {
	YangTsBuffer pat_sb;
	TsHeader ts_header;
	ts_header.sync_byte = 0x47;
	ts_header.transport_error_indicator = 0;
	ts_header.payload_unit_start_indicator = 1;
	ts_header.transport_priority = 0;
	ts_header.pid = MPEGTS_PAT_PID;
	ts_header.transport_scrambling_control = 0;
	ts_header.adaptation_field_control =
			MpegTsAdaptationFieldType::payload_only;
	ts_header.continuity_counter = cc;

	AdaptationFieldHeader adapt_field;

	PATHeader pat_header;
	pat_header.table_id = 0x00;
	pat_header.section_syntax_indicator = 1;
	pat_header.b0 = 0;
	pat_header.reserved0 = 0x3;
	pat_header.transport_stream_id = 0;
	pat_header.reserved1 = 0x3;
	pat_header.version_number = 0;
	pat_header.current_next_indicator = 1;
	pat_header.section_number = 0x0;
	pat_header.last_section_number = 0x0;

	//program_number
	uint16_t program_number = 0x0001;
	//program_map_PID
	uint16_t program_map_PID = 0xe000 | (pmt_pid & 0x1fff);

	uint32_t  section_length = 4 + 4 + 5;
	pat_header.section_length = section_length & 0x3ff;

	ts_header.encode(&pat_sb);
	adapt_field.encode(&pat_sb);
	pat_header.encode(&pat_sb);
	pat_sb.write_2bytes(program_number);
	pat_sb.write_2bytes(program_map_PID);

	// crc32
	uint32_t crc_32 = crc32((uint8_t*) pat_sb.data() + 5, pat_sb.size() - 5);
	pat_sb.write_4bytes(crc_32);

	int32_t suffLen = 188 - pat_sb.size();
    //uint8_t stuff[suffLen];
    uint8_t* stuff=new uint8_t[suffLen];
    YangAutoFreeA(uint8_t,stuff);
	memset(stuff, 0xff, suffLen);
	pat_sb.writeBytes(stuff, 188 - suffLen);
	// memcpy()
	//std::string stuff(188 - pat_sb.size(), 0xff);
	//pat_sb.write_string(stuff);

	sb->append(pat_sb.data(), pat_sb.size());
}

void YangTsMuxer::create_pmt(YangTsBuffer *sb, uint8_t cc) {
	YangTsBuffer pmt_sb;
	TsHeader ts_header;
	ts_header.sync_byte = 0x47;
	ts_header.transport_error_indicator = 0;
	ts_header.payload_unit_start_indicator = 1;
	ts_header.transport_priority = 0;
	ts_header.pid = m_pmt_pid;
	ts_header.transport_scrambling_control = 0;
	ts_header.adaptation_field_control =
			MpegTsAdaptationFieldType::payload_only;
	ts_header.continuity_counter = cc;

	AdaptationFieldHeader adapt_field;

	PMTHeader pmt_header;
	pmt_header.table_id = 0x02;
	pmt_header.section_syntax_indicator = 1;
	pmt_header.b0 = 0;
	pmt_header.reserved0 = 0x3;
	pmt_header.section_length = 0;
	pmt_header.program_number = 0x0001;
	pmt_header.reserved1 = 0x3;
	pmt_header.version_number = 0;
	pmt_header.current_next_indicator = 1;
	pmt_header.section_number = 0x00;
	pmt_header.last_section_number = 0x00;
	pmt_header.reserved2 = 0x7;
	pmt_header.reserved3 = 0xf;
	pmt_header.program_info_length = 0;
	for (auto it = m_stream_pid_map.begin(); it != m_stream_pid_map.end();
			it++) {
		pmt_header.infos.push_back(
				std::shared_ptr<PMTElementInfo>(
						new PMTElementInfo(it->first, it->second)));
		if (it->first == MpegTsStream::AVC) {
			pmt_header.PCR_PID = it->second;
		}
	}

	uint16_t section_length = pmt_header.size() - 3 + 4;
	pmt_header.section_length = section_length & 0x3ff;

	ts_header.encode(&pmt_sb);
	adapt_field.encode(&pmt_sb);
	pmt_header.encode(&pmt_sb);

	// crc32
	uint32_t crc_32 = crc32((uint8_t*) pmt_sb.data() + 5, pmt_sb.size() - 5);
	pmt_sb.write_4bytes(crc_32);

	int32_t suffLen = 188 - pmt_sb.size();
    //uint8_t stuff[suffLen];
    uint8_t* stuff=new uint8_t[suffLen];
    YangAutoFreeA(uint8_t,stuff);
	memset(stuff, 0xff, suffLen);
	pmt_sb.writeBytes(stuff, 188 - suffLen);
	// std::string stuff(188 - pmt_sb.size(), 0xff);
	// pmt_sb.write_string(stuff);

	sb->append(pmt_sb.data(), pmt_sb.size());
}
void YangTsMuxer::create_pes(YangTsPes *frame, uint8_t *p, int32_t plen,
		int32_t frametype, int64_t timestamp, YangTsStream pstreamType) {
	YangTsBuffer packet;
	PESHeader pes_header;
	uint8_t streamType = STREAM_TYPE_VIDEO_H264;
	if (pstreamType == TS_H264) {
		streamType = PES_VIDEO_ID;   //STREAM_TYPE_VIDEO_H264;
		frame->pid = Yang_H264_PID;
		frame->dts = timestamp;
		frame->pts = frame->dts;
	}
	if (pstreamType == TS_H265) {
		streamType = PES_VIDEO_ID;   //STREAM_TYPE_VIDEO_H265;
		frame->pid = Yang_H265_PID;
		frame->dts = timestamp;
		frame->pts =  timestamp;
	}
	if (pstreamType == TS_AAC) {
		streamType = PES_AUDIO_ID;   //STREAM_TYPE_AUDIO_AAC;
		frame->pid = Yang_AAC_PID;
		frame->pts = frame->dts = timestamp;
	}
	if (pstreamType == TS_OPUS) {
		streamType = PES_AUDIO_ID;   //STREAM_TYPE_AUDIO_OPUS;
		frame->pid = Yang_OPUS_PID;
		frame->pts = frame->dts = timestamp;
	}
	if (pstreamType == TS_PRIVATE) {
		streamType = STREAM_TYPE_PRIVATE_DATA;   //STREAM_TYPE_AUDIO_OPUS;
		frame->pid = Yang_PRIVATE_PID;

	}

	frame->stream_type = streamType;

	pes_header.packet_start_code = 0x000001;
	pes_header.stream_id = frame->stream_type;
	pes_header.marker_bits = 0x02;
	pes_header.original_or_copy = 0x01;

	if (frame->pts != frame->dts) {
		pes_header.pts_dts_flags = 0x03;
		pes_header.header_data_length = 0x0A;
	} else {
		pes_header.pts_dts_flags = 0x2;
		pes_header.header_data_length = 0x05;
	}

	uint32_t pes_size = (pes_header.header_data_length + frame->len + 3);
	//printf("\n*****************pes=====%d,",pes_size);
	pes_header.pes_packet_length = pes_size > 0xffff ? 0 : pes_size;
	pes_header.encode(&packet);

	if (pes_header.pts_dts_flags == 0x03) {
		write_pts(&packet, 3, frame->pts);
		write_pts(&packet, 1, frame->dts);
	} else {
		write_pts(&packet, 2, frame->pts);
	}
	memcpy(frame->data, packet.data(), packet.size());
	memcpy(frame->data + packet.size(), p, plen);
	frame->len = plen + packet.size();
}
void YangTsMuxer::create_ts(YangTsPes *frame, vector<YangTsBuffer> *sb) {
	bool first = true;
	//   while (!frame->_data->empty()) {
	while (frame->pos < frame->len) {
		YangTsBuffer packet;

		TsHeader ts_header;
		ts_header.pid = frame->pid;
		ts_header.adaptation_field_control =
				MpegTsAdaptationFieldType::payload_only;
		ts_header.continuity_counter = get_cc(frame->stream_type);

		if (first) {
			ts_header.payload_unit_start_indicator = 0x01;
			if (frame->stream_type == STREAM_TYPE_VIDEO_H264
					|| frame->stream_type == STREAM_TYPE_VIDEO_HEVC) {
				ts_header.adaptation_field_control |= 0x02;
				AdaptationFieldHeader adapt_field_header;
				adapt_field_header.adaptation_field_length = 0x07;
				adapt_field_header.random_access_indicator = 0x01;
				adapt_field_header.pcr_flag = 0x01;

				ts_header.encode(&packet);
				adapt_field_header.encode(&packet);
				write_pcr(&packet, frame->dts);
			} else {
				ts_header.encode(&packet);
			}
			//pes handle

			first = false;
		} else {
			ts_header.encode(&packet);
		}
		// printf("\n********tsheader===%d\n",packet.size());
		uint32_t pos = packet.size();
		uint32_t body_size = 188 - pos;
        uint8_t* bodys=new uint8_t[body_size];
        YangAutoFreeA(uint8_t,bodys);
        memset(bodys,0,body_size);
		packet.writeBytes(bodys, body_size);
		// packet.write_string(std::string(body_size, 0));
		packet.skip(pos);
		uint32_t in_size = frame->len - frame->pos;
		if (body_size <= in_size) { // MpegTsAdaptationFieldType::payload_only or MpegTsAdaptationFieldType::payload_adaption_both for AVC
//             packet.write_string(frame->_data->read_string(body_size));
				// std::string body_string = frame->_data->read_string(body_size);
			packet.set_data(pos, frame->data + frame->pos, body_size);
			frame->pos += body_size;
		} else {
			uint16_t stuff_size = body_size - in_size;
			if (ts_header.adaptation_field_control
					== MpegTsAdaptationFieldType::adaption_only
					|| ts_header.adaptation_field_control
							== MpegTsAdaptationFieldType::payload_adaption_both) {
				uint8_t *base = packet.data() + 5 + packet.data()[4];
				packet.set_data(base - packet.data() + stuff_size, base,
						packet.data() + packet.pos() - base);
				memset(base, 0xff, stuff_size);
				packet.skip(stuff_size);
				packet.data()[4] += stuff_size;
			} else {
				// adaptation_field_control |= 0x20 == MpegTsAdaptationFieldType::payload_adaption_both
				packet.data()[3] |= 0x20;
				packet.set_data(188 - 4 - stuff_size, packet.data() + 4,
						packet.pos() - 4);
				packet.skip(stuff_size);
				packet.data()[4] = stuff_size - 1;
				if (stuff_size >= 2) {
					packet.data()[5] = 0;
					memset(&(packet.data()[6]), 0xff, stuff_size - 2);
				}
			}
//             packet.write_string(frame->_data->read_string(in_size));
			// std::string body_string = frame->_data->read_string(in_size);
			//packet.set_data(packet.pos(), body_string.c_str(), body_string.length());
			packet.set_data(packet.pos(), frame->data + frame->pos, in_size);
			frame->pos += in_size;
		}
		sb->push_back(packet);
		// sb->append(packet.data(), packet.size());
	}
}

void YangTsMuxer::create_pcr(YangTsBuffer *sb) {
	uint64_t pcr = 0;
	TsHeader ts_header;
	ts_header.sync_byte = 0x47;
	ts_header.transport_error_indicator = 0;
	ts_header.payload_unit_start_indicator = 0;
	ts_header.transport_priority = 0;
	ts_header.pid = MPEGTS_PCR_PID;
	ts_header.transport_scrambling_control = 0;
	ts_header.adaptation_field_control =
			MpegTsAdaptationFieldType::adaption_only;
	ts_header.continuity_counter = 0;

	AdaptationFieldHeader adapt_field;
	adapt_field.adaptation_field_length = 188 - 4 - 1;
	adapt_field.discontinuity_indicator = 0;
	adapt_field.random_access_indicator = 0;
	adapt_field.elementary_stream_priority_indicator = 0;
	adapt_field.pcr_flag = 1;
	adapt_field.opcr_flag = 0;
	adapt_field.splicing_point_flag = 0;
	adapt_field.transport_private_data_flag = 0;
	adapt_field.adaptation_field_extension_flag = 0;

	// char *p = sb->data();
	ts_header.encode(sb);
	adapt_field.encode(sb);
	write_pcr(sb, pcr);
}

void YangTsMuxer::create_null(YangTsBuffer *sb) {
	TsHeader ts_header;
	ts_header.sync_byte = 0x47;
	ts_header.transport_error_indicator = 0;
	ts_header.payload_unit_start_indicator = 0;
	ts_header.transport_priority = 0;
	ts_header.pid = MPEGTS_NULL_PACKET_PID;
	ts_header.transport_scrambling_control = 0;
	ts_header.adaptation_field_control =
			MpegTsAdaptationFieldType::payload_only;
	ts_header.continuity_counter = 0;
	ts_header.encode(sb);
}

void YangTsMuxer::encode(uint8_t *p, int32_t plen, int32_t frametype,
		int64_t timestamp, YangTsStream streamType,
		vector<YangTsBuffer> *sb) {
	if (should_create_pat()) {
		encodePmtWithoutData(sb);
	}
	YangTsPes *frame = (YangTsPes*) malloc(sizeof(YangTsPes));
	memset(frame, 0, sizeof(YangTsPes));
	frame->data = (uint8_t*) malloc(plen + 20);
	frame->len = plen;
	create_pes(frame, p, plen, frametype, timestamp, streamType);
	create_ts(frame, sb);
	free(frame->data);
	frame->data = nullptr;
	free(frame);
	frame = nullptr;
}
void YangTsMuxer::encodeWithPmt(uint8_t *p, int32_t plen, int32_t frametype,
		int64_t timestamp, YangTsStream streamType,
		vector<YangTsBuffer> *sb) {
	encodePmtWithoutData(sb);

	YangTsPes *frame = (YangTsPes*) malloc(sizeof(YangTsPes));
	memset(frame, 0, sizeof(YangTsPes));
	frame->data = (uint8_t*) malloc(plen + 20);
	frame->len = plen;
	create_pes(frame, p, plen, frametype, timestamp, streamType);
	create_ts(frame, sb);
	free(frame->data);
	frame->data = nullptr;
	free(frame);
	frame = nullptr;
}
//YangTsMuxer::current_index=0;
//void YangTsMuxer::encodePmt(vector<YangTsBuffer> *sb) {
//	if (should_create_pat()) {
//		encodePmtWithoutData(sb);
//	}
//}
uint8_t YangTsMuxer::get_cc(uint32_t with_pid) {
	if (_pid_cc_map.find(with_pid) != _pid_cc_map.end()) {
		_pid_cc_map[with_pid] = (_pid_cc_map[with_pid] + 1) & 0x0F;
		return _pid_cc_map[with_pid];
	}

	_pid_cc_map[with_pid] = 0;
	return 0;
}
void YangTsMuxer::encodePmtWithoutData(vector<YangTsBuffer> *sb){
	current_index=0;
	sb->push_back(YangTsBuffer());
	uint8_t pat_pmt_cc = get_cc(0);
	create_pat(&sb->at(0), m_pmt_pid, pat_pmt_cc);
	sb->push_back(YangTsBuffer());
	create_pmt(&sb->at(1), pat_pmt_cc);
}
bool YangTsMuxer::should_create_pat() {
	bool ret = false;

	if (current_index % pat_interval == 0) {
		if (current_index > 0) {
			current_index = 0;
		}
		ret = true;
	}

	current_index++;

	return ret;
}

