#ifndef YangRtcSdp_H__
#define YangRtcSdp_H__

#include <stdint.h>

#include <yangstream/YangStreamType.h>

#include <yangutil/sys/YangSsl.h>
#include <yangsdp/YangMediaDesc.h>
#include <yangsdp/YangMediaPayloadType.h>
#include <yangsdp/YangSdpType.h>

#define kTWCCExt  (char*)"http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01"

typedef struct
{
    char dtls_role[64];
    char dtls_version[16];
}YangSessionConfig;



typedef struct
{
    char profile_level_id[16];
    char packetization_mode[32];
    char level_asymmerty_allow[16];
}H264SpecificParam;



typedef struct{
	 int32_t in_media_session;



		    // timing
		    int64_t start_time;
		    int64_t end_time;

		    YangSessionInfo session_info;
		    YangSessionConfig session_config;
		    YangSessionConfig session_negotiate;

		    YangStrVector groups;
		    char group_policy[16];

		    char msid_semantic[64];
		    YangStrVector msids;

		    // m-line, media sessions
		    YangMediaDescVector media_descs;
		    // version
		    char version[16];

		    // origin
		    char username[128];
		    char session_id[32];
		    char session_version[128];
		    char nettype[16];
		    char addrtype[32];
		    char unicast_address[32];

		    // session_name
		    char session_name[64];
}YangSdp;

void yang_init_rtcsdp(YangSdp* sdp);
void yang_destroy_rtcsdp(YangSdp* sdp);
int32_t yang_rtcsdp_parse(YangSdp* sdp,char* sdp_str);
/**
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


     int32_t is_unified() const;
    // TODO: FIXME: will be fixed when use single pc.
    int32_t update_msid(std::string id);
};
**/




#endif
