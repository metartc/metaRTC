#ifndef SRC_YANGRTP_YANGRTPHEADER_H_
#define SRC_YANGRTP_YANGRTPHEADER_H_
#include <yangrtp/YangRtp.h>
struct YangRtpHeader{
    uint8_t padding_length;
    uint8_t cc;
    bool marker;
    uint8_t payload_type;
    uint16_t sequence;
    uint32_t timestamp;
    uint32_t ssrc;
    uint32_t csrc[15];
    YangRtpExtensions* extensions;
    bool ignore_padding;
};

int32_t yang_encode_rtpHeader(YangBuffer* buf,YangRtpHeader* header);
int32_t yang_decode_rtpHeader(YangBuffer* buf,YangRtpHeader* header);
/**
// Note that the header sho;uld never extends from any class, for performance.
class YangRtpHeader //: public IYangCodec
{
private:
    uint8_t m_padding_length;
    uint8_t m_cc;
    bool m_marker;
    uint8_t m_payload_type;
    uint16_t m_sequence;
    uint32_t m_timestamp;
    uint32_t m_ssrc;
    uint32_t m_csrc[15];
    YangRtpExtensions m_extensions;
    bool m_ignore_padding;
public:
    YangRtpHeader();
    virtual ~YangRtpHeader();
public:
    void reset();
    int32_t decode(YangBuffer* buf);
private:
    int32_t parse_extensions(YangBuffer* buf);
public:
     int32_t encode(YangBuffer* buf);
     uint64_t nb_bytes();
public:
    void enable_twcc_decode() { m_extensions.enable_twcc_decode(); } // SrsRtpHeader::enable_twcc_decode
    void set_marker(bool v);
    bool get_marker() const;
    void set_payload_type(uint8_t v);
    uint8_t get_payload_type() const;
    void set_sequence(uint16_t v);
    uint16_t get_sequence() const;
    void set_timestamp(uint32_t v);
    uint32_t get_timestamp() const;
    void set_ssrc(uint32_t v);
    inline uint32_t get_ssrc() const { return m_ssrc; } // SrsRtpHeader::get_ssrc
    void set_padding(uint8_t v);
    uint8_t get_padding() const;
    void set_extensions(YangRtpExtensionTypes* extmap);
    void ignore_padding(bool v);
    int32_t get_twcc_sequence_number(uint16_t& twcc_sn);
    int32_t set_twcc_sequence_number(uint8_t id, uint16_t sn);
};**/
#endif /* SRC_YANGRTP_YANGRTPHEADER_H_ */
