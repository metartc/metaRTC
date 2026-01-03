//
// Copyright (c) 2019-2023 yanggaofeng
//

#include <yangsdp/YangCandidate.h>

#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangCString.h>
#define Yang_CandidateType_Host "host"
#define Yang_CandidateType_Srflx "srflx"
#define Yang_CandidateType_Relay "relay"
#define kCRLF  "\r\n"

int32_t yang_candidate_parse(YangCandidate* candidate,char* candidateStr,YangIpFamilyType familyType){
	YangStrings str;
	yang_cstr_split(candidateStr, " ", &str);
	if(str.vsize<8) {
		yang_destroy_strings(&str);
		return yang_error_wrap(ERROR_RTC_SDP_DECODE, "invalid candidate line=%s",candidateStr);
	}

	candidate->socketProtocol=yang_strcmp(str.str[2],"udp")==0?Yang_Socket_Protocol_Udp:Yang_Socket_Protocol_Tcp;
	yang_addr_set(&candidate->address,str.str[4],yang_atoi(str.str[5]),familyType,candidate->socketProtocol);

	if(yang_memcmp(str.str[7],Yang_CandidateType_Host,4)==0){
		candidate->candidateType=YangIceHost;
	}else if(yang_memcmp(str.str[7],Yang_CandidateType_Srflx,5)==0){
		candidate->candidateType=YangIceServerReflexive;
	}else if(yang_memcmp(str.str[7],Yang_CandidateType_Relay,5)==0){
		candidate->candidateType=YangIceRelayed;
	}
	yang_destroy_strings(&str);
	return Yang_Ok;
}


int32_t yang_candidate_toBuffer(YangCandidate* candidate,int32_t foundation,YangBuffer *os){
	// @see: https://tools.ietf.org/html/draft-ietf-ice-rfc5245bis-00#section-4.2
	char intstr[32];
	char ip[64]={0};
	yang_memset(intstr, 0, sizeof(intstr));

	int32_t component_id = 1; /* RTP */
	uint32_t priority = (1 << 24) * (126) + (1 << 8) * (65535)
							+ (1) * (256 - component_id);
	yang_write_cstring(os, "a=candidate:");
	yang_memset(intstr, 0, sizeof(intstr));
	yang_itoa(foundation, intstr, 10);
	yang_write_cstring(os, intstr);
	yang_write_cstring(os, " ");
	//
	yang_memset(intstr, 0, sizeof(intstr));
	yang_itoa(component_id, intstr, 10);
	yang_write_cstring(os, intstr);
	if(candidate->socketProtocol==Yang_Socket_Protocol_Udp)
		yang_write_cstring(os, " udp ");
	else if(candidate->socketProtocol==Yang_Socket_Protocol_Tcp)
		yang_write_cstring(os, " tcp ");

	yang_memset(intstr, 0, sizeof(intstr));
	yang_itoa(priority, intstr, 10);
	yang_write_cstring(os, intstr);
	yang_write_cstring(os, " ");
	//
	yang_addr_getIPStr(&candidate->address,ip,sizeof(ip));
	yang_write_cstring(os, ip);
	yang_write_cstring(os, " ");
	yang_memset(intstr, 0, sizeof(intstr));
	yang_itoa(yang_addr_getPort(&candidate->address), intstr, 10);
	yang_write_cstring(os, intstr);
	//
	yang_write_cstring(os, " typ ");
	switch(candidate->candidateType){
	case YangIceHost:yang_write_cstring(os, Yang_CandidateType_Host);break;
	case YangIceServerReflexive:yang_write_cstring(os, Yang_CandidateType_Srflx);break;
	case YangIceRelayed:yang_write_cstring(os, Yang_CandidateType_Relay);break;
	default:break;
	}

	//
	yang_write_cstring(os, " generation 0");
	yang_write_cstring(os, kCRLF);

	// @see: https://tools.ietf.org/id/draft-ietf-mmusic-ice-sip-sdp-14.html#rfc.section.5.1
	return Yang_Ok;
}

int32_t yang_candidate_toString(YangCandidate* candidate,int32_t foundation,char **candidateStr){
	char* str;
	char sdp[512]={0};
	YangBuffer os;
	yang_init_buffer(&os,sdp,256);
	yang_candidate_toBuffer(candidate,foundation,&os);
	int32_t len=yang_buffer_pos(&os);

	str=(char*)yang_malloc(len+1);
	yang_memcpy(str,os.data,len);
	str[len]=0;
	*candidateStr=str;

	return Yang_Ok;
}

int32_t yang_candidate_toBuffer2(YangCandidate* candidate,int32_t foundation,char* ufrag,char* stunStr,YangBuffer *os){
	// @see: https://tools.ietf.org/html/draft-ietf-ice-rfc5245bis-00#section-4.2
	char intstr[32];
	char ip[64]={0};
	yang_memset(intstr, 0, sizeof(intstr));

	int32_t component_id = 1; /* RTP */
	uint32_t priority = (1 << 24) * (126) + (1 << 8) * (65535)
							+ (1) * (256 - component_id);
	yang_write_cstring(os, "candidate:");
	yang_memset(intstr, 0, sizeof(intstr));
	yang_itoa(foundation, intstr, 10);
	yang_write_cstring(os, intstr);
	yang_write_cstring(os, " ");
	//
	yang_memset(intstr, 0, sizeof(intstr));
	yang_itoa(component_id, intstr, 10);
	yang_write_cstring(os, intstr);
	if(candidate->socketProtocol==Yang_Socket_Protocol_Udp)
		yang_write_cstring(os, " udp ");
	else if(candidate->socketProtocol==Yang_Socket_Protocol_Tcp)
		yang_write_cstring(os, " tcp ");

	yang_memset(intstr, 0, sizeof(intstr));
	yang_itoa(priority, intstr, 10);
	yang_write_cstring(os, intstr);
	yang_write_cstring(os, " ");
	//
	yang_addr_getIPStr(&candidate->address,ip,sizeof(ip));
	yang_write_cstring(os, ip);
	yang_write_cstring(os, " ");
	yang_memset(intstr, 0, sizeof(intstr));
	yang_itoa(yang_addr_getPort(&candidate->address), intstr, 10);
	yang_write_cstring(os, intstr);
	//
	yang_write_cstring(os, " typ ");

	switch(candidate->candidateType){
	case YangIceHost:yang_write_cstring(os, Yang_CandidateType_Host);break;
	case YangIceServerReflexive:{
		yang_write_cstring(os, Yang_CandidateType_Srflx);
		if(stunStr) yang_write_cstring(os, stunStr);
		break;
	}
	case YangIceRelayed:yang_write_cstring(os, Yang_CandidateType_Relay);break;
	default:break;
	}

	//
	yang_write_cstring(os, " generation 0 ufrag ");
	yang_write_cstring(os, ufrag);
	yang_write_cstring(os, " network-cost 999");
	//yang_write_cstring(os, kCRLF);

	// @see: https://tools.ietf.org/id/draft-ietf-mmusic-ice-sip-sdp-14.html#rfc.section.5.1
	return Yang_Ok;
}

int32_t yang_candidate_toJson(YangCandidate* candidate,int32_t foundation,char* ufrag,char* stunStr,char **candidateStr){
	int32_t mid=0,len;
	char* str;
	char sdp[256]={0};
	char sdpStr[512]={0};

	YangBuffer os;
	yang_init_buffer(&os,sdp,sizeof(sdp));
	yang_candidate_toBuffer2(candidate,foundation,ufrag,stunStr,&os);

	//char* candidateStr=NULL;
	sprintf(sdpStr,"{\"candidate\":\"%s\",\"sdpMid\":\"%d\",\"sdpMLineIndex\":%d,\"usernameFragment\":\"%s\"}",
			sdp,mid,foundation,ufrag);
	len=yang_strlen(sdpStr);
	str=(char*)yang_malloc(len+1);
	yang_memcpy(str,sdpStr,len);
	str[len]=0;
	*candidateStr=str;
	return Yang_Ok;

}


