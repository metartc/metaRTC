//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangsrt/YangTsBuffer.h>
#include <yangsrt/YangTsPacket.h>
#include <iostream>

TsFrame::TsFrame(){
    _data.reset(new YangTsBuffer);
    pts=dts=pcr=0;
    stream_type=stream_id=0;
    completed=false;
    pid=0;
    expected_pes_packet_length=0;


}

TsFrame::TsFrame(uint8_t st){
	stream_type=st;
	   pts=dts=pcr=0;
	    stream_id=0;
	    completed=false;
	     pid=0;
	     expected_pes_packet_length=0;
    _data.reset(new YangTsBuffer);
}

bool TsFrame::empty()
{
    return _data->size() == 0;
}

void TsFrame::reset()
{
    pid = 0;
    completed = false;
    expected_pes_packet_length = 0;
    _data.reset(new YangTsBuffer);
}

TsHeader::TsHeader()
    : sync_byte(0x47)
    , transport_error_indicator(0)
    , payload_unit_start_indicator(0)
    , transport_priority(0)
    , pid(0)
    , transport_scrambling_control(0)
    , adaptation_field_control(0)
    , continuity_counter(0)
{
}

TsHeader::~TsHeader()
{
}

void TsHeader::encode(YangTsBuffer *sb)
{
    sb->write_1byte(sync_byte);

    uint16_t b1b2 = pid & 0x1FFF;
    b1b2 |= (transport_priority << 13) & 0x2000;
    b1b2 |= (payload_unit_start_indicator << 14) & 0x4000;
    b1b2 |= (transport_error_indicator << 15) & 0x8000;
    sb->write_2bytes(b1b2);

    uint8_t b3 = continuity_counter & 0x0F;
    b3 |= (adaptation_field_control << 4) & 0x30;
    b3 |= (transport_scrambling_control << 6) & 0xC0;
    sb->write_1byte(b3);
}

void TsHeader::decode(YangTsBuffer *sb)
{
    sync_byte = sb->read_1byte();

    uint16_t b1b2 = sb->read_2bytes();
    pid = b1b2 & 0x1FFF;
    transport_error_indicator = (b1b2 >> 13) & 0x01;
    payload_unit_start_indicator = (b1b2 >> 14) & 0x01;
    transport_error_indicator = (b1b2 >> 15) & 0x01;

    uint8_t b3 = sb->read_1byte();
    continuity_counter = b3 & 0x0F;
    adaptation_field_control = (b3 >> 4) & 0x03;
    transport_scrambling_control = (b3 >> 6) & 0x03;
}

PATHeader::PATHeader()
    : table_id(0)
    , section_syntax_indicator(0)
    , b0(0)
    , reserved0(0)
    , section_length(0)
    , transport_stream_id(0)
    , reserved1(0)
    , version_number(0)
    , current_next_indicator(0)
    , section_number(0)
    , last_section_number(0)
{

}

PATHeader::~PATHeader()
{

}

void PATHeader::encode(YangTsBuffer *sb)
{
    sb->write_1byte(table_id);

    uint16_t b1b2 = section_length & 0x0FFF;
    b1b2 |= (reserved0 << 12) & 0x3000;
    b1b2 |= (b0 << 14) & 0x4000;
    b1b2 |= (section_syntax_indicator << 15) & 0x8000;
    sb->write_2bytes(b1b2);

    sb->write_2bytes(transport_stream_id);

    uint8_t b5 = current_next_indicator & 0x01;
    b5 |= (version_number << 1) & 0x3E;
    b5 |= (reserved1 << 6) & 0xC0;
    sb->write_1byte(b5);

    sb->write_1byte(section_number);
    sb->write_1byte(last_section_number);
}

void PATHeader::decode(YangTsBuffer *sb)
{
    table_id = sb->read_1byte();

    uint16_t b1b2 = sb->read_2bytes();
    section_syntax_indicator = (b1b2 >> 15) & 0x01;
    b0 = (b1b2 >> 14) & 0x01;
    section_length = b1b2 & 0x0FFF;

    transport_stream_id = sb->read_2bytes();

    uint8_t b5 = sb->read_1byte();
    reserved1 = (b5 >> 6) & 0x03;
    version_number = (b5 >> 1) & 0x1F;
    current_next_indicator = b5 & 0x01;

    section_number = sb->read_1byte();

    last_section_number = sb->read_1byte();
}

void PATHeader::print()
{
    std::cout << "----------PAT information----------" << std::endl;
    std::cout << "table_id: " << std::to_string(table_id) << std::endl;
    std::cout << "section_syntax_indicator: " << std::to_string(section_syntax_indicator) << std::endl;
    std::cout << "b0: " << std::to_string(b0) << std::endl;
    std::cout << "reserved0: " << std::to_string(reserved0) << std::endl;
    std::cout << "section_length: " << std::to_string(section_length) << std::endl;
    std::cout << "transport_stream_id: " << std::to_string(transport_stream_id) << std::endl;
    std::cout << "reserved1: " << std::to_string(reserved1) << std::endl;
    std::cout << "version_number: " << std::to_string(version_number) << std::endl;
    std::cout << "current_next_indicator: " << std::to_string(current_next_indicator) << std::endl;
    std::cout << "section_number: " << std::to_string(section_number) << std::endl;
    std::cout << "last_section_number: " << std::to_string(last_section_number) << std::endl;
    std::cout << std::endl;
    std::flush(std::cout);
}

PMTElementInfo::PMTElementInfo(uint8_t st, uint16_t pid)
    : stream_type(st)
    , reserved0(0x7)
    , elementary_PID(pid)
    , reserved1(0xf)
    , ES_info_length(0)
{

}

PMTElementInfo::PMTElementInfo()
    : PMTElementInfo(0, 0)
{

}

PMTElementInfo::~PMTElementInfo()
{

}

void PMTElementInfo::encode(YangTsBuffer *sb)
{
    sb->write_1byte(stream_type);

    uint16_t b1b2 = elementary_PID & 0x1FFF;
    b1b2 |= (reserved0 << 13) & 0xE000;
    sb->write_2bytes(b1b2);

    int16_t b3b4 = ES_info_length & 0x0FFF;
    b3b4 |= (reserved1 << 12) & 0xF000;
    sb->write_2bytes(b3b4);

    if (ES_info_length > 0) {
        // TODO:
    }
}

void PMTElementInfo::decode(YangTsBuffer *sb)
{
    stream_type = sb->read_1byte();

    uint16_t b1b2 = sb->read_2bytes();
    reserved0 = (b1b2 >> 13) & 0x07;
    elementary_PID = b1b2 & 0x1FFF;

    uint16_t b3b4 = sb->read_2bytes();
    reserved1 = (b3b4 >> 12) & 0xF;
    ES_info_length = b3b4 & 0xFFF;

    if (ES_info_length > 0) {
        ES_info = sb->read_string(ES_info_length);
    }
}

uint16_t PMTElementInfo::size()
{
    return 5 + ES_info_length;
}

void PMTElementInfo::print()
{
    std::cout << "**********PMTElement information**********" << std::endl;
    std::cout << "stream_type: " << std::to_string(stream_type) << std::endl;
    std::cout << "reserved0: " << std::to_string(reserved0) << std::endl;
    std::cout << "elementary_PID: " << std::to_string(elementary_PID) << std::endl;
    std::cout << "reserved1: " << std::to_string(reserved1) << std::endl;
    std::cout << "ES_info_length: " << std::to_string(ES_info_length) << std::endl;
    std::cout << "ES_info: " << ES_info << std::endl;
    std::flush(std::cout);
}

PMTHeader::PMTHeader()
    : table_id(0x02)
    , section_syntax_indicator(0)
    , b0(0)
    , reserved0(0)
    , section_length(0)
    , program_number(0)
    , reserved1(0)
    , version_number(0)
    , current_next_indicator(0)
    , section_number(0)
    , last_section_number(0)
    , reserved2(0)
    , PCR_PID(0)
    , reserved3(0)
    , program_info_length(0)
{

}

PMTHeader::~PMTHeader()
{

}

void PMTHeader::encode(YangTsBuffer *sb)
{
    sb->write_1byte(table_id);

    uint16_t b1b2 = section_length & 0xFFFF;
    b1b2 |= (reserved0 << 12) & 0x3000;
    b1b2 |= (b0 << 14) & 0x4000;
    b1b2 |= (section_syntax_indicator << 15) & 0x8000;
    sb->write_2bytes(b1b2);

    sb->write_2bytes(program_number);

    uint8_t b5 = current_next_indicator & 0x01;
    b5 |= (version_number << 1) & 0x3E;
    b5 |= (reserved1 << 6) & 0xC0;
    sb->write_1byte(b5);

    sb->write_1byte(section_number);
    sb->write_1byte(last_section_number);

    uint16_t b8b9 = PCR_PID & 0x1FFF;
    b8b9 |= (reserved2 << 13) & 0xE000;
    sb->write_2bytes(b8b9);

    uint16_t b10b11 = program_info_length & 0xFFF;
    b10b11 |= (reserved3 << 12) & 0xF000;
    sb->write_2bytes(b10b11);

    for (int32_t i = 0; i < (int)infos.size(); i++) {
        infos[i]->encode(sb);
    }
}

void PMTHeader::decode(YangTsBuffer *sb)
{
    table_id = sb->read_1byte();

    uint16_t b1b2 = sb->read_2bytes();
    section_syntax_indicator = (b1b2 >> 15) & 0x01;
    b0 = (b1b2 >> 14) & 0x01;
    reserved0 = (b1b2 >> 12) & 0x03;
    section_length = b1b2 & 0xFFF;

    program_number = sb->read_2bytes();

    uint8_t b5 = sb->read_1byte();
    reserved1 = (b5 >> 6) & 0x03;
    version_number = (b5 >> 1) & 0x1F;
    current_next_indicator = b5 & 0x01;

    section_number = sb->read_1byte();
    last_section_number = sb->read_1byte();

    uint16_t b8b9 = sb->read_2bytes();
    reserved2 = (b8b9 >> 13) & 0x07;
    PCR_PID = b8b9 & 0x1FFF;

    uint16_t b10b11 = sb->read_2bytes();
    reserved3 = (b10b11 >> 12) & 0xF;
    program_info_length = b10b11 & 0xFFF;

    if (program_info_length > 0) {
        sb->read_string(program_info_length);
    }

    int32_t remain_bytes = section_length - 4 - 9 - program_info_length;
    while (remain_bytes > 0) {
        std::shared_ptr<PMTElementInfo> element_info(new PMTElementInfo);
        element_info->decode(sb);
        infos.push_back(element_info);
        remain_bytes -= element_info->size();
    }
}

uint16_t PMTHeader::size()
{
    uint16_t ret = 12;
    for (int32_t i = 0; i < (int)infos.size(); i++) {
        ret += infos[i]->size();
    }

    return ret;
}

void PMTHeader::print()
{
    std::cout << "----------PMT information----------" << std::endl;
    std::cout << "table_id: " << std::to_string(table_id) << std::endl;
    std::cout << "section_syntax_indicator: " << std::to_string(section_syntax_indicator) << std::endl;
    std::cout << "b0: " << std::to_string(b0) << std::endl;
    std::cout << "reserved0: " << std::to_string(reserved0) << std::endl;
    std::cout << "section_length: " << std::to_string(section_length) << std::endl;
    std::cout << "program_number: " << std::to_string(program_number) << std::endl;
    std::cout << "reserved1: " << std::to_string(reserved1) << std::endl;
    std::cout << "version_number: " << std::to_string(version_number) << std::endl;
    std::cout << "current_next_indicator: " << std::to_string(current_next_indicator) << std::endl;
    std::cout << "section_number: " << std::to_string(section_number) << std::endl;
    std::cout << "last_section_number: " << std::to_string(last_section_number) << std::endl;
    std::cout << "reserved2: " << std::to_string(reserved2) << std::endl;
    std::cout << "PCR_PID: " << std::to_string(PCR_PID) << std::endl;
    std::cout << "reserved3: " << std::to_string(reserved3) << std::endl;
    std::cout << "program_info_length: " << std::to_string(program_info_length) << std::endl;
    for (int32_t i = 0; i < (int)infos.size(); i++) {
        infos[i]->print();
    }
    std::cout << std::endl;
    std::flush(std::cout);
}

AdaptationFieldHeader::AdaptationFieldHeader()
    : adaptation_field_length(0)
    , adaptation_field_extension_flag(0)
    , transport_private_data_flag(0)
    , splicing_point_flag(0)
    , opcr_flag(0)
    , pcr_flag(0)
    , elementary_stream_priority_indicator(0)
    , random_access_indicator(0)
    , discontinuity_indicator(0)
{

}

AdaptationFieldHeader::~AdaptationFieldHeader()
{

}

void AdaptationFieldHeader::encode(YangTsBuffer *sb)
{
    sb->write_1byte(adaptation_field_length);
    if (adaptation_field_length != 0) {
        uint8_t val = adaptation_field_extension_flag & 0x01;
        val |= (transport_private_data_flag << 1) & 0x02;
        val |= (splicing_point_flag << 2) & 0x04;
        val |= (opcr_flag << 3) & 0x08;
        val |= (pcr_flag << 4) & 0x10;
        val |= (elementary_stream_priority_indicator << 5) & 0x20;
        val |= (random_access_indicator << 6) & 0x40;
        val |= (discontinuity_indicator << 7) & 0x80;
        sb->write_1byte(val);
    }
}

void AdaptationFieldHeader::decode(YangTsBuffer *sb)
{
    adaptation_field_length = sb->read_1byte();
    if (adaptation_field_length != 0) {
        uint8_t val = sb->read_1byte();
        adaptation_field_extension_flag = val & 0x01;
        transport_private_data_flag = (val >> 1) & 0x01;
        splicing_point_flag = (val >> 2) & 0x01;
        opcr_flag = (val >> 3) & 0x01;
        pcr_flag = (val >> 4) & 0x01;
        elementary_stream_priority_indicator = (val >> 5) & 0x01;
        random_access_indicator = (val >> 6) & 0x01;
        discontinuity_indicator = (val >> 7) & 0x01;
    }
}

PESHeader::PESHeader()
    : packet_start_code(0x000001)
    , stream_id(0)
    , pes_packet_length(0)
    , original_or_copy(0)
    , copyright(0)
    , data_alignment_indicator(0)
    , pes_priority(0)
    , pes_scrambling_control(0)
    , marker_bits(0x02)
    , pes_ext_flag(0)
    , pes_crc_flag(0)
    , add_copy_info_flag(0)
    , dsm_trick_mode_flag(0)
    , es_rate_flag(0)
    , escr_flag(0)
    , pts_dts_flags(0)
    , header_data_length(0)
{

}

PESHeader::~PESHeader()
{

}

void PESHeader::encode(YangTsBuffer *sb)
{
    uint32_t b0b1b2b3 = (packet_start_code << 8) & 0xFFFFFF00;
    b0b1b2b3 |= stream_id & 0xFF;
    sb->write_4bytes(b0b1b2b3);

    sb->write_2bytes(pes_packet_length);

    uint8_t b6 = original_or_copy & 0x01;
    b6 |= (copyright << 1) & 0x02;
    b6 |= (data_alignment_indicator << 2) & 0x04;
    b6 |= (pes_priority << 3) & 0x08;
    b6 |= (pes_scrambling_control << 4) & 0x30;
    b6 |= (marker_bits << 6) & 0xC0;
    sb->write_1byte(b6);

    uint8_t b7 = pes_ext_flag & 0x01;
    b7 |= (pes_crc_flag << 1) & 0x02;
    b7 |= (add_copy_info_flag << 2) & 0x04;
    b7 |= (dsm_trick_mode_flag << 3) & 0x08;
    b7 |= (es_rate_flag << 4) & 0x10;
    b7 |= (escr_flag << 5) & 0x20;
    b7 |= (pts_dts_flags << 6) & 0xC0;
    sb->write_1byte(b7);

    sb->write_1byte(header_data_length);
}

void PESHeader::decode(YangTsBuffer *sb)
{
    uint32_t b0b1b2b3 = sb->read_4bytes();
    packet_start_code = (b0b1b2b3 >> 8) & 0x00FFFFFF;
    stream_id = (b0b1b2b3) & 0xFF;

    pes_packet_length = sb->read_2bytes();

    uint8_t b6 = sb->read_1byte();
    original_or_copy = b6 & 0x01;
    copyright = (b6 >> 1) & 0x01;
    data_alignment_indicator = (b6 >> 2) & 0x01;
    pes_priority = (b6 >> 3) & 0x01;
    pes_scrambling_control = (b6 >> 4) & 0x03;
    marker_bits = (b6 >> 6) & 0x03;

    uint8_t b7 = sb->read_1byte();
    pes_ext_flag = b7 & 0x01;
    pes_crc_flag = (b7 >> 1) & 0x01;
    add_copy_info_flag = (b7 >> 2) & 0x01;
    dsm_trick_mode_flag = (b7 >> 3) & 0x01;
    es_rate_flag = (b7 >> 4) & 0x01;
    escr_flag = (b7 >> 5) & 0x01;
    pts_dts_flags = (b7 >> 6) & 0x03;

    header_data_length = sb->read_1byte();
}
