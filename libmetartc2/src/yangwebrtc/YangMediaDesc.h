#ifndef SRC_YANGWEBRTC_YANGMEDIADESC_H_
#define SRC_YANGWEBRTC_YANGMEDIADESC_H_
#include <yangwebrtc/YangSSRCInfo.h>
#include <yangrtp/YangMediaPayloadType.h>
#include <map>


#define Yang_Fetch_Failed 200
#define Yang_Fetch_Failed_WithDelim 200
struct YangCandidate
{
    std::string ip_;
    int32_t port_;
    std::string type_;
};

class YangSessionInfo
{
public:
    YangSessionInfo();
    virtual ~YangSessionInfo();

    int32_t parse_attribute(const std::string& attribute, const std::string& value);
    int32_t encode(std::stringstream& os);

    bool operator=(const YangSessionInfo& rhs);
public:
    std::string m_ice_ufrag;
    std::string m_ice_pwd;
    std::string m_ice_options;
    std::string m_fingerprint_algo;
    std::string m_fingerprint;
    std::string m_setup;
};

class YangMediaDesc
{
public:
    YangMediaDesc(const std::string& type);
    virtual ~YangMediaDesc();
public:
    int32_t parse_line(const std::string& line);
    int32_t encode(std::stringstream& os);
    YangMediaPayloadType* find_media_with_payload_type(int32_t payload_type);
    std::vector<YangMediaPayloadType> find_media_with_encoding_name(const std::string& encoding_name) const;

    const std::map<int, std::string>& get_extmaps() const { return m_extmaps; }
    int32_t update_msid(std::string id);
    bool find_encoding_name(const std::string& encoding_name) const;
    bool is_audio() const { return m_type == "audio"; }
    bool is_video() const { return m_type == "video"; }
private:
    int32_t parse_attribute(const std::string& content);
    int32_t parse_attr_rtpmap(const std::string& value);
    int32_t parse_attr_rtcp(const std::string& value);
    int32_t parse_attr_rtcp_fb(const std::string& value);
    int32_t parse_attr_fmtp(const std::string& value);
    int32_t parse_attr_mid(const std::string& value);
    int32_t parse_attr_msid(const std::string& value);
    int32_t parse_attr_ssrc(const std::string& value);
    int32_t parse_attr_ssrc_group(const std::string& value);
    int32_t parse_attr_extmap(const std::string& value);
private:
    YangSSRCInfo& fetch_or_create_ssrc_info(uint32_t ssrc);

public:
    YangSessionInfo m_session_info;
    std::string m_type;
    int32_t m_port;

    bool m_rtcp_mux;
    bool m_rtcp_rsize;

    bool m_sendonly;
    bool m_recvonly;
    bool m_sendrecv;
    bool m_inactive;

    std::string m_mid;
    std::string m_msid;
    std::string m_msid_tracker;
    std::string m_protos;
    std::vector<YangMediaPayloadType> m_payload_types;

    std::vector<YangCandidate> m_candidates;
    std::vector<YangSSRCGroup> m_ssrc_groups;
    std::vector<YangSSRCInfo>  m_ssrc_infos;
    std::map<int, std::string> m_extmaps;
};
#endif /* SRC_YANGWEBRTC_YANGMEDIADESC_H_ */
