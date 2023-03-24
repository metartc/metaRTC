//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YangRtcSdp_H__
#define YangRtcSdp_H__

#include <yangstream/YangStreamType.h>

#include <yangssl/YangSsl.h>
#include <yangsdp/YangSdpType.h>
#include <yangsdp/YangMediaDesc.h>
#include <yangsdp/YangMediaPayloadType.h>



#define Yang_SDP_kTWCCExt  (char*)"http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01"

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

	YangStringVector groups;
	char group_policy[16];

	char msid_semantic[64];
	YangStringVector msids;

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

void yang_create_rtcsdp(YangSdp* sdp);
void yang_destroy_rtcsdp(YangSdp* sdp);
int32_t yang_rtcsdp_encode(YangSdp *sdp, YangBuffer *os);
int32_t yang_rtcsdp_parse(YangSdp* sdp,char* sdp_str);

#endif
