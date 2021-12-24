#ifndef YangStunPacket_H__
#define YangStunPacket_H__


#include <string>
#include <stdint.h>
#include <string.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangutil/buffer/YangBuffer.h>
#include <yangutil/sys/YangSsrc.h>
/**

**/
// @see: https://tools.ietf.org/html/rfc5389
// The magic cookie field MUST contain the fixed value 0x2112A442 in network byte order
const uint32_t kStunMagicCookie = 0x2112A442;

enum YangStunMessageType
{
	// see @ https://tools.ietf.org/html/rfc3489#section-11.1	
    BindingRequest            = 0x0001,
    BindingResponse           = 0x0101,
    BindingErrorResponse      = 0x0111,
    SharedSecretRequest       = 0x0002,
    SharedSecretResponse      = 0x0102,
    SharedSecretErrorResponse = 0x0112,
};

enum YangStunMessageAttribute
{
    // see @ https://tools.ietf.org/html/rfc3489#section-11.2
	MappedAddress     = 0x0001,
   	ResponseAddress   = 0x0002,
   	ChangeRequest     = 0x0003,
   	SourceAddress     = 0x0004,
   	ChangedAddress    = 0x0005,
   	Username          = 0x0006,
   	Password          = 0x0007,
   	MessageIntegrity  = 0x0008,
   	ErrorCode         = 0x0009,
   	UnknownAttributes = 0x000A,
   	ReflectedFrom     = 0x000B,

    // see @ https://tools.ietf.org/html/rfc5389#section-18.2
    Realm             = 0x0014,
    Nonce             = 0x0015,
    XorMappedAddress  = 0x0020,
    Software          = 0x8022,
    AlternateServer   = 0x8023,
    Fingerprint      = 0x8028,

    Priority          = 0x0024,
    UseCandidate      = 0x0025,
    IceControlled     = 0x8029,
    IceControlling    = 0x802A,
};

class YangStunPacket 
{
public:
    YangStunPacket();
    virtual ~YangStunPacket();


public:
    bool is_binding_request() const;
    bool is_binding_response() const;
    uint16_t get_message_type() const;
    std::string get_username() const;
    std::string get_local_ufrag() const;
    std::string get_remote_ufrag() const;
    std::string get_transcation_id() const;
    uint32_t get_mapped_address() const;
    uint16_t get_mapped_port() const;
    bool get_ice_controlled() const;
    bool get_ice_controlling() const;
    bool get_use_candidate() const;
    void set_message_type(const uint16_t& m);
    void set_local_ufrag(const std::string& u);
    void set_remote_ufrag(const std::string& u);
    void set_transcation_id(const std::string& t);
    void set_mapped_address(const uint32_t& addr);
    void set_mapped_port(const uint32_t& port);
    int32_t decode(const char* buf, const int32_t nb_buf);
    int32_t encode(const std::string& pwd, YangBuffer* stream);
    int32_t encode_binding_request(const std::string& pwd, YangBuffer* stream);
private:
    int32_t encode_binding_response(const std::string& pwd, YangBuffer* stream);
    std::string encode_username();
    std::string encode_mapped_address();
private:
    uint16_t m_message_type;
    std::string m_username;
    std::string m_local_ufrag;
    std::string m_remote_ufrag;
    std::string m_transcation_id;
    uint32_t m_mapped_address;
    uint16_t m_mapped_port;
    bool m_use_candidate;
    bool m_ice_controlled;
    bool m_ice_controlling;

};

#endif
