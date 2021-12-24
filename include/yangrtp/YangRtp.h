
#ifndef YANGWEBRTC_YANGRTCRTP_H_
#define YANGWEBRTC_YANGRTCRTP_H_
#include <yangutil/buffer/YangBuffer.h>
#include <yangutil/yangavtype.h>
#include <string>
#include <string.h>
#include <vector>
using namespace std;

class YangRtpPacket;
// The payload type, for performance to avoid dynamic cast.
enum YangRtspPacketPayloadType
{
   YangRtspPacketPayloadTypeRaw,
    YangRtspPacketPayloadTypeFUA2,
	YangRtspPacketPayloadTypeFUA,
	YangRtspPacketPayloadTypeNALU,
	YangRtspPacketPayloadTypeSTAP,
	YangRtspPacketPayloadTypeUnknown,
};

enum YangRtpExtensionType
{
    kRtpExtensionNone,
    kRtpExtensionTransportSequenceNumber,
    kRtpExtensionAudioLevel,
    kRtpExtensionNumberOfExtensions  // Must be the last entity in the enum.
};

const std::string kAudioLevelUri = "urn:ietf:params:rtp-hdrext:ssrc-audio-level";

struct YangExtensionInfo
{
    YangRtpExtensionType type;
    std::string uri;
};

// Fast parse the SSRC from RTP packet. Return 0 if invalid.
uint32_t yang_rtp_fast_parse_ssrc(char* buf, int32_t size);
uint8_t yang_rtp_fast_parse_pt(char* buf, int32_t size);
int32_t yang_rtp_fast_parse_twcc(char* buf, int32_t size, uint8_t twcc_id, uint16_t& twcc_sn);



// The "distance" between two uint16 number, for example:
//      distance(prev_value=3, value=5) === (int16_t)(uint16_t)((uint16_t)3-(uint16_t)5) === -2
//      distance(prev_value=3, value=65534) === (int16_t)(uint16_t)((uint16_t)3-(uint16_t)65534) === 5
//      distance(prev_value=65532, value=65534) === (int16_t)(uint16_t)((uint16_t)65532-(uint16_t)65534) === -2
// For RTP sequence, it's only uint16 and may flip back, so 3 maybe 3+0xffff.
// @remark Note that srs_rtp_seq_distance(0, 32768)>0 is TRUE by https://mp.weixin.qq.com/s/JZTInmlB9FUWXBQw_7NYqg
//      but for WebRTC jitter buffer it's FALSE and we follow it.
// @remark For srs_rtp_seq_distance(32768, 0)>0, it's FALSE definitely.
inline int16_t yang_rtp_seq_distance(const uint16_t& prev_value, const uint16_t& value)
{
    return (int16_t)(value - prev_value);
}

// For map to compare the sequence of RTP.
struct YangSeqCompareLess {
    bool operator()(const uint16_t& pre_value, const uint16_t& value) const {
        return yang_rtp_seq_distance(pre_value, value) > 0;
    }
};

bool yang_seq_is_newer(uint16_t value, uint16_t pre_value);
bool yang_seq_is_rollback(uint16_t value, uint16_t pre_value);
int32_t yang_seq_distance(uint16_t value, uint16_t pre_value);


// The common payload interface for RTP packet.
class IYangRtpPayloader //: public IYangCodec
{
public:
    IYangRtpPayloader();
    virtual ~IYangRtpPayloader();
public:
    virtual IYangRtpPayloader* copy() = 0;
    virtual int32_t decode(YangBuffer* buf) = 0;
    virtual int32_t encode(YangBuffer* buf) = 0;
    virtual uint64_t nb_bytes() = 0;
    virtual int32_t nb_size()=0;
};
class YangRtpPayloader //: public IYangCodec
{
public:
    YangRtpPayloader();
    virtual ~YangRtpPayloader();

    char* m_payload;
    int32_t m_size;
public:
    virtual YangRtpPayloader* copy() = 0;
    virtual int32_t decode(YangBuffer* buf) = 0;
    virtual int32_t encode(YangBuffer* buf) = 0;
    virtual uint64_t nb_bytes() = 0;
    virtual bool getStart()=0;
};
/**
class IYangRtspPacketDecodeHandler
{
public:
    IYangRtspPacketDecodeHandler();
    virtual ~IYangRtspPacketDecodeHandler();
public:
    // We don't know the actual payload, so we depends on external handler.
    virtual void on_before_decode_payload(YangRtpPacket* pkt, YangBuffer* buf, YangRtpPayloader** ppayload, YangRtspPacketPayloadType* ppt) = 0;
};
**/


class YangRtpExtensionTypes
{
public:
    static const YangRtpExtensionType kInvalidType = kRtpExtensionNone;
    static const int32_t kInvalidId = 0;
public:
    bool register_by_uri(int32_t id, std::string uri);
    YangRtpExtensionType get_type(int32_t id) const;
public:
    YangRtpExtensionTypes();
    virtual ~YangRtpExtensionTypes();
private:
    bool register_id(int32_t id, YangRtpExtensionType type, std::string uri);
private:
    uint8_t m_ids[kRtpExtensionNumberOfExtensions];
};

// Note that the extensions should never extends from any class, for performance.
class YangRtpExtensionTwcc// : public ISrsCodec
{
    bool m_has_twcc;
    uint8_t m_id;
    uint16_t m_sn;
public:
    YangRtpExtensionTwcc();
    virtual ~YangRtpExtensionTwcc();
public:
    inline bool exists() { return m_has_twcc; } // SrsRtpExtensionTwcc::exists
    uint8_t get_id();
    void set_id(uint8_t id);
    uint16_t get_sn();
    void set_sn(uint16_t sn);
public:
    // ISrsCodec
    virtual int32_t decode(YangBuffer* buf);
    virtual int32_t encode(YangBuffer* buf);
    virtual uint64_t nb_bytes();
};

// Note that the extensions should never extends from any class, for performance.
class YangRtpExtensionOneByte// : public ISrsCodec
{
    bool m_has_ext;
    int32_t m_id;
    uint8_t m_value;
public:
    YangRtpExtensionOneByte();
    virtual ~YangRtpExtensionOneByte();
public:
    inline bool exists() { return m_has_ext; } // SrsRtpExtensionOneByte::exists
    int32_t get_id() { return m_id; }
    uint8_t get_value() { return m_value; }
    void set_id(int32_t id);
    void set_value(uint8_t value);
public:

    virtual int32_t decode(YangBuffer* buf);
    virtual int32_t encode(YangBuffer* buf);
    virtual uint64_t nb_bytes() { return 2; };
};

struct YangRtpExtension{
	   bool m_has_ext;
	    // by default, twcc isnot decoded. Because it is decoded by fast function(srs_rtp_fast_parse_twcc)
	    bool m_decode_twcc_extension;
};
// Note that the extensions should never extends from any class, for performance.
class YangRtpExtensions// : public ISrsCodec
{
private:
    bool m_has_ext;
    // by default, twcc isnot decoded. Because it is decoded by fast function(srs_rtp_fast_parse_twcc)
    bool m_decode_twcc_extension;
private:
    // The extension types is used to decode the packet, which is reference to
    // the types in publish stream.
    YangRtpExtensionTypes* m_types;
private:
    YangRtpExtensionTwcc m_twcc;
    YangRtpExtensionOneByte m_audio_level;
public:
    YangRtpExtensions();
    virtual ~YangRtpExtensions();
public:
    void enable_twcc_decode() { m_decode_twcc_extension = true; } // SrsRtpExtensions::enable_twcc_decode
    inline bool exists() { return m_has_ext; } // SrsRtpExtensions::exists
    void set_types_(YangRtpExtensionTypes* types);
    int32_t get_twcc_sequence_number(uint16_t& twcc_sn);
    int32_t set_twcc_sequence_number(uint8_t id, uint16_t sn);
    int32_t get_audio_level(uint8_t& level);
    int32_t set_audio_level(int32_t id, uint8_t level);
// ISrsCodec
public:
    virtual int32_t decode(YangBuffer* buf);
private:
    int32_t decode_0xbede(YangBuffer* buf);
public:
    virtual int32_t encode(YangBuffer* buf);
    virtual uint64_t nb_bytes();
};




#endif /* YANGWEBRTC_YANGRTCRTP_H_ */
