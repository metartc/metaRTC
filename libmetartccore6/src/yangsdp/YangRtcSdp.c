//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangsdp/YangRtcSdp.h>

#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangSsrc.h>
#include <yangutil/sys/YangCString.h>

#include <yangssl/YangSsl.h>
#include <yangsdp/YangMediaDesc.h>
#include <yangsdp/YangMediaPayloadType.h>

#define  kCRLF  (char*)"\r\n"

void yang_create_rtcsdp(YangSdp *sdp) {
	if(sdp==NULL) return;
	sdp->in_media_session = yangfalse;
	yang_itoa(0, sdp->version, 10);
	sdp->start_time = 0;
	sdp->end_time = 0;
}
void yang_destroy_rtcsdp(YangSdp *sdp) {
	if(sdp==NULL) return;
	for(int i=0;i<sdp->media_descs.vsize;i++){
		yang_destroy_mediadesc(&sdp->media_descs.payload[i]);
	}
	yang_destroy_YangMediaDescVector(&sdp->media_descs);
}

char* yang_rtcsdp_getnull(char *os) {
	char *p = os;
	while (1) {
		if (*p == 0)
			return p;
		p++;
	}
}

YangMediaDesc* yang_rtcsdp_find_media_descs(YangSdp *sdp, char *type) {


	for (int i = 0; i < sdp->media_descs.vsize; i++) {
		if (yang_strcmp(sdp->media_descs.payload[i].type, type) == 0)
			return &sdp->media_descs.payload[i];
	}
	return NULL;
}

void yang_rtcsdp_set_ice_ufrag(YangSdp *sdp, char *ufrag) {

	for (int i = 0; i < sdp->media_descs.vsize; i++) {
		yang_strcpy(sdp->media_descs.payload[i].session_info.ice_ufrag, ufrag);
	}
}

void yang_rtcsdp_set_ice_pwd(YangSdp *sdp, char *pwd) {

	for (int i = 0; i < sdp->media_descs.vsize; i++) {
		yang_strcpy(sdp->media_descs.payload[i].session_info.ice_pwd, pwd);
	}
}

void yang_rtcsdp_set_dtls_role(YangSdp *sdp, char *dtls_role) {

	for (int i = 0; i < sdp->media_descs.vsize; i++) {
		yang_strcpy(sdp->media_descs.payload[i].session_info.setup, dtls_role);
	}
}

void yang_rtcsdp_set_fingerprint_algo(YangSdp *sdp, char *algo) {

	for (int i = 0; i < sdp->media_descs.vsize; i++) {
		yang_strcpy(sdp->media_descs.payload[i].session_info.fingerprint_algo, algo);
	}

}

void yang_rtcsdp_set_fingerprint(YangSdp *sdp, char *fingerprint) {

	for (int i = 0; i < sdp->media_descs.vsize; i++) {
		yang_strcpy(sdp->media_descs.payload[i].session_info.fingerprint,
				fingerprint);
	}

}

void yang_rtcsdp_add_candidate(YangSdp *sdp, char *ip, int port, char *type) {
	// @see: https://tools.ietf.org/id/draft-ietf-mmusic-ice-sip-sdp-14.html#rfc.section.5.1
	YangCandidate candidate;

	yang_strcpy(candidate.ip, ip);
	candidate.port = port;
	yang_strcpy(candidate.type, type);


	for (int i = 0; i < sdp->media_descs.vsize; i++) {
		yang_insert_YangCandidateVector(&sdp->media_descs.payload[i].candidates,
				&candidate);
	}
}

char* yang_rtcsdp_get_ice_ufrag(YangSdp *sdp) {
	// Becaues we use BUNDLE, so we can choose the first element.

	for (int i = 0; i < sdp->media_descs.vsize; i++) {
		return sdp->media_descs.payload[i].session_info.ice_ufrag;
	}
	return "";
}

char* yang_rtcsdp_get_ice_pwd(YangSdp *sdp) {
	// Becaues we use BUNDLE, so we can choose the first element.

	for (int i = 0; i < sdp->media_descs.vsize; i++) {
		return sdp->media_descs.payload[i].session_info.ice_pwd;
	}
	return "";
}

char* yang_rtcsdp_get_dtls_role(YangSdp *sdp) {
	// Becaues we use BUNDLE, so we can choose the first element.

	for (int i = 0; i < sdp->media_descs.vsize; i++) {
		return sdp->media_descs.payload[i].session_info.setup;
	}
	return "";
}

int32_t yang_rtcsdp_parse_origin(YangSdp *sdp, char *content) {
	int32_t err = Yang_Ok;
	YangStrings str;
	yang_cstr_split(content, " ", &str);
	if (str.vsize < 5)
		return 1;
	// @see: https://tools.ietf.org/html/rfc4566#section-5.2
	// o=<username> <sess-id> <sess-version> <nettype> <addrtype> <unicast-address>
	// eg. o=- 9164462281920464688 2 IN IP4 127.0.0.1

	yang_strcpy(sdp->username, str.str[0]);
	yang_strcpy(sdp->session_id, str.str[1]);
	yang_strcpy(sdp->session_version, str.str[2]);
	yang_strcpy(sdp->nettype, str.str[3]);
	yang_strcpy(sdp->addrtype, str.str[4]);
	yang_strcpy(sdp->unicast_address, str.str[5]);

	yang_destroy_strings(&str);
	return err;
}

int32_t yang_rtcsdp_parse_version(YangSdp *sdp, char *content) {
	int32_t err = Yang_Ok;
	// @see: https://tools.ietf.org/html/rfc4566#section-5.1
	if (yang_strlen(content))
		yang_strcpy(sdp->version, content);
	else
		yang_itoa(0, sdp->version, 10);

	return err;
}

int32_t yang_rtcsdp_parse_session_name(YangSdp *sdp, char *content) {
	int32_t err = Yang_Ok;
	// @see: https://tools.ietf.org/html/rfc4566#section-5.3
	// s=<session name>
	yang_strcpy(sdp->session_name, content);

	return err;
}

int32_t yang_rtcsdp_parse_timing(YangSdp *sdp, char *content) {
	int32_t err = Yang_Ok;

	// @see: https://tools.ietf.org/html/rfc4566#section-5.9
	// t=<start-time> <stop-time>
	YangStrings str;
	yang_cstr_split(content, " ", &str);

	if (str.vsize < 2)
		return 1;

	sdp->start_time = atol(str.str[0]);
	sdp->end_time = atol(str.str[1]);

	yang_destroy_strings(&str);
	return err;
}

int32_t yang_rtcsdp_parse_attr_group(YangSdp *sdp, char *value) {
	int32_t err = Yang_Ok;
	// @see: https://tools.ietf.org/html/rfc5888#section-5
	YangStrings str;
	yang_cstr_split(value, " ", &str);
	if (str.vsize == 0)
		return 1;

	yang_strcpy(sdp->group_policy, str.str[0]);
	for (int i = 1; i < str.vsize; i++) {
		yang_insert_stringVector(&sdp->groups, str.str[i]);
	}

	yang_destroy_strings(&str);
	return err;
}

int32_t yang_rtcsdp_parse_media_description(YangSdp *sdp, char *content) {
	int32_t err = Yang_Ok;

	// @see: https://tools.ietf.org/html/rfc4566#section-5.14
	// m=<media> <port> <proto> <fmt> ...
	// m=<media> <port>/<number of ports> <proto> <fmt> ...

	YangStrings str;
	yang_cstr_split(content, " ", &str);
	if (str.vsize < 4)
		return yang_error_wrap(1,
				"rtcsdp_parse_media_description error content=%s", content);

	YangMediaDesc md;
	yang_memset(&md, 0, sizeof(YangMediaDesc));
	yang_insert_YangMediaDescVector(&sdp->media_descs, &md);
	YangMediaDesc *desc = &sdp->media_descs.payload[sdp->media_descs.vsize - 1];
	yang_strcpy(desc->type, str.str[0]);
	yang_strcpy(desc->protos, str.str[2]);
	desc->port = atoi(str.str[1]);
	for (int32_t i = 3; i < str.vsize; i++) {
		YangMediaPayloadType pt;
		yang_memset(&pt, 0, sizeof(YangMediaPayloadType));
		pt.payload_type = atoi(str.str[i]);
		yang_insert_YangMediaPayloadTypeVector(&desc->payload_types, &pt);
		yang_create_stringVector(&desc->payload_types.payload[desc->payload_types.vsize-1].rtcp_fb);
	}

	if (!sdp->in_media_session) {
		sdp->in_media_session = yangtrue;
	}
	yang_destroy_strings(&str);
	return err;
}
int32_t yang_rtcsdp_parse_attribute(YangSdp *sdp, char *content) {
	int32_t err = Yang_Ok;
	// @see: https://tools.ietf.org/html/rfc4566#section-5.13
	// a=<attribute>
	// a=<attribute>:<value>
	char *p = yang_strstr(content, ":");

	char attribute[256];
	char value[256];
	yang_memset(attribute, 0, sizeof(attribute));
	yang_memset(value, 0, sizeof(value));
	if (p) {
		yang_memcpy(attribute, content, p - content);
		yang_strcpy(value, p + 1);

	}
	if (yang_strcmp(attribute, "group") == 0) {
		return yang_rtcsdp_parse_attr_group(sdp, value);
	} else if (yang_strcmp(attribute, "msid-semantic") == 0) {
		YangStrings str;
		yang_cstr_split(value, " ", &str);
		yang_strcpy(sdp->msid_semantic, str.str[0]);
		for (int j = 1; j < str.vsize; j++) {
			yang_insert_stringVector(&sdp->msids, str.str[j]);
		}
		yang_destroy_strings(&str);
	} else {
		return yang_sessioninfo_parse_attribute(&sdp->session_info, attribute,
				value);
	}

	return err;
}

int32_t yang_rtcsdp_is_unified(YangSdp *sdp) {

	return sdp->media_descs.vsize > 2 ? 1 : 0;
}

int32_t yang_rtcsdp_update_msid(YangSdp *sdp, char *id) {
	int32_t err = Yang_Ok;
	yang_clear_stringVector(&sdp->msids);
	yang_insert_stringVector(&sdp->msids, id);


	for (int i = 0; i < sdp->media_descs.vsize; i++) {

		YangMediaDesc *desc = &sdp->media_descs.payload[i];

		if ((err = yang_mediadesc_update_msid(desc, id)) != Yang_Ok) {
			yang_trace("\ndesc %s update msid %s", desc->mid, id);
			return err;
		}
	}

	return err;
}

int32_t yang_rtcsdp_parse_line(YangSdp *sdp, char *line) {
	int32_t err = Yang_Ok;

	char *content = line + 2;

	switch (line[0]) {
	case 'o': {
		return yang_rtcsdp_parse_origin(sdp, content);
	}
	case 'v': {
		return yang_rtcsdp_parse_version(sdp, content);
	}
	case 's': {
		return yang_rtcsdp_parse_session_name(sdp, content);
	}
	case 't': {
		return yang_rtcsdp_parse_timing(sdp, content);
	}
	case 'a': {
		if (sdp->in_media_session) {
			return yang_mediadesc_parse_line(
					&sdp->media_descs.payload[sdp->media_descs.vsize - 1], line);
		}
		return yang_rtcsdp_parse_attribute(sdp, content);
	}
	case 'm': {
		return yang_rtcsdp_parse_media_description(sdp, content);
	}
	case 'c': {

		break;
	}
	default: {
		yang_trace("ignore sdp line=%s", line);
		break;
	}
	}

	return err;
}

int32_t yang_rtcsdp_encode(YangSdp *sdp, YangBuffer *os) {
	int32_t err = Yang_Ok;
	char *p = NULL;
	char tmp[2048];
	yang_memset(tmp, 0, sizeof(tmp));
	yang_sprintf(tmp, "v=%s%s"
			"o=%s %s %s %s %s %s%s"
			"s=%s%s"
			"t=%" PRId64" %" PRId64"%s"
			"a=ice-lite%s", sdp->version, kCRLF, sdp->username, sdp->session_id,
			sdp->session_version, sdp->nettype, sdp->addrtype,
			sdp->unicast_address, kCRLF, sdp->session_name, kCRLF,
			sdp->start_time, sdp->end_time, kCRLF,
			kCRLF);


	if (!sdp->groups.vsize) {
		p = yang_rtcsdp_getnull(tmp);
		yang_sprintf(p, "a=group:%s", sdp->group_policy);
		for (int i = 0; i < sdp->groups.vsize; i++) {
			p = yang_rtcsdp_getnull(tmp);
			yang_sprintf(p, " %s", sdp->groups.payload[i]);
		}
		p = yang_rtcsdp_getnull(tmp);
		yang_sprintf(p, "%s", kCRLF);

	}
	p = yang_rtcsdp_getnull(tmp);
	yang_sprintf(p, "a=msid-semantic: %s", sdp->msid_semantic);
	for (int i = 0; i < sdp->msids.vsize; i++) {
		p = yang_rtcsdp_getnull(tmp);
		yang_sprintf(p, " %s", sdp->msids.payload[i]);
	}
	p = yang_rtcsdp_getnull(tmp);
	yang_sprintf(p, "%s", kCRLF);

	yang_write_cstring(os, tmp);
	if ((err = yang_encode_sessionInfo(&sdp->session_info, os)) != Yang_Ok) {
		return printf("encode session info failed");
	}


	for (int i = 0; i < sdp->media_descs.vsize; i++) {

		if ((err = yang_encode_mediadesc(&sdp->media_descs.payload[i], os))
				!= Yang_Ok) {
			return printf("encode media description failed");
		}
	}

	return err;
}

int32_t yang_rtcsdp_parse(YangSdp *sdp, char *sdp_str) {
	int32_t err = Yang_Ok;
	YangStrings strs;

	yang_cstr_split(sdp_str, "\n", &strs);
	//printf("\nsdp===%s\n",sdp_str.c_str());
	// All webrtc SrsSdp annotated example
	// @see: https://tools.ietf.org/html/draft-ietf-rtcweb-SrsSdp-11
	// Sdp example
	// session info
	// v=
	// o=
	// s=
	// t=
	// media description
	// m=
	// a=
	// ...
	// media description
	// m=
	// a=
	// ...
	// std::istringstream is(sdp_str);
	// std::string line;
	int32_t i = 0;
	for (i = 0; i < strs.vsize; i++) {
		char *line = strs.str[i];
		//yang_trace("%s", line.c_str());
		if (yang_strlen(line) < 2 || line[1] != '=') {
			return yang_error_wrap(ERROR_RTC_SDP_DECODE, "invalid sdp line=%s",
					line);
		}

		if ((err = yang_rtcsdp_parse_line(sdp, line)) != Yang_Ok) {
			return yang_error_wrap(1, "parse sdp line failed:%s",line);
		}
	}

	// The msid/tracker/mslabel is optional for SSRC, so we copy it when it's empty.

	for (i = 0; i < sdp->media_descs.vsize; i++) {
		YangMediaDesc *media_desc = &sdp->media_descs.payload[i];
		for (size_t j = 0; j < media_desc->ssrc_infos.vsize; ++j) {

			YangSSRCInfo *ssrc_info = &media_desc->ssrc_infos.payload[j];
			if (yang_strlen(ssrc_info->msid) == 0) {
				yang_strcpy(ssrc_info->msid, media_desc->msid);
			}

			if (yang_strlen(ssrc_info->msid_tracker) == 0) {
				yang_strcpy(ssrc_info->msid_tracker, media_desc->msid_tracker);
			}

			if (yang_strlen(ssrc_info->mslabel) == 0) {
				yang_strcpy(ssrc_info->mslabel, media_desc->msid);
			}

			if (yang_strlen(ssrc_info->label) == 0) {
				yang_strcpy(ssrc_info->label, media_desc->msid_tracker);
			}
		}
	}
	yang_destroy_strings(&strs);
	return err;
}

