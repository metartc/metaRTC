/*
 * YangTsdemux.h
 *
 *  Created on: 2020年8月15日
 *      Author: yang
 */

#ifndef YANGSRT_INCLUDE_YANGTSDEMUX_H_
#define YANGSRT_INCLUDE_YANGTSDEMUX_H_
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include "srt_data.hpp"
#include "yangutil/YangErrorCode.h"
#include "YangTsPid.h"


class ts_media_data_callback_I {
public:
    virtual void on_data_callback(SRT_DATA_MSG_PTR data_ptr, uint32_t  media_type, uint64_t dts, uint64_t pts) = 0;
};

typedef std::shared_ptr<ts_media_data_callback_I> TS_DATA_CALLBACK_PTR;

class adaptation_field {
public:
    adaptation_field(){};
    ~adaptation_field(){};

public:
    uint8_t _adaptation_field_length;

    uint8_t _discontinuity_indicator:1;
    uint8_t _random_access_indicator:1;
    uint8_t _elementary_stream_priority_indicator:1;
    uint8_t _PCR_flag:1;
    uint8_t _OPCR_flag:1;
    uint8_t _splicing_point_flag:1;
    uint8_t _transport_private_data_flag:1;
    uint8_t _adaptation_field_extension_flag:1;

    //if(PCR_flag == '1')
    unsigned long _program_clock_reference_base;//33 bits
    unsigned short _program_clock_reference_extension;//9bits
    //if (OPCR_flag == '1')
    unsigned long _original_program_clock_reference_base;//33 bits
    unsigned short _original_program_clock_reference_extension;//9bits
    //if (splicing_point_flag == '1')
    uint8_t _splice_countdown;
    //if (transport_private_data_flag == '1')
    uint8_t _transport_private_data_length;
    uint8_t _private_data_byte[256];
    //if (adaptation_field_extension_flag == '1')
    uint8_t _adaptation_field_extension_length;
    uint8_t _ltw_flag;
    uint8_t _piecewise_rate_flag;
    uint8_t _seamless_splice_flag;
    uint8_t _reserved0;
    //if (ltw_flag == '1')
    unsigned short _ltw_valid_flag:1;
    unsigned short _ltw_offset:15;
    //if (piecewise_rate_flag == '1')
    uint32_t  _piecewise_rate;//22bits
    //if (seamless_splice_flag == '1')
    uint8_t _splice_type;//4bits
    uint8_t _DTS_next_AU1;//3bits
    uint8_t _marker_bit1;//1bit
    unsigned short _DTS_next_AU2;//15bit
    uint8_t _marker_bit2;//1bit
    unsigned short _DTS_next_AU3;//15bit
};

class ts_header {
public:
    ts_header(){}
    ~ts_header(){}

public:
    uint8_t _sync_byte;

    unsigned short _transport_error_indicator:1;
    unsigned short _payload_unit_start_indicator:1;
    unsigned short _transport_priority:1;
    unsigned short _PID:13;

    uint8_t _transport_scrambling_control:2;
    uint8_t _adaptation_field_control:2;
    uint8_t _continuity_counter:4;

    adaptation_field _adaptation_field_info;
};

typedef struct {
    unsigned short _program_number;
    unsigned short _pid;
    unsigned short _network_id;
} PID_INFO;

class pat_info {
public:
    pat_info(){};
    ~pat_info(){};

public:
    uint8_t _table_id;

    unsigned short _section_syntax_indicator:1;
    unsigned short _reserved0:1;
    unsigned short _reserved1:2;
    unsigned short _section_length:12;

    unsigned short _transport_stream_id;

    uint8_t _reserved3:2;
    uint8_t _version_number:5;
    uint8_t _current_next_indicator:1;

    uint8_t _section_number;
    uint8_t _last_section_number;
    std::vector<PID_INFO> _pid_vec;
};

typedef struct {
    uint8_t  _stream_type;
    unsigned short _reserved1:3;
    unsigned short _elementary_PID:13;
    unsigned short _reserved:4;
    unsigned short _ES_info_length;
    uint8_t  _dscr[4096];
    uint32_t    _crc_32;
} STREAM_PID_INFO;

class pmt_info {
public:
    pmt_info(){};
    ~pmt_info(){};
public:
    uint8_t _table_id;
    unsigned short _section_syntax_indicator:1;
    unsigned short _reserved1:1;
    unsigned short _reserved2:2;
    unsigned short _section_length:12;
    unsigned short _program_number:16;
    uint8_t  _reserved:2;
    uint8_t  _version_number:5;
    uint8_t  _current_next_indicator:5;
    uint8_t  _section_number;
    uint8_t  _last_section_number;
    unsigned short _reserved3:3;
    unsigned short _PCR_PID:13;
    unsigned short _reserved4:4;
    unsigned short _program_info_length:12;
    uint8_t  _dscr[4096];

    std::unordered_map<unsigned short, uint8_t> _pid2steamtype;
    std::vector<STREAM_PID_INFO> _stream_pid_vec;
};

class YangTsdemux {
public:
	YangTsdemux();
	virtual ~YangTsdemux();
	 int32_t decode(SRT_DATA_MSG_PTR data_ptr, ts_media_data_callback_I* callback);

	private:
	    int32_t decode_unit(uint8_t* data_p,ts_media_data_callback_I* callback);
	    bool is_pmt(unsigned short pmt_id);
	    int32_t pes_parse(uint8_t* p, size_t npos, uint8_t** ret_pp, size_t& ret_size,
	            uint64_t& dts, uint64_t& pts,int32_t *pesLen);
	    void insert_into_databuf(uint8_t* data_p, size_t data_size, unsigned short pid);
	    void on_callback(ts_media_data_callback_I* callback, unsigned short pid, uint64_t dts, uint64_t pts);

	private:
	    std::string _key_path;//only for srt

	    pat_info _pat;
	    pmt_info _pmt;
	    std::map<int,std::vector<SRT_DATA_MSG_PTR>> _data_buffer_map;
	   //std::vector<SRT_DATA_MSG_PTR> _data_buffer_vec;
	    size_t _data_total;
	    unsigned short _last_pid;
	    uint64_t _last_dts;
	    uint64_t _last_pts;
};
typedef std::shared_ptr<YangTsdemux> TS_DEMUX_PTR;
#endif /* YANGSRT_INCLUDE_YANGTSDEMUX_H_ */
