#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#endif
#include <yangrtp/YangRtp.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangutil/sys/YangSsrc.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/buffer/YangBuffer.h>

const YangExtensionInfo kExtensions[] = {
    {kRtpExtensionTransportSequenceNumber, std::string("http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01")},
    {kRtpExtensionAudioLevel, kAudioLevelUri},
};
/* @see https://tools.ietf.org/html/rfc1889#section-5.1
  0                   1                   2                   3
  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |V=2|P|X|  CC   |M|     PT      |       sequence number         |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |                           timestamp                           |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |           synchronization source (SSRC) identifier            |
 +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
 |            contributing source (CSRC) identifiers             |
 |                             ....                              |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
uint32_t yang_rtp_fast_parse_ssrc(char* buf, int32_t size)
{
    if (size < 12) {
        return 0;
    }

    uint32_t value = 0;
    char* pp = (char*)&value;

    char* p = buf + 8;
    pp[3] = *p++;
    pp[2] = *p++;
    pp[1] = *p++;
    pp[0] = *p++;
    return value;
}
uint8_t yang_rtp_fast_parse_pt(char* buf, int32_t size)
{
    if (size < 12) {
        return 0;
    }
    return buf[1] & 0x7f;
}
int32_t yang_rtp_fast_parse_twcc(char* buf, int32_t size, uint8_t twcc_id, uint16_t& twcc_sn)
{
    int32_t err = Yang_Ok;

    int32_t need_size = 12 /*rtp head fix len*/ + 4 /* extension header len*/ + 3 /* twcc extension len*/;
    if(size < (need_size)) {
        return yang_error_wrap(ERROR_RTC_RTP_MUXER, "required %d bytes, actual %d", need_size, size);
    }

    uint8_t first = buf[0];
    bool extension = (first & 0x10);
    uint8_t cc = (first & 0x0F);

    if(!extension) {
        return yang_error_wrap(ERROR_RTC_RTP, "no extension in rtp");
    }

    need_size += cc * 4; // csrc size
    if(size < (need_size)) {
        return yang_error_wrap(ERROR_RTC_RTP_MUXER, "required %d bytes, actual %d", need_size, size);
    }
    buf += 12 + 4*cc;

    uint16_t value = *((uint16_t*)buf);
    value = ntohs(value);
    if(0xBEDE != value) {
        return yang_error_wrap(ERROR_RTC_RTP_MUXER, "no support this type(0x%02x) extension", value);
    }
    buf += 2;

    uint16_t extension_length = ntohs(*((uint16_t*)buf));
    buf += 2;
    extension_length *= 4;
    need_size += extension_length; // entension size
    if(size < (need_size)) {
        return yang_error_wrap(ERROR_RTC_RTP_MUXER, "required %d bytes, actual %d", need_size, size);
    }

    while(extension_length > 0) {
        uint8_t v = buf[0];
        buf++;
        extension_length--;
        if(0 == v) {
            continue;
        }

        uint8_t id = (v & 0xF0) >>4;
        uint8_t len = (v & 0x0F) + 1;

        if(id == twcc_id) {
            twcc_sn = ntohs(*((uint16_t*)buf));
            return err;
        } else {
            buf += len;
            extension_length -= len;
        }
    }


    return err;
}

// If value is newer than pre_value，return true; otherwise false
bool yang_seq_is_newer(uint16_t value, uint16_t pre_value)
{
    return yang_rtp_seq_distance(pre_value, value) > 0;
}

bool yang_seq_is_rollback(uint16_t value, uint16_t pre_value)
{
    if(yang_seq_is_newer(value, pre_value)) {
        return pre_value > value;
    }
    return false;
}

// If value is newer then pre_value, return positive, otherwise negative.
int32_t yang_seq_distance(uint16_t value, uint16_t pre_value)
{
    return yang_rtp_seq_distance(pre_value, value);
}

YangRtpExtensionTypes::YangRtpExtensionTypes()
{
    memset(m_ids, kRtpExtensionNone, sizeof(m_ids));
}

YangRtpExtensionTypes::~YangRtpExtensionTypes()
{
}

bool YangRtpExtensionTypes::register_by_uri(int32_t id, std::string uri)
{
    for (int32_t i = 0; i < (int)(sizeof(kExtensions)/sizeof(kExtensions[0])); ++i) {
        if (kExtensions[i].uri == uri) {
            return register_id(id, kExtensions[i].type, kExtensions[i].uri);
        }
    }
    return false;
}

bool YangRtpExtensionTypes::register_id(int32_t id, YangRtpExtensionType type, std::string uri)
{
    if (id < 1 || id > 255) {
        return false;
    }

    m_ids[type] = static_cast<uint8_t>(id);
    return true;
}

YangRtpExtensionType YangRtpExtensionTypes::get_type(int32_t id) const
{
    for (int32_t type = kRtpExtensionNone + 1; type < kRtpExtensionNumberOfExtensions; ++type) {
        if (m_ids[type] == id) {
            return static_cast<YangRtpExtensionType>(type);
        }
    }
    return kInvalidType;
}

YangRtpExtensionTwcc::YangRtpExtensionTwcc()
{
    m_has_twcc = false;
    m_id = 0;
    m_sn = 0;
}

YangRtpExtensionTwcc::~YangRtpExtensionTwcc()
{
}

int32_t YangRtpExtensionTwcc::decode(YangBuffer* buf)
{
    int32_t err = Yang_Ok;

    //   0                   1                   2
    //   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3
    //  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    //  |  ID   | L=1   |transport wide sequence number |
    //  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    if (!buf->require(1)) {
        return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 1);
    }
    uint8_t v = buf->read_1bytes();

    m_id = (v & 0xF0) >> 4;
    uint8_t len = (v & 0x0F);
    if(!m_id || len != 1) {
        return yang_error_wrap(ERROR_RTC_RTP, "invalid twcc id=%d, len=%d", m_id, len);
    }

    if (!buf->require(2)) {
        return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 2);
    }
    m_sn = buf->read_2bytes();

    m_has_twcc = true;
    return err;
}

uint64_t YangRtpExtensionTwcc::nb_bytes()
{
    return 3;
}

int32_t YangRtpExtensionTwcc::encode(YangBuffer* buf)
{
    int32_t err = Yang_Ok;

    if(!buf->require(3)) {
        return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 3);
    }

    uint8_t id_len = (m_id & 0x0F)<< 4| 0x01;
    buf->write_1bytes(id_len);
    buf->write_2bytes(m_sn);

    return err;
}


uint8_t YangRtpExtensionTwcc::get_id()
{
    return m_id;
}

void YangRtpExtensionTwcc::set_id(uint8_t id)
{
    m_id = id;
    m_has_twcc = true;
}

uint16_t YangRtpExtensionTwcc::get_sn()
{
    return m_sn;
}

void YangRtpExtensionTwcc::set_sn(uint16_t sn)
{
    m_sn = sn;
    m_has_twcc = true;
}

YangRtpExtensionOneByte::YangRtpExtensionOneByte()
{
    m_has_ext = false;
    m_id = 0;
    m_value = 0;
}

YangRtpExtensionOneByte::~YangRtpExtensionOneByte()
{
}

void YangRtpExtensionOneByte::set_id(int32_t id)
{
    m_id = id;
    m_has_ext = true;
}

void YangRtpExtensionOneByte::set_value(uint8_t value)
{
    m_value = value;
    m_has_ext = true;
}

int32_t YangRtpExtensionOneByte::decode(YangBuffer* buf)
{
    int32_t err = Yang_Ok;

    if (!buf->require(2)) {
        return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 2);
    }
    uint8_t v = buf->read_1bytes();

    m_id = (v & 0xF0) >> 4;
    uint8_t len = (v & 0x0F);
    if(!m_id || len != 0) {
        return yang_error_wrap(ERROR_RTC_RTP, "invalid rtp extension id=%d, len=%d", m_id, len);
    }

    m_value = buf->read_1bytes();

    m_has_ext = true;
    return err;
}

int32_t YangRtpExtensionOneByte::encode(YangBuffer* buf)
{
    int32_t err = Yang_Ok;

    if (!buf->require(2)) {
        return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 2);
    }

    uint8_t id_len = (m_id & 0x0F)<< 4 | 0x00;
    buf->write_1bytes(id_len);
    buf->write_1bytes(m_value);

    return err;
}

YangRtpExtensions::YangRtpExtensions()
{
    m_types = NULL;
    m_has_ext = false;
    m_decode_twcc_extension = false;
}

YangRtpExtensions::~YangRtpExtensions()
{
}

int32_t YangRtpExtensions::decode(YangBuffer* buf)
{
    int32_t err = Yang_Ok;

    /* @see https://tools.ietf.org/html/rfc3550#section-5.3.1
        0                   1                   2                   3
        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
        +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        |      defined by profile       |           length              |
        +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        |                        header extension                       |
        |                             ....                              |
    */
    if (!buf->require(4)) {
        return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires 4 bytes");
    }
    uint16_t profile_id = buf->read_2bytes();
    uint16_t extension_length = buf->read_2bytes();

    // @see: https://tools.ietf.org/html/rfc5285#section-4.2
    if (profile_id == 0xBEDE) {
        YangBuffer xbuf(buf->head(), extension_length * 4);
        buf->skip(extension_length * 4);
        return decode_0xbede(&xbuf);
    }  else if (profile_id == 0x1000) {
        buf->skip(extension_length * 4);
    } else {
        return yang_error_wrap(ERROR_RTC_RTP_MUXER, "fail to parse extension");
    }
    return err;
}

int32_t YangRtpExtensions::decode_0xbede(YangBuffer* buf)
{
    int32_t err = Yang_Ok;

    while (!buf->empty()) {
        // The first byte maybe padding or id+len.
        if (!buf->require(1)) {
            return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 1);
        }
        uint8_t v = *((uint8_t*)buf->head());

        // Padding, ignore
        if(v == 0) {
            buf->skip(1);
            continue;
        }

        //  0
        //  0 1 2 3 4 5 6 7
        // +-+-+-+-+-+-+-+-+
        // |  ID   |  len  |
        // +-+-+-+-+-+-+-+-+
        // Note that 'len' is the header extension element length, which is the
        // number of bytes - 1.
        uint8_t id = (v & 0xF0) >> 4;
        uint8_t len = (v & 0x0F) + 1;

        YangRtpExtensionType xtype = m_types? m_types->get_type(id) : kRtpExtensionNone;
        if (xtype == kRtpExtensionTransportSequenceNumber) {
            if (m_decode_twcc_extension) {
                if ((err = m_twcc.decode(buf)) != Yang_Ok) {
                    return yang_error_wrap(err, "decode twcc extension");
                }
                m_has_ext = true;
            } else {
                if (!buf->require(len+1)) {
                    return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", len+1);
                }
                buf->skip(len + 1);
            }
        } else if (xtype == kRtpExtensionAudioLevel) {
            if((err = m_audio_level.decode(buf)) != Yang_Ok) {
                return yang_error_wrap(err, "decode audio level extension");
            }
            m_has_ext = true;
        } else {
            buf->skip(1 + len);
        }
    }

    return err;
}

uint64_t YangRtpExtensions::nb_bytes()
{
    int32_t size =  4 + (m_twcc.exists() ? m_twcc.nb_bytes() : 0);
    size += (m_audio_level.exists() ? m_audio_level.nb_bytes() : 0);
    // add padding
    size += (size % 4 == 0) ? 0 : (4 - size % 4);
    return size;
}

int32_t YangRtpExtensions::encode(YangBuffer* buf)
{
    int32_t err = Yang_Ok;

    buf->write_2bytes(0xBEDE);

    // Write length.
    int32_t len = 0;

    if (m_twcc.exists()) {
        len += m_twcc.nb_bytes();
    }

    if (m_audio_level.exists()) {
        len += m_audio_level.nb_bytes();
    }

    int32_t padding_count = (len % 4 == 0) ? 0 : (4 - len % 4);
    len += padding_count;

    if (!buf->require(len)) {
        return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", len);
    }

    buf->write_2bytes(len / 4);

    // Write extensions.
    if (m_twcc.exists()) {
        if ((err = m_twcc.encode(buf)) != Yang_Ok) {
            return yang_error_wrap(err, "encode twcc extension");
        }
    }

    if (m_audio_level.exists()) {
        if (Yang_Ok != (err = m_audio_level.encode(buf))) {
            return yang_error_wrap(err, "encode audio level extension");
        }
    }

    // add padding
    if (padding_count) {
        memset(buf->head(), 0, padding_count);
        buf->skip(padding_count);
    }

    return err;
}

void YangRtpExtensions::set_types_(YangRtpExtensionTypes* types)
{
    m_types = types;
}

int32_t YangRtpExtensions::get_twcc_sequence_number(uint16_t& twcc_sn)
{
    if (m_twcc.exists()) {
        twcc_sn = m_twcc.get_sn();
        return Yang_Ok;
    }
    return yang_error_wrap(ERROR_RTC_RTP_MUXER, "not find twcc sequence number");
}

int32_t YangRtpExtensions::set_twcc_sequence_number(uint8_t id, uint16_t sn)
{
    m_has_ext = true;
    m_twcc.set_id(id);
    m_twcc.set_sn(sn);
    return Yang_Ok;
}

int32_t YangRtpExtensions::get_audio_level(uint8_t& level)
{
    if(m_audio_level.exists()) {
        level = m_audio_level.get_value();
        return Yang_Ok;
    }
    return yang_error_wrap(ERROR_RTC_RTP_MUXER, "not find rtp extension audio level");
}

int32_t YangRtpExtensions::set_audio_level(int32_t id, uint8_t level)
{
    m_has_ext = true;
    m_audio_level.set_id(id);
    m_audio_level.set_value(level);
    return Yang_Ok;
}
YangRtpPayloader::YangRtpPayloader(){
	m_payload = NULL;
	m_size = 0;
}
 YangRtpPayloader::~YangRtpPayloader(){

}
IYangRtpPayloader::IYangRtpPayloader() {
}

IYangRtpPayloader::~IYangRtpPayloader() {
}



