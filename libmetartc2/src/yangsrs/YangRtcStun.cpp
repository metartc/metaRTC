#include <yangrtp/YangRtpConstant.h>

#include <yangutil/sys/YangSsl.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangString.h>
#include <yangutil/yangavinfotype.h>
#include <yangutil/buffer/YangBuffer.h>
#include <stdint.h>
#include <openssl/dh.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/ssl.h>
#include <yangsrs/YangRtcStun.h>


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

    YangBuffer stream ;
    yang_init_buffer(&stream,const_cast<char*>(buf), nb_buf);


    if (yang_buffer_left(&stream) < 20) {
        return yang_error_wrap(ERROR_RTC_STUN, "ERROR_RTC_STUN invalid stun packet, size=%d", stream.size);
    }

    m_message_type = yang_read_2bytes(&stream);
    uint16_t message_len = yang_read_2bytes(&stream);
    string magic_cookie = yang_read_string(&stream,4);
    m_transcation_id = yang_read_string(&stream,12);

    if (nb_buf != 20 + message_len) {
        return yang_error_wrap(ERROR_RTC_STUN, "ERROR_RTC_STUN invalid stun packet, message_len=%d, nb_buf=%d", message_len, nb_buf);
    }

    while (yang_buffer_left(&stream) >= 4) {
        uint16_t type = yang_read_2bytes(&stream);
        uint16_t len = yang_read_2bytes(&stream);

        if (yang_buffer_left(&stream)  < len) {
            return yang_error_wrap(ERROR_RTC_STUN, "invalid stun packet");
        }

        string val = yang_read_string(&stream,len);
        // padding
        if (len % 4 != 0) {
            yang_read_string(&stream,4 - (len % 4));
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

	    yang_write_2bytes(stream,BindingResponse);
	    yang_write_2bytes(stream,property_username.size() + mapped_address.size());
	    yang_write_4bytes(stream,kStunMagicCookie);
	    yang_write_string(stream,m_transcation_id);
	    yang_write_string(stream,property_username);
	    yang_write_string(stream,mapped_address);

	    stream->data[2] = ((yang_buffer_pos(stream) - 20 + 20 + 4) & 0x0000FF00) >> 8;
	    stream->data[3] = ((yang_buffer_pos(stream) - 20 + 20 + 4) & 0x000000FF);

	    char hmac_buf[20] = {0};
	    uint32_t  hmac_buf_len = 0;
	    if ((err = hmac_encode("sha1", pwd.c_str(), pwd.size(), stream->data, yang_buffer_pos(stream), hmac_buf, &hmac_buf_len)) != Yang_Ok) {
	        return yang_error_wrap(err, "hmac encode failed");
	    }

	    string hmac = encode_hmac(hmac_buf, hmac_buf_len);

	    yang_write_string(stream,hmac);
	    stream->data[2] = ((yang_buffer_pos(stream) - 20 + 8) & 0x0000FF00) >> 8;
	    stream->data[3] = ((yang_buffer_pos(stream) - 20 + 8) & 0x000000FF);

	    uint32_t crc32 = yang_crc32_ieee(stream->data, yang_buffer_pos(stream), 0) ^ 0x5354554E;

	    string fingerprint = encode_fingerprint(crc32);

	    yang_write_string(stream,fingerprint);

	    stream->data[2] = ((yang_buffer_pos(stream) - 20) & 0x0000FF00) >> 8;
	    stream->data[3] = ((yang_buffer_pos(stream) - 20) & 0x000000FF);

	    return err;
}
int32_t YangStunPacket::encode_binding_request(const string& pwd, YangBuffer* stream)
{
    int32_t err = Yang_Ok;

    string property_username = encode_username();
    string mapped_address = encode_mapped_address();

    yang_write_2bytes(stream,BindingRequest);
    yang_write_2bytes(stream,property_username.size() + mapped_address.size());
    yang_write_4bytes(stream,kStunMagicCookie);
    yang_write_string(stream,m_transcation_id);
    yang_write_string(stream,property_username);
    yang_write_string(stream,mapped_address);

    stream->data[2] = ((yang_buffer_pos(stream) - 20 + 20 + 4) & 0x0000FF00) >> 8;
    stream->data[3] = ((yang_buffer_pos(stream) - 20 + 20 + 4) & 0x000000FF);

    char hmac_buf[20] = {0};
    uint32_t  hmac_buf_len = 0;
    if ((err = hmac_encode("sha1", pwd.c_str(), pwd.size(), stream->data, yang_buffer_pos(stream), hmac_buf, &hmac_buf_len)) != Yang_Ok) {
        return yang_error_wrap(err, "hmac encode failed");
    }

    string hmac = encode_hmac(hmac_buf, hmac_buf_len);

    yang_write_string(stream,hmac);
    stream->data[2] = ((yang_buffer_pos(stream) - 20 + 8) & 0x0000FF00) >> 8;
    stream->data[3] = ((yang_buffer_pos(stream) - 20 + 8) & 0x000000FF);

    uint32_t crc32 = yang_crc32_ieee(stream->data, yang_buffer_pos(stream), 0) ^ 0x5354554E;

    string fingerprint = encode_fingerprint(crc32);

    yang_write_string(stream,fingerprint);

    stream->data[2] = ((yang_buffer_pos(stream) - 20) & 0x0000FF00) >> 8;
    stream->data[3] = ((yang_buffer_pos(stream) - 20) & 0x000000FF);

    return err;
}
string YangStunPacket::encode_username()
{
    char buf[1460];

    YangBuffer stream ;
    yang_init_buffer(&stream,buf, sizeof(buf));
    string username = m_remote_ufrag + ":" + m_local_ufrag;

    yang_write_2bytes(&stream,Username);
    yang_write_2bytes(&stream,username.size());
    yang_write_string(&stream,username);

    if (yang_buffer_pos(&stream) % 4 != 0) {
        static char padding[4] = {0};
        yang_write_bytes(&stream,padding, 4 - (yang_buffer_pos(&stream) % 4));
    }

    return string(stream.data, yang_buffer_pos(&stream));
}

string YangStunPacket::encode_mapped_address()
{
    char buf[1460];

    YangBuffer stream ;
       yang_init_buffer(&stream,buf, sizeof(buf));
    yang_write_2bytes(&stream,XorMappedAddress);
    yang_write_2bytes(&stream,8);
    yang_write_1bytes(&stream,0); // ignore this bytes
    yang_write_1bytes(&stream,1); // ipv4 family
    yang_write_2bytes(&stream,m_mapped_port ^ (kStunMagicCookie >> 16));
    yang_write_4bytes(&stream,m_mapped_address ^ kStunMagicCookie);

    return string(stream.data, yang_buffer_pos(&stream));
}

string encode_hmac(char* hmac_buf, const int32_t hmac_buf_len)
{
    char buf[1460];
    YangBuffer stream ;
           yang_init_buffer(&stream,buf, sizeof(buf));

    yang_write_2bytes(&stream,MessageIntegrity);
    yang_write_2bytes(&stream,hmac_buf_len);
    yang_write_bytes(&stream,hmac_buf, hmac_buf_len);

    return string(stream.data, yang_buffer_pos(&stream));
}

string encode_fingerprint(uint32_t crc32)
{
    char buf[1460];
    YangBuffer stream ;
     yang_init_buffer(&stream,buf, sizeof(buf));

    yang_write_2bytes(&stream,Fingerprint);
    yang_write_2bytes(&stream,4);
    yang_write_4bytes(&stream,crc32);

    return string(stream.data, yang_buffer_pos(&stream));
}

