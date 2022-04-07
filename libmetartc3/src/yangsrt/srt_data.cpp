//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <string.h>
#include <yangsrt/srt_data.hpp>
SRT_DATA_MSG::SRT_DATA_MSG():_msg_type(SRT_MSG_DATA_TYPE)
    ,_len(0)
    ,_data_p(nullptr)
    //,_key_path(path)
{

}


SRT_DATA_MSG::SRT_DATA_MSG(uint32_t  len):_msg_type(SRT_MSG_DATA_TYPE)
    ,_len(len)
    //,_key_path(path)
{
    _data_p = new uint8_t[len];
    memset(_data_p, 0, len);
}

SRT_DATA_MSG::SRT_DATA_MSG(uint8_t* data_p, uint32_t  len):_msg_type(SRT_MSG_DATA_TYPE)
    ,_len(len)

{
    _data_p = new uint8_t[len];
    memcpy(_data_p, data_p, len);
}

SRT_DATA_MSG::~SRT_DATA_MSG() {
    if (_data_p && (_len > 0)) {
        delete _data_p;
    }
}

uint32_t  SRT_DATA_MSG::msg_type() {
    return _msg_type;
}


uint32_t  SRT_DATA_MSG::data_len() {
    return _len;
}

uint8_t* SRT_DATA_MSG::get_data() {
    return _data_p;
}
