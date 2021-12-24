#ifndef YangSdp_H__
#define YangSdp_H__
#include <string>
#include <vector>
#include <map>
#include <stdint.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangstream/YangStreamType.h>
#include <yangwebrtc/YangRtcContext.h>
#include <yangwebrtc/YangRtcDtls.h>
#include <yangrtp/YangMediaPayloadType.h>
#include <yangwebrtc/YangMediaDesc.h>
const std::string kTWCCExt = "http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01";

class YangSessionConfig
{
public:
    std::string m_dtls_role;
    std::string m_dtls_version;
};



struct H264SpecificParam
{
    std::string profile_level_id;
    std::string packetization_mode;
    std::string level_asymmerty_allow;
};

class YangSdp
{
public:
    YangSdp();
    virtual ~YangSdp();
public:
    int32_t parse(const std::string& sdp_str);
    int32_t encode(std::stringstream& os);
public:
    YangMediaDesc* find_media_descs(const std::string& type);
public:
    void set_ice_ufrag(const std::string& ufrag);
    void set_ice_pwd(const std::string& pwd);
    void set_dtls_role(const std::string& dtls_role);
    void set_fingerprint_algo(const std::string& algo);
    void set_fingerprint(const std::string& fingerprint);
    void add_candidate(const std::string& ip, const int& port, const std::string& type);

    std::string get_ice_ufrag() const;
    std::string get_ice_pwd() const;
    std::string get_dtls_role() const;

private:
    int32_t parse_line(const std::string& line);
private:
    int32_t parse_origin(const std::string& content);
    int32_t parse_version(const std::string& content);
    int32_t parse_session_name(const std::string& content);
    int32_t parse_timing(const std::string& content);
    int32_t parse_attribute(const std::string& content);
    int32_t parse_media_description(const std::string& content);
    int32_t parse_attr_group(const std::string& content);
private:
    bool m_in_media_session;
public:
    // version
    std::string m_version;

    // origin
    std::string m_username;
    std::string m_session_id;
    std::string m_session_version;
    std::string m_nettype;
    std::string m_addrtype;
    std::string m_unicast_address;

    // session_name
    std::string m_session_name;

    // timing
    int64_t m_start_time;
    int64_t m_end_time;

    YangSessionInfo m_session_info;
    YangSessionConfig m_session_config;
    YangSessionConfig m_session_negotiate;

    std::vector<std::string> m_groups;
    std::string m_group_policy;

    std::string m_msid_semantic;
    std::vector<std::string> m_msids;

    // m-line, media sessions
    std::vector<YangMediaDesc> m_media_descs;

     bool is_unified() const;
    // TODO: FIXME: will be fixed when use single pc.
    int32_t update_msid(std::string id);
};




class YangSdpHandle {
public:
	YangSdpHandle();
	virtual ~YangSdpHandle();
		//role0: play 1:publish 2:play&publish
	int32_t genLocalSdp(int32_t localport,YangRtcContext* conf, char *sdp,YangStreamOptType role);
	void init(YangCertificateImpl* pcer);

private:
	YangCertificateImpl* m_cer;
	int32_t generate_local_sdp(YangRtcContext* pconf, bool unified_plan,int32_t role);
	int32_t generate_publish_local_sdp(YangStreamConfig* req, YangSdp& local_sdp,  bool unified_plan);
};
#endif
