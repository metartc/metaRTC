#include <yangwebrtc/YangMediaDesc.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangString.h>
#include <algorithm>
#include <sstream>
#define kCRLF  "\r\n"
#define FETCH(is,word) \
if (!(is >> word)) {\
    return Yang_Fetch_Failed;\
}\

#define FETCH_WITH_DELIM(is,word,delim) \
if (!getline(is,word,delim)) {\
    return Yang_Fetch_Failed_WithDelim;\
}\

using namespace std;
YangSessionInfo::YangSessionInfo()
{
}

YangSessionInfo::~YangSessionInfo()
{
}

int32_t YangSessionInfo::parse_attribute(const std::string& attribute, const std::string& value)
{
    int32_t err = Yang_Ok;

    if (attribute == "ice-ufrag") {
        m_ice_ufrag = value;
    } else if (attribute == "ice-pwd") {
        m_ice_pwd = value;
    } else if (attribute == "ice-options") {
        m_ice_options = value;
    } else if (attribute == "fingerprint") {
        std::istringstream is(value);
        FETCH(is, m_fingerprint_algo);
        FETCH(is, m_fingerprint);
    } else if (attribute == "setup") {
        // @see: https://tools.ietf.org/html/rfc4145#section-4
        m_setup = value;
    } else {
       // yang_trace("ignore attribute=%s, value=%s", attribute.c_str(), value.c_str());
    }

    return err;
}

int32_t YangSessionInfo::encode(std::stringstream& os)
{
    int32_t err = Yang_Ok;

    if (!m_ice_ufrag.empty()) {
        os << "a=ice-ufrag:" << m_ice_ufrag << kCRLF;
    }

    if (!m_ice_pwd.empty()) {
        os << "a=ice-pwd:" << m_ice_pwd << kCRLF;
    }

    // For ICE-lite, we never set the trickle.
    if (!m_ice_options.empty()) {
        os << "a=ice-options:" << m_ice_options << kCRLF;
    }

    if (!m_fingerprint_algo.empty() && ! m_fingerprint.empty()) {
        os << "a=fingerprint:" << m_fingerprint_algo << " " << m_fingerprint << kCRLF;
    }

    if (!m_setup.empty()) {
        os << "a=setup:" << m_setup << kCRLF;
    }

    return err;
}

bool YangSessionInfo::operator=(const YangSessionInfo& rhs)
{
    return m_ice_ufrag        == rhs.m_ice_ufrag &&
           m_ice_pwd          == rhs.m_ice_pwd &&
           m_ice_options      == rhs.m_ice_options &&
           m_fingerprint_algo == rhs.m_fingerprint_algo &&
           m_fingerprint      == rhs.m_fingerprint &&
           m_setup            == rhs.m_setup;
}


YangMediaDesc::YangMediaDesc(const std::string& type)
{
    m_type = type;

    m_port = 0;
    m_rtcp_mux = false;
    m_rtcp_rsize = false;

    m_sendrecv = false;
    m_recvonly = false;
    m_sendonly = false;
    m_inactive = false;
}

YangMediaDesc::~YangMediaDesc()
{
}

YangMediaPayloadType* YangMediaDesc::find_media_with_payload_type(int32_t payload_type)
{
    for (size_t i = 0; i < m_payload_types.size(); ++i) {
        if (m_payload_types[i].m_payload_type == payload_type) {
            return &m_payload_types[i];
        }
    }

    return NULL;
}
bool YangMediaDesc::find_encoding_name(const std::string& encoding_name) const{

    std::string lower_name(encoding_name), upper_name(encoding_name);
    transform(encoding_name.begin(), encoding_name.end(), lower_name.begin(), ::tolower);
    transform(encoding_name.begin(), encoding_name.end(), upper_name.begin(), ::toupper);

    for (size_t i = 0; i < m_payload_types.size(); ++i) {
        if (m_payload_types[i].m_encoding_name == std::string(lower_name.c_str()) ||
            m_payload_types[i].m_encoding_name == std::string(upper_name.c_str())) {
           return true;
        }
    }

	return false;
}
vector<YangMediaPayloadType> YangMediaDesc::find_media_with_encoding_name(const std::string& encoding_name) const
{
    std::vector<YangMediaPayloadType> payloads;

    std::string lower_name(encoding_name), upper_name(encoding_name);
    transform(encoding_name.begin(), encoding_name.end(), lower_name.begin(), ::tolower);
    transform(encoding_name.begin(), encoding_name.end(), upper_name.begin(), ::toupper);

    for (size_t i = 0; i < m_payload_types.size(); ++i) {
        if (m_payload_types[i].m_encoding_name == std::string(lower_name.c_str()) ||
            m_payload_types[i].m_encoding_name == std::string(upper_name.c_str())) {
            payloads.push_back(m_payload_types[i]);
        }
    }

    return payloads;
}

int32_t YangMediaDesc::update_msid(string id)
{
    int32_t err = Yang_Ok;

    for(vector<YangSSRCInfo>::iterator it = m_ssrc_infos.begin(); it != m_ssrc_infos.end(); ++it) {
        YangSSRCInfo& info = *it;

        info.m_msid = id;
        info.m_mslabel = id;
    }

    return err;
}

int32_t YangMediaDesc::parse_line(const std::string& line)
{
    int32_t err = Yang_Ok;
    std::string content = line.substr(2);

    switch (line[0]) {
        case 'a': {
            return parse_attribute(content);
        }
        case 'c': {
            // TODO: process c-line
            break;
        }
        default: {
            yang_trace("ignore media line=%s", line.c_str());
            break;
        }
    }

    return err;
}

int32_t YangMediaDesc::encode(std::stringstream& os)
{
    int32_t err = Yang_Ok;

    os << "m=" << m_type << " " << m_port << " " << m_protos;
    for (std::vector<YangMediaPayloadType>::iterator iter = m_payload_types.begin(); iter != m_payload_types.end(); ++iter) {
        os << " " << iter->m_payload_type;
    }

    os << kCRLF;

    // TODO:nettype and address type
    os << "c=IN IP4 0.0.0.0" << kCRLF;

    if ((err = m_session_info.encode(os)) != Yang_Ok) {
        return printf("encode session info failed");
    }

    os << "a=mid:" << m_mid << kCRLF;
    if (!m_msid.empty()) {
        os << "a=msid:" << m_msid;

        if (!m_msid_tracker.empty()) {
            os << " " << m_msid_tracker;
        }

        os << kCRLF;
    }

    for(map<int, string>::iterator it = m_extmaps.begin(); it != m_extmaps.end(); ++it) {
        os << "a=extmap:"<< it->first<< " "<< it->second<< kCRLF;
    }
    if (m_sendonly) {
        os << "a=sendonly" << kCRLF;
    }
    if (m_recvonly) {
        os << "a=recvonly" << kCRLF;
    }
    if (m_sendrecv) {
        os << "a=sendrecv" << kCRLF;
    }
    if (m_inactive) {
        os << "a=inactive" << kCRLF;
    }

    if (m_rtcp_mux) {
        os << "a=rtcp-mux" << kCRLF;
    }

    if (m_rtcp_rsize) {
        os << "a=rtcp-rsize" << kCRLF;
    }

    for (std::vector<YangMediaPayloadType>::iterator iter = m_payload_types.begin(); iter != m_payload_types.end(); ++iter) {
        if ((err = iter->encode(os)) != Yang_Ok) {
            return printf("encode media payload failed");
        }
    }

    for (std::vector<YangSSRCInfo>::iterator iter = m_ssrc_infos.begin(); iter != m_ssrc_infos.end(); ++iter) {
        YangSSRCInfo& ssrc_info = *iter;

        if ((err = ssrc_info.encode(os)) != Yang_Ok) {
            return printf("encode ssrc failed");
        }
    }

    int32_t foundation = 0;
    int32_t component_id = 1; /* RTP */
    for (std::vector<YangCandidate>::iterator iter = m_candidates.begin(); iter != m_candidates.end(); ++iter) {
        // @see: https://tools.ietf.org/html/draft-ietf-ice-rfc5245bis-00#section-4.2
        uint32_t priority = (1<<24)*(126) + (1<<8)*(65535) + (1)*(256 - component_id);

        // @see: https://tools.ietf.org/id/draft-ietf-mmusic-ice-sip-sdp-14.html#rfc.section.5.1
        os << "a=candidate:" << foundation++ << " "
           << component_id << " udp " << priority << " "
           << iter->ip_ << " " << iter->port_
           << " typ " << iter->type_
           << " generation 0" << kCRLF;

        yang_trace("local SDP candidate line=%s", os.str().c_str());
    }

    return err;
}

int32_t YangMediaDesc::parse_attribute(const std::string& content)
{
    int32_t err = Yang_Ok;
    std::string attribute = "";
    std::string value = "";
    size_t pos = content.find_first_of(":");

    if (pos != std::string::npos) {
        attribute = content.substr(0, pos);
        value = content.substr(pos + 1);
    } else {
        attribute = content;
    }

    if (attribute == "extmap") {
        return parse_attr_extmap(value);
    } else if (attribute == "rtpmap") {
        return parse_attr_rtpmap(value);
    } else if (attribute == "rtcp") {
        return parse_attr_rtcp(value);
    } else if (attribute == "rtcp-fb") {
        return parse_attr_rtcp_fb(value);
    } else if (attribute == "fmtp") {
        return parse_attr_fmtp(value);
    } else if (attribute == "mid") {
        return parse_attr_mid(value);
    } else if (attribute == "msid") {
        return parse_attr_msid(value);
    } else if (attribute == "ssrc") {
        return parse_attr_ssrc(value);
    } else if (attribute == "ssrc-group") {
        return parse_attr_ssrc_group(value);
    } else if (attribute == "rtcp-mux") {
        m_rtcp_mux = true;
    } else if (attribute == "rtcp-rsize") {
        m_rtcp_rsize = true;
    } else if (attribute == "recvonly") {
        m_recvonly = true;
    } else if (attribute == "sendonly") {
        m_sendonly = true;
    } else if (attribute == "sendrecv") {
        m_sendrecv = true;
    } else if (attribute == "inactive") {
        m_inactive = true;
    } else {
        return m_session_info.parse_attribute(attribute, value);
    }

    return err;
}
int32_t YangMediaDesc::parse_attr_extmap(const std::string& value)
{
    int32_t err = Yang_Ok;
    std::istringstream is(value);
    int32_t id = 0;
    FETCH(is, id);
    if(m_extmaps.end() != m_extmaps.find(id)) {
        return yang_error_wrap(ERROR_RTC_SDP_DECODE, "duplicate ext id: %d", id);
    }
    string ext;
    FETCH(is, ext);
    m_extmaps[id] = ext;
    return err;
}

int32_t YangMediaDesc::parse_attr_rtpmap(const std::string& value)
{
    int32_t err = Yang_Ok;
    // @see: https://tools.ietf.org/html/rfc4566#page-25
    // a=rtpmap:<payload type> <encoding name>/<clock rate> [/<encoding parameters>]

    std::istringstream is(value);

    int32_t payload_type = 0;
    FETCH(is, payload_type);

    YangMediaPayloadType* payload = find_media_with_payload_type(payload_type);
    if (payload == NULL) {
        return yang_error_wrap(ERROR_RTC_SDP_DECODE, "can not find payload %d when pase rtpmap", payload_type);
    }

    std::string word;
    FETCH(is, word);

    std::vector<std::string> vec = yang_splits(word, "/");
    if (vec.size() < 2) {
        return yang_error_wrap(ERROR_RTC_SDP_DECODE, "invalid rtpmap line=%s", value.c_str());
    }

    payload->m_encoding_name = vec[0];
    payload->m_clock_rate = atoi(vec[1].c_str());

    if (vec.size() == 3) {
        payload->m_encoding_param = vec[2];
    }

    return err;
}

int32_t YangMediaDesc::parse_attr_rtcp(const std::string& value)
{
    int32_t err = Yang_Ok;

    // TODO:parse rtcp attribute

    return err;
}

int32_t YangMediaDesc::parse_attr_rtcp_fb(const std::string& value)
{
    int32_t err = Yang_Ok;
    // @see: https://tools.ietf.org/html/rfc5104#section-7.1

    std::istringstream is(value);

    int32_t payload_type = 0;
    FETCH(is, payload_type);

    YangMediaPayloadType* payload = find_media_with_payload_type(payload_type);
    if (payload == NULL) {
        return yang_error_wrap(ERROR_RTC_SDP_DECODE, "can not find payload %d when pase rtcp-fb", payload_type);
    }

    std::string rtcp_fb = is.str().substr(is.tellg());
    skip_first_spaces(rtcp_fb);

    payload->m_rtcp_fb.push_back(rtcp_fb);

    return err;
}

int32_t YangMediaDesc::parse_attr_fmtp(const std::string& value)
{
    int32_t err = Yang_Ok;
    // @see: https://tools.ietf.org/html/rfc4566#page-30
    // a=fmtp:<format> <format specific parameters>

    std::istringstream is(value);

    int32_t payload_type = 0;
    FETCH(is, payload_type);

    YangMediaPayloadType* payload = find_media_with_payload_type(payload_type);
    if (payload == NULL) {
        return yang_error_wrap(ERROR_RTC_SDP_DECODE, "can not find payload %d when pase fmtp", payload_type);
    }

    std::string word;
    FETCH(is, word);

    payload->m_format_specific_param = word;

    return err;
}

int32_t YangMediaDesc::parse_attr_mid(const std::string& value)
{
    // @see: https://tools.ietf.org/html/rfc3388#section-3
    int32_t err = Yang_Ok;
    std::istringstream is(value);
    // mid_ means m-line id
    FETCH(is, m_mid);
    yang_trace("mid=%s", m_mid.c_str());
    return err;
}

int32_t YangMediaDesc::parse_attr_msid(const std::string& value)
{
    // @see: https://tools.ietf.org/id/draft-ietf-mmusic-msid-08.html#rfc.section.2
    // TODO: msid and msid_tracker
    int32_t err = Yang_Ok;
    std::istringstream is(value);
    // msid_ means media stream id
    FETCH(is, m_msid);
    is >> m_msid_tracker;
    return err;
}

int32_t YangMediaDesc::parse_attr_ssrc(const std::string& value)
{
    int32_t err = Yang_Ok;
    // @see: https://tools.ietf.org/html/rfc5576#section-4.1

    std::istringstream is(value);

    uint32_t ssrc = 0;
    FETCH(is, ssrc);

    std::string ssrc_attr = "";
    FETCH_WITH_DELIM(is, ssrc_attr, ':');
    skip_first_spaces(ssrc_attr);

    std::string ssrc_value = is.str().substr(is.tellg());
    skip_first_spaces(ssrc_value);

    YangSSRCInfo& ssrc_info = fetch_or_create_ssrc_info(ssrc);

    if (ssrc_attr == "cname") {
        // @see: https://tools.ietf.org/html/rfc5576#section-6.1
        ssrc_info.m_cname = ssrc_value;
        ssrc_info.m_ssrc = ssrc;
    } else if (ssrc_attr == "msid") {
        // @see: https://tools.ietf.org/html/draft-alvestrand-mmusic-msid-00#section-2
        std::vector<std::string> vec = yang_splits(ssrc_value, " ");
        if (vec.empty()) {
            return yang_error_wrap(ERROR_RTC_SDP_DECODE, "invalid ssrc line=%s", value.c_str());
        }

        ssrc_info.m_msid = vec[0];
        if (vec.size() > 1) {
            ssrc_info.m_msid_tracker = vec[1];
        }
    } else if (ssrc_attr == "mslabel") {
        ssrc_info.m_mslabel = ssrc_value;
    } else if (ssrc_attr == "label") {
        ssrc_info.m_label = ssrc_value;
    }

    return err;
}

int32_t YangMediaDesc::parse_attr_ssrc_group(const std::string& value)
{
    int32_t err = Yang_Ok;
    // @see: https://tools.ietf.org/html/rfc5576#section-4.2
    // a=ssrc-group:<semantics> <ssrc-id> ...

    std::istringstream is(value);

    std::string semantics;
    FETCH(is, semantics);

    std::string ssrc_ids = is.str().substr(is.tellg());
    skip_first_spaces(ssrc_ids);

    std::vector<std::string> vec = yang_splits(ssrc_ids, " ");
    if (vec.size() == 0) {
        return yang_error_wrap(ERROR_RTC_SDP_DECODE, "invalid ssrc-group line=%s", value.c_str());
    }

    std::vector<uint32_t> ssrcs;
    for (size_t i = 0; i < vec.size(); ++i) {
        std::istringstream in_stream(vec[i]);
        uint32_t ssrc = 0;
        in_stream >> ssrc;
        ssrcs.push_back(ssrc);
    }
    m_ssrc_groups.push_back(YangSSRCGroup(semantics, ssrcs));

    return err;
}

YangSSRCInfo& YangMediaDesc::fetch_or_create_ssrc_info(uint32_t ssrc)
{
    for (size_t i = 0; i < m_ssrc_infos.size(); ++i) {
        if (m_ssrc_infos[i].m_ssrc == ssrc) {
            return m_ssrc_infos[i];
        }
    }

    YangSSRCInfo ssrc_info;
    ssrc_info.m_ssrc = ssrc;
    m_ssrc_infos.push_back(ssrc_info);

    return m_ssrc_infos.back();
}
