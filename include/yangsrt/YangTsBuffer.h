//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef __SIMPLE_BUFFER_H__
#define __SIMPLE_BUFFER_H__

#include <vector>
#include <string>
#include <stdint.h>

// only support little endian
class YangTsBuffer
{
public:
    YangTsBuffer();
    YangTsBuffer(int32_t size, char value);
    virtual ~YangTsBuffer();

public:
    void write_1byte(char val);
    void write_2bytes(int16_t val);
    void write_3bytes(int32_t val);
    void write_4bytes(int32_t val);
    void write_8bytes(int64_t val);
    void writeBytes(uint8_t* bytes,int32_t size);
    void append( uint8_t* bytes, int32_t size);

public:
    char read_1byte();
    int16_t read_2bytes();
    int32_t read_3bytes();
    int32_t read_4bytes();
    int64_t read_8bytes();
    void readBytes(uint8_t *p,int32_t len);
    std::string read_string(int32_t len);

public:
    void skip(int32_t size);
    bool require(int32_t required_size);
    bool empty();
    int32_t size();
    int32_t pos();
    uint8_t *data();
    void clear();
    void set_data(int32_t pos, const uint8_t *data, int32_t len);

public:
    std::string to_string();

private:
    uint8_t _data[188*2];
    int32_t _pos;
    int32_t curPos;
};

#endif /* __SIMPLE_BUFFER_H__ */
