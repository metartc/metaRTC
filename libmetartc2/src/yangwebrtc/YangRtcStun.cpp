#include <yangwebrtc/YangRtcStun.h>

#include <yangrtp/YangRtpConstant.h>

#include <yangutil/sys/YangSsl.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/yangavinfotype.h>

#include <stdint.h>
#include <openssl/dh.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/ssl.h>

using namespace std;
YangStunPacket::YangStunPacket()
{
    m_message_type = 0;
    m_local_ufrag = "";
    m_remote_ufrag = "";
    m_use_candidate = false;
    m_ice_controlled = false;
    m_ice_controlling = false;
    m_mapped_port = 0;
    m_mapped_address = 0;
}

YangStunPacket::~YangStunPacket()
{
}

bool YangStunPacket::is_binding_request() const
{
    return m_message_type == BindingRequest;
}

bool YangStunPacket::is_binding_response() const
{
    return m_message_type == BindingResponse;
}

uint16_t YangStunPacket::get_message_type() const
{
    return m_message_type;
}

std::string YangStunPacket::get_username() const
{
    return m_username;
}

std::string YangStunPacket::get_local_ufrag() const
{
    return m_local_ufrag;
}

std::string YangStunPacket::get_remote_ufrag() const
{
    return m_remote_ufrag;
}

std::string YangStunPacket::get_transcation_id() const
{
    return m_transcation_id;
}

uint32_t YangStunPacket::get_mapped_address() const
{
    return m_mapped_address;
}

uint16_t YangStunPacket::get_mapped_port() const
{
    return m_mapped_port;
}

bool YangStunPacket::get_ice_controlled() const
{
    return m_ice_controlled;
}

bool YangStunPacket::get_ice_controlling() const
{
    return m_ice_controlling;
}

bool YangStunPacket::get_use_candidate() const
{
    return m_use_candidate;
}

void YangStunPacket::set_message_type(const uint16_t& m)
{
    m_message_type = m;
}

void YangStunPacket::set_local_ufrag(const std::string& u)
{
    m_local_ufrag = u;
}

void YangStunPacket::set_remote_ufrag(const std::string& u)
{
    m_remote_ufrag = u;
}

void YangStunPacket::set_transcation_id(const std::string& t)
{
    m_transcation_id = t;
}

void YangStunPacket::set_mapped_address(const uint32_t& addr)
{
    m_mapped_address = addr;
}

void YangStunPacket::set_mapped_port(const uint32_t& port)
{
    m_mapped_port = port;
}

int32_t YangStunPacket::decode(const char* buf, const int32_t nb_buf)
{
    int32_t err = Yang_Ok;

    YangBuffer* stream = new YangBuffer(const_cast<char*>(buf), nb_buf);
    YangAutoFree(YangBuffer, stream);

    if (stream->left() < 20) {
        return yang_error_wrap(ERROR_RTC_STUN, "ERROR_RTC_STUN invalid stun packet, size=%d", stream->size());
    }

    m_message_type = stream->read_2bytes();
    uint16_t message_len = stream->read_2bytes();
    string magic_cookie = stream->read_string(4);
    m_transcation_id = stream->read_string(12);

    if (nb_buf != 20 + message_len) {
        return yang_error_wrap(ERROR_RTC_STUN, "ERROR_RTC_STUN invalid stun packet, message_len=%d, nb_buf=%d", message_len, nb_buf);
    }

    while (stream->left() >= 4) {
        uint16_t type = stream->read_2bytes();
        uint16_t len = stream->read_2bytes();

        if (stream->left() < len) {
            return yang_error_wrap(ERROR_RTC_STUN, "invalid stun packet");
        }

        string val = stream->read_string(len);
        // padding
        if (len % 4 != 0) {
            stream->read_string(4 - (len % 4));
        }

        switch (type) {
            case Username: {
                m_username = val;
                size_t p = val.find(":");
                if (p != string::npos) {
                    m_local_ufrag = val.substr(0, p);
                    m_remote_ufrag = val.substr(p + 1);
                  //  yang_trace("stun packet local_ufrag=%s, remote_ufrag=%s", local_ufrag.c_str(), remote_ufrag.c_str());
                }
                break;
            }

			case UseCandidate: {
                m_use_candidate = true;
                yang_trace("stun use-candidate");
                break;
            }

            // @see: https://tools.ietf.org/html/draft-ietf-ice-rfc5245bis-00#section-5.1.2
			// One agent full, one lite:  The full agent MUST take the controlling
            // role, and the lite agent MUST take the controlled role.  The full
            // agent will form check lists, run the ICE state machines, and
            // generate connectivity checks.
			case IceControlled: {
                m_ice_controlled = true;
                yang_trace("stun ice-controlled");
                break;
            }

			case IceControlling: {
                m_ice_controlling = true;
                yang_trace("stun ice-controlling");
                break;
            }
            
            default: {
              //  yang_trace("stun type=%u, no process", type);
                break;
            }
        }
    }

    return err;
}

int32_t YangStunPacket::encode(const string& pwd, YangBuffer* stream)
{
    if (is_binding_response()) {
        return encode_binding_response(pwd, stream);
    }

     yang_error("ERROR_RTC_STUN unknown stun type=%d", get_message_type());
     return 1;
}

// FIXME: make this function easy to read
int32_t YangStunPacket::encode_binding_response(const string& pwd, YangBuffer* stream)
{
    int32_t err = Yang_Ok;

    string property_username = encode_username();
    string mapped_address = encode_mapped_address();

    stream->write_2bytes(BindingResponse);
    stream->write_2bytes(property_username.size() + mapped_address.size());
    stream->write_4bytes(kStunMagicCookie);
    stream->write_string(m_transcation_id);
    stream->write_string(property_username);
    stream->write_string(mapped_address);

    stream->data()[2] = ((stream->pos() - 20 + 20 + 4) & 0x0000FF00) >> 8;
    stream->data()[3] = ((stream->pos() - 20 + 20 + 4) & 0x000000FF);

    char hmac_buf[20] = {0};
    uint32_t  hmac_buf_len = 0;
    if ((err = hmac_encode("sha1", pwd.c_str(), pwd.size(), stream->data(), stream->pos(), hmac_buf, hmac_buf_len)) != Yang_Ok) {
        return yang_error_wrap(err, "hmac encode failed");
    }

    string hmac = encode_hmac(hmac_buf, hmac_buf_len);

    stream->write_string(hmac);
    stream->data()[2] = ((stream->pos() - 20 + 8) & 0x0000FF00) >> 8;
    stream->data()[3] = ((stream->pos() - 20 + 8) & 0x000000FF);

    uint32_t crc32 = yang_crc32_ieee(stream->data(), stream->pos(), 0) ^ 0x5354554E;

    string fingerprint = encode_fingerprint(crc32);

    stream->write_string(fingerprint);

    stream->data()[2] = ((stream->pos() - 20) & 0x0000FF00) >> 8;
    stream->data()[3] = ((stream->pos() - 20) & 0x000000FF);

    return err;
}
int32_t YangStunPacket::encode_binding_request(const string& pwd, YangBuffer* stream)
{
    int32_t err = Yang_Ok;

    string property_username = encode_username();
    string mapped_address = encode_mapped_address();

    stream->write_2bytes(BindingRequest);
    stream->write_2bytes(property_username.size() + mapped_address.size());
    stream->write_4bytes(kStunMagicCookie);
    stream->write_string(m_transcation_id);
    stream->write_string(property_username);
    stream->write_string(mapped_address);

    stream->data()[2] = ((stream->pos() - 20 + 20 + 4) & 0x0000FF00) >> 8;
    stream->data()[3] = ((stream->pos() - 20 + 20 + 4) & 0x000000FF);

    char hmac_buf[20] = {0};
    uint32_t  hmac_buf_len = 0;
    if ((err = hmac_encode("sha1", pwd.c_str(), pwd.size(), stream->data(), stream->pos(), hmac_buf, hmac_buf_len)) != Yang_Ok) {
        return yang_error_wrap(err, "hmac encode failed");
    }

    string hmac = encode_hmac(hmac_buf, hmac_buf_len);

    stream->write_string(hmac);
    stream->data()[2] = ((stream->pos() - 20 + 8) & 0x0000FF00) >> 8;
    stream->data()[3] = ((stream->pos() - 20 + 8) & 0x000000FF);

    uint32_t crc32 = yang_crc32_ieee(stream->data(), stream->pos(), 0) ^ 0x5354554E;

    string fingerprint = encode_fingerprint(crc32);

    stream->write_string(fingerprint);

    stream->data()[2] = ((stream->pos() - 20) & 0x0000FF00) >> 8;
    stream->data()[3] = ((stream->pos() - 20) & 0x000000FF);

    return err;
}
string YangStunPacket::encode_username()
{
    char buf[1460];
    YangBuffer* stream = new YangBuffer(buf, sizeof(buf));
    YangAutoFree(YangBuffer, stream);

    string username = m_remote_ufrag + ":" + m_local_ufrag;

    stream->write_2bytes(Username);
    stream->write_2bytes(username.size());
    stream->write_string(username);

    if (stream->pos() % 4 != 0) {
        static char padding[4] = {0};
        stream->write_bytes(padding, 4 - (stream->pos() % 4));
    }

    return string(stream->data(), stream->pos());
}

string YangStunPacket::encode_mapped_address()
{
    char buf[1460];
    YangBuffer* stream = new YangBuffer(buf, sizeof(buf));
    YangAutoFree(YangBuffer, stream);

    stream->write_2bytes(XorMappedAddress);
    stream->write_2bytes(8);
    stream->write_1bytes(0); // ignore this bytes
    stream->write_1bytes(1); // ipv4 family
    stream->write_2bytes(m_mapped_port ^ (kStunMagicCookie >> 16));
    stream->write_4bytes(m_mapped_address ^ kStunMagicCookie);

    return string(stream->data(), stream->pos());
}

string encode_hmac(char* hmac_buf, const int32_t hmac_buf_len)
{
    char buf[1460];
    YangBuffer* stream = new YangBuffer(buf, sizeof(buf));
    YangAutoFree(YangBuffer, stream);

    stream->write_2bytes(MessageIntegrity);
    stream->write_2bytes(hmac_buf_len);
    stream->write_bytes(hmac_buf, hmac_buf_len);

    return string(stream->data(), stream->pos());
}

string encode_fingerprint(uint32_t crc32)
{
    char buf[1460];
    YangBuffer* stream = new YangBuffer(buf, sizeof(buf));
    YangAutoFree(YangBuffer, stream);

    stream->write_2bytes(Fingerprint);
    stream->write_2bytes(4);
    stream->write_4bytes(crc32);

    return string(stream->data(), stream->pos());
}

