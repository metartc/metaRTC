//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef INCLUDE_YANGUTIL_BUFFER_YANGCBUFFER_H_
#define INCLUDE_YANGUTIL_BUFFER_YANGCBUFFER_H_

#include <yangutil/yangtype.h>

typedef struct{
    // current position at bytes.
    char* head;
    // the bytes data for buffer to read or write.
    char* data;
    // the total number of bytes.
    int32_t size;
}YangBuffer;

void yang_init_buffer(YangBuffer* buf,char* b, int32_t nn);
void yang_destroy_buffer(YangBuffer* buf);
int32_t yang_buffer_pos(YangBuffer* buf);
 // Left bytes in buffer, total size() minus the current pos().
 int32_t yang_buffer_left(YangBuffer* buf);
 // Whether buffer is empty.
 int32_t yang_buffer_empty(YangBuffer* buf);
 // Whether buffer is able to supply required size of bytes.
 // @remark User should check buffer by require then do read/write.
 // @remark Assert the required_size is not negative.
 int32_t yang_buffer_require(YangBuffer* buf,int32_t required_size);
 void yang_buffer_skip(YangBuffer* buf,int32_t size);
// Write 1bytes char to buffer.
void yang_write_1bytes(YangBuffer* buf,char value);
// Write 2bytes int32_t to buffer.
void yang_write_2bytes(YangBuffer* buf,int16_t value);
void yang_write_le2bytes(YangBuffer* buf,int16_t value);
// Write 4bytes int32_t to buffer.
void yang_write_4bytes(YangBuffer* buf,int32_t value);
void yang_write_le4bytes(YangBuffer* buf,int32_t value);
// Write 3bytes int32_t to buffer.
void yang_write_3bytes(YangBuffer* buf,int32_t value);
void yang_write_le3bytes(YangBuffer* buf,int32_t value);
// Write 8bytes int32_t to buffer.
void yang_write_8bytes(YangBuffer* buf,int64_t value);
void yang_write_le8bytes(YangBuffer* buf,int64_t value);
// Write string to buffer
//void yang_write_string(YangBuffer* buf,std::string value);
// Write bytes to buffer
void yang_write_bytes(YangBuffer* buf,char* data, int32_t size);
void yang_write_cstring(YangBuffer* buf,char* data);

// Read 1bytes char from buffer.
    char yang_read_1bytes(YangBuffer* buf);
    // Read 2bytes int32_t from buffer.
    int16_t yang_read_2bytes(YangBuffer* buf);
    int16_t yang_read_le2bytes(YangBuffer* buf);
    // Read 3bytes int32_t from buffer.
    int32_t yang_read_3bytes(YangBuffer* buf);
    int32_t yang_read_le3bytes(YangBuffer* buf);
    // Read 4bytes int32_t from buffer.
    int32_t yang_read_4bytes(YangBuffer* buf);
    int32_t yang_read_le4bytes(YangBuffer* buf);
    // Read 8bytes int32_t from buffer.
    int64_t yang_read_8bytes(YangBuffer* buf);
    int64_t yang_read_le8bytes(YangBuffer* buf);
    // Read bytes from buffer, length specifies by param len.
    void yang_read_bytes(YangBuffer* buf,char* data, int32_t size);


    uint16_t yang_readchar_2bytes(char* buf);
    uint32_t yang_readchar_4bytes(char* buf);


#endif /* INCLUDE_YANGUTIL_BUFFER_YANGCBUFFER_H_ */
