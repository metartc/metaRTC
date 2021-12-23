#include <yangutil/buffer/YangBuffer.h>


YangBuffer::YangBuffer(char* b, int32_t nn)
{
    p = bytes = b;
    nb_bytes = nn;
}
YangBuffer::YangBuffer()
{
    p = bytes = NULL;
    nb_bytes = 0;
}
YangBuffer::~YangBuffer()
{
}
void YangBuffer::init(char* b, int32_t nn)
{
    p = bytes = b;
    nb_bytes = nn;
}
YangBuffer* YangBuffer::copy()
{
    YangBuffer* cp = new YangBuffer(bytes, nb_bytes);
    cp->p = p;
    return cp;
}

char* YangBuffer::data()
{
    return bytes;
}

char* YangBuffer::head()
{
    return p;
}

int32_t YangBuffer::size()
{
    return nb_bytes;
}

void YangBuffer::set_size(int32_t v)
{
    nb_bytes = v;
}

int32_t YangBuffer::pos()
{
    return (int)(p - bytes);
}

int32_t YangBuffer::left()
{
    return nb_bytes - (int)(p - bytes);
}

bool YangBuffer::empty()
{
    return !bytes || (p >= bytes + nb_bytes);
}

bool YangBuffer::require(int32_t required_size)
{
    if (required_size < 0) {
        return false;
    }

    return required_size <= nb_bytes - (p - bytes);
}

void YangBuffer::skip(int32_t size)
{

    p += size;
}

char YangBuffer::read_1bytes()
{
   // //srs_assert(require(1));
	//require(1);
    return (char)*p++;
}

int16_t YangBuffer::read_2bytes()
{
   // //srs_assert(require(2));

    int16_t value;
    char* pp = (char*)&value;
    pp[1] = *p++;
    pp[0] = *p++;

    return value;
}

int16_t YangBuffer::read_le2bytes()
{
    ////srs_assert(require(2));

    int16_t value;
    char* pp = (char*)&value;
    pp[0] = *p++;
    pp[1] = *p++;

    return value;
}

int32_t YangBuffer::read_3bytes()
{
   // //srs_assert(require(3));

    int32_t value = 0x00;
    char* pp = (char*)&value;
    pp[2] = *p++;
    pp[1] = *p++;
    pp[0] = *p++;

    return value;
}

int32_t YangBuffer::read_le3bytes()
{
    ////srs_assert(require(3));

    int32_t value = 0x00;
    char* pp = (char*)&value;
    pp[0] = *p++;
    pp[1] = *p++;
    pp[2] = *p++;

    return value;
}

int32_t YangBuffer::read_4bytes()
{
    ////srs_assert(require(4));

    int32_t value;
    char* pp = (char*)&value;
    pp[3] = *p++;
    pp[2] = *p++;
    pp[1] = *p++;
    pp[0] = *p++;

    return value;
}

int32_t YangBuffer::read_le4bytes()
{
    ////srs_assert(require(4));

    int32_t value;
    char* pp = (char*)&value;
    pp[0] = *p++;
    pp[1] = *p++;
    pp[2] = *p++;
    pp[3] = *p++;

    return value;
}

int64_t YangBuffer::read_8bytes()
{
   // //srs_assert(require(8));

    int64_t value;
    char* pp = (char*)&value;
    pp[7] = *p++;
    pp[6] = *p++;
    pp[5] = *p++;
    pp[4] = *p++;
    pp[3] = *p++;
    pp[2] = *p++;
    pp[1] = *p++;
    pp[0] = *p++;

    return value;
}

int64_t YangBuffer::read_le8bytes()
{
    ////srs_assert(require(8));

    int64_t value;
    char* pp = (char*)&value;
    pp[0] = *p++;
    pp[1] = *p++;
    pp[2] = *p++;
    pp[3] = *p++;
    pp[4] = *p++;
    pp[5] = *p++;
    pp[6] = *p++;
    pp[7] = *p++;

    return value;
}

string YangBuffer::read_string(int32_t len)
{
    ////srs_assert(require(len));

    std::string value;
    value.append(p, len);

    p += len;

    return value;
}

void YangBuffer::read_bytes(char* data, int32_t size)
{
    //srs_assert(require(size));

    memcpy(data, p, size);

    p += size;
}

void YangBuffer::write_1bytes(char value)
{
    //srs_assert(require(1));

    *p++ = value;
}

void YangBuffer::write_2bytes(int16_t value)
{
    //srs_assert(require(2));

    char* pp = (char*)&value;
    *p++ = pp[1];
    *p++ = pp[0];
}

void YangBuffer::write_le2bytes(int16_t value)
{
    //srs_assert(require(2));

    char* pp = (char*)&value;
    *p++ = pp[0];
    *p++ = pp[1];
}

void YangBuffer::write_4bytes(int32_t value)
{
    //srs_assert(require(4));

    char* pp = (char*)&value;
    *p++ = pp[3];
    *p++ = pp[2];
    *p++ = pp[1];
    *p++ = pp[0];
}

void YangBuffer::write_le4bytes(int32_t value)
{
    //srs_assert(require(4));

    char* pp = (char*)&value;
    *p++ = pp[0];
    *p++ = pp[1];
    *p++ = pp[2];
    *p++ = pp[3];
}

void YangBuffer::write_3bytes(int32_t value)
{
    //srs_assert(require(3));

    char* pp = (char*)&value;
    *p++ = pp[2];
    *p++ = pp[1];
    *p++ = pp[0];
}

void YangBuffer::write_le3bytes(int32_t value)
{
    //srs_assert(require(3));

    char* pp = (char*)&value;
    *p++ = pp[0];
    *p++ = pp[1];
    *p++ = pp[2];
}

void YangBuffer::write_8bytes(int64_t value)
{
    //srs_assert(require(8));

    char* pp = (char*)&value;
    *p++ = pp[7];
    *p++ = pp[6];
    *p++ = pp[5];
    *p++ = pp[4];
    *p++ = pp[3];
    *p++ = pp[2];
    *p++ = pp[1];
    *p++ = pp[0];
}

void YangBuffer::write_le8bytes(int64_t value)
{
    //srs_assert(require(8));

    char* pp = (char*)&value;
    *p++ = pp[0];
    *p++ = pp[1];
    *p++ = pp[2];
    *p++ = pp[3];
    *p++ = pp[4];
    *p++ = pp[5];
    *p++ = pp[6];
    *p++ = pp[7];
}

void YangBuffer::write_string(string value)
{
    //srs_assert(require((int)value.length()));

    memcpy(p, value.data(), value.length());
    p += value.length();
}

void YangBuffer::write_bytes(char* data, int32_t size)
{
    //srs_assert(require(size));

    memcpy(p, data, size);
    p += size;
}

YangBitBuffer::YangBitBuffer(YangBuffer* b)
{
    cb = 0;
    cb_left = 0;
    stream = b;
}

YangBitBuffer::~YangBitBuffer()
{
}

bool YangBitBuffer::empty() {
    if (cb_left) {
        return false;
    }
    return stream->empty();
}

char YangBitBuffer::read_bit() {
    if (!cb_left) {
        //srs_assert(!stream->empty());
        cb = stream->read_1bytes();
        cb_left = 8;
    }

    char v = (cb >> (cb_left - 1)) & 0x01;
    cb_left--;
    return v;
}
