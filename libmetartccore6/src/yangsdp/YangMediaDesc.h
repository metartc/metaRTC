//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef SRC_YANGWEBRTC_YANGMEDIADESC_H_
#define SRC_YANGWEBRTC_YANGMEDIADESC_H_
#include <yangutil/buffer/YangBuffer.h>

#include <yangsdp/YangSdpType.h>
#include <yangsdp/YangSSRCInfo.h>
#include <yangsdp/YangMediaPayloadType.h>

typedef struct
{
	int32_t port;
	YangSocketProtocol socketProtocol;
    char ip[64];
    char type[32];
}YangCandidate;

typedef struct{
    char ice_ufrag[32];
    char ice_pwd[128];
    char ice_options[128];
    char fingerprint_algo[128];
    char fingerprint[128];
    char setup[128];
}YangSessionInfo;

typedef struct{
	int32_t mapid;
	char extmap[128];
}YangExtmap;

yang_vector_declare(YangCandidate)

yang_vector_declare(YangExtmap)


typedef struct{
	int32_t port;
	int32_t sctp_port;
	int32_t rtcp_mux;
	int32_t rtcp_rsize;


	int32_t sendonly;
	int32_t recvonly;
	int32_t sendrecv;
	int32_t inactive;

	char type[16];
	char mid[64];
	char msid[64];
	char msid_tracker[64];
	char protos[64];

	YangSessionInfo session_info;
	YangMediaPayloadTypeVector payload_types;
	YangCandidateVector candidates;
	YangSSRCGroupVector ssrc_groups;
	YangSSRCInfoVector  ssrc_infos;
	YangExtmapVector extmaps;
}YangMediaDesc;

yang_vector_declare(YangMediaDesc)

void yang_create_mediadesc(YangMediaDesc* desc);
void yang_destroy_mediadesc(YangMediaDesc* desc);
int32_t yang_encode_sessionInfo(YangSessionInfo* info,YangBuffer* os);
int32_t yang_encode_mediadesc(YangMediaDesc* desc,YangBuffer* os);
int32_t yang_mediadesc_update_msid(YangMediaDesc* desc,char* id);
int32_t yang_sessioninfo_parse_attribute(YangSessionInfo* info, char* attribute, char* value);
int32_t yang_mediadesc_parse_line(YangMediaDesc* desc,char* line);

#endif /* SRC_YANGWEBRTC_YANGMEDIADESC_H_ */
