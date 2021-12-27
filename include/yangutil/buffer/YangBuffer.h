
#ifndef YangBuffer_YANGRTCUTIL_H_
#define YangBuffer_YANGRTCUTIL_H_
#include <string>
#include <string.h>
#include <vector>
#include <stdint.h>
using namespace std;
class YangBuffer
{
private:
    // current position at bytes.
    char* p;
    // the bytes data for buffer to read or write.
    char* bytes;
    // the total number of bytes.
    int32_t nb_bytes;
public:
    // Create buffer with data b and size nn.
    // @remark User must free the data b.
     YangBuffer();
    YangBuffer(char* b, int32_t nn);
    ~YangBuffer();
public:
    // Copy the object, keep position of buffer.
    YangBuffer* copy();
    void init(char* b, int32_t nn);
    // Get the data and head of buffer.
    //      current-bytes = head() = data() + pos()
    char* data();
    char* head();
    // Get the total size of buffer.
    //      left-bytes = size() - pos()
    int32_t size();
    void set_size(int32_t v);
    // Get the current buffer position.
    int32_t pos();
    // Left bytes in buffer, total size() minus the current pos().
    int32_t left();
    // Whether buffer is empty.
    bool empty();
    // Whether buffer is able to supply required size of bytes.
    // @remark User should check buffer by require then do read/write.
    // @remark Assert the required_size is not negative.
    bool require(int32_t required_size);
public:
    // Skip some size.
    // @param size can be any value. positive to forward; negative to backward.
    // @remark to skip(pos()) to reset buffer.
    // @remark assert initialized, the data() not NULL.
    void skip(int32_t size);
public:
    // Read 1bytes char from buffer.
    char read_1bytes();
    // Read 2bytes int32_t from buffer.
    int16_t read_2bytes();
    int16_t read_le2bytes();
    // Read 3bytes int32_t from buffer.
    int32_t read_3bytes();
    int32_t read_le3bytes();
    // Read 4bytes int32_t from buffer.
    int32_t read_4bytes();
    int32_t read_le4bytes();
    // Read 8bytes int32_t from buffer.
    int64_t read_8bytes();
    int64_t read_le8bytes();
    // Read string from buffer, length specifies by param len.
    std::string read_string(int32_t len);
    // Read bytes from buffer, length specifies by param len.
    void read_bytes(char* data, int32_t size);
public:
    // Write 1bytes char to buffer.
    void write_1bytes(char value);
    // Write 2bytes int32_t to buffer.
    void write_2bytes(int16_t value);
    void write_le2bytes(int16_t value);
    // Write 4bytes int32_t to buffer.
    void write_4bytes(int32_t value);
    void write_le4bytes(int32_t value);
    // Write 3bytes int32_t to buffer.
    void write_3bytes(int32_t value);
    void write_le3bytes(int32_t value);
    // Write 8bytes int32_t to buffer.
    void write_8bytes(int64_t value);
    void write_le8bytes(int64_t value);
    // Write string to buffer
    void write_string(std::string value);
    // Write bytes to buffer
    void write_bytes(char* data, int32_t size);
};

/**
 * the bit buffer, base on SrsBuffer,
 * for exmaple, the h.264 avc buffer is bit buffer.
 */
class YangBitBuffer
{
private:
    char cb;
    uint8_t cb_left;
    YangBuffer* stream;
public:
    YangBitBuffer(YangBuffer* b);
    ~YangBitBuffer();
public:
    bool empty();
    char read_bit();
};

#endif
