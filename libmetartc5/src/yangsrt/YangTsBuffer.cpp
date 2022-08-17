//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <assert.h>
#include <memory.h>
#include <yangsrt/YangTsBuffer.h>
#include <stdint.h>
YangTsBuffer::YangTsBuffer()
    : _pos(0)
{
	curPos=0;
}

YangTsBuffer::YangTsBuffer(int32_t size, char value)
    : _pos(0)
{
	curPos=0;
}

YangTsBuffer::~YangTsBuffer()
{
}

void YangTsBuffer::write_1byte(char val)
{
	_data[curPos++]=val;
}

void YangTsBuffer::write_2bytes(int16_t val)
{
    char *p = (char *)&val;

    for (int32_t i = 1; i >= 0; --i) {
    	_data[curPos++]=p[i];

    }
}

void YangTsBuffer::write_3bytes(int32_t val)
{
    char *p = (char *)&val;

    for (int32_t i = 2; i >= 0; --i) {
    	_data[curPos++]=p[i];

    }
}

void YangTsBuffer::write_4bytes(int32_t val)
{
    char *p = (char *)&val;

    for (int32_t i = 3; i >= 0; --i) {
    	_data[curPos++]=p[i];
    }
}

void YangTsBuffer::write_8bytes(int64_t val)
{
    char *p = (char *)&val;

    for (int32_t i = 7; i >= 0; --i) {
    	_data[curPos++]=p[i];
    }
}
void YangTsBuffer::writeBytes(uint8_t* bytes,int32_t size){
	memcpy(_data+curPos,bytes,size);
	   curPos+=size;
}


void YangTsBuffer::append( uint8_t* bytes, int32_t size)
{
    if (!bytes || size <= 0)
        return;
    memcpy(_data+curPos,bytes,size);
    curPos+=size;
}

char YangTsBuffer::read_1byte()
{
    assert(require(1));

    char val = _data[_pos];
    _pos++;

    return val;
}

int16_t YangTsBuffer::read_2bytes()
{
    assert(require(2));

    int16_t val = 0;
    char *p = (char *)&val;

    for (int32_t i = 1; i >= 0; --i) {
        p[i] = _data[_pos];
        _pos++;
    }

    return val;
}

int32_t YangTsBuffer::read_3bytes()
{
    assert(require(3));

    int32_t val = 0;
    char *p = (char *)&val;

    for (int32_t i = 2; i >= 0; --i) {
        p[i] =  _data[_pos];//_data.at(0 + _pos);
        _pos++;
    }

    return val;
}

int32_t YangTsBuffer::read_4bytes()
{
    assert(require(4));

    int32_t val = 0;
    char *p = (char *)&val;

    for (int32_t i = 3; i >= 0; --i) {
        p[i] =  _data[_pos];
        _pos++;
    }

    return val;
}

int64_t YangTsBuffer::read_8bytes()
{
    assert(require(8));

    int64_t val = 0;
    char *p = (char *)&val;

    for (int32_t i = 7; i >= 0; --i) {
        p[i] =  _data[_pos];
        _pos++;
    }

    return val;
}
void YangTsBuffer::readBytes(uint8_t *p,int32_t len){
	memcpy(p,_data+_pos,len);
	_pos += len;
}
std::string YangTsBuffer::read_string(int32_t len)
{
    assert(require(len));

    std::string val((char*)_data + _pos, len);
    _pos += len;

    return val;
}

void YangTsBuffer::skip(int32_t size)
{
    _pos += size;
}

bool YangTsBuffer::require(int32_t required_size)
{
    assert(required_size >= 0);

    return required_size <= curPos-_pos;
}

bool YangTsBuffer::empty()
{
    return _pos >= curPos;
}

int32_t YangTsBuffer::size()
{
    return curPos;
}

int32_t YangTsBuffer::pos()
{
    return _pos;
}

uint8_t *YangTsBuffer::data()
{
    return (size() == 0) ? nullptr : _data;
}

void YangTsBuffer::clear()
{
    _pos = 0;
    curPos=0;

}

void YangTsBuffer::set_data(int32_t pos, const uint8_t *data, int32_t len)
{
    if (!data)
        return;

    if (pos + len > size()) {
        return;
    }

    for (int32_t i = 0; i < len; i++) {
        _data[pos + i] = data[i];
    }
}

std::string YangTsBuffer::to_string()
{
    return std::string(_data, _data+curPos);
}
