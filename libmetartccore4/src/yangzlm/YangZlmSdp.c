//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangzlm/YangZlmSdp.h>
#include <yangutil/sys/YangSsrc.h>
#include <yangutil/sys/YangMath.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangCString.h>
#include <yangutil/sys/YangHttpSocket.h>
#include <yangsdp/YangSdp.h>
#include <stdio.h>
#include <stdlib.h>
#include <yangrtc/YangRtcSessionH.h>
#define Yang_SDP_BUFFERLEN 1024*12
int32_t yang_zlm_handlePlaySdp(YangRtcSession* session,ZlmSdpResponseType* zlm) ;
void yang_destroy_zlmresponse(ZlmSdpResponseType* zlm){
	if(zlm==NULL) return;
	yang_free(zlm->sdp);
	yang_free(zlm->id);
}

int32_t yang_zlm_query(YangRtcSession* session,ZlmSdpResponseType* zlm,int32_t isplay,char* ip,int32_t port,char* purl, char* psdp)
{

	int32_t err=Yang_Ok;

	char* sdp=(char*)calloc(1,Yang_SDP_BUFFERLEN);
	if(yang_http_post(sdp,ip, port, purl, (uint8_t*)psdp, strlen(psdp))){
		yang_free(sdp);
		return yang_error_wrap(1,"query zlm sdp failure!");
	}

	char* sBuffer=(char*)calloc(1,Yang_SDP_BUFFERLEN);

	yang_cstr_replace(sdp,sBuffer, "{", "");
	memset(sdp,0,Yang_SDP_BUFFERLEN);
	strcpy(sdp,sBuffer);
	memset(sBuffer,0,Yang_SDP_BUFFERLEN);
	yang_cstr_replace(sdp,sBuffer, "}", "");

	memset(sdp,0,Yang_SDP_BUFFERLEN);
	strcpy(sdp,sBuffer);
	memset(sBuffer,0,Yang_SDP_BUFFERLEN);
	yang_cstr_replace(sdp,sBuffer, "\\r\\n", "\n");

	YangStrings strs;
	memset(&strs,0,sizeof(YangStrings));
	yang_cstr_split(sBuffer, (char*)",",&strs);


	char* p=NULL;
	for (int32_t i = 0; i < strs.vsize; i++) {

		if ((p = strstr(strs.str[i], "\"code\""))) {
			char *buf = (char*) calloc(1, strlen(p) + 1);
			yang_cstr_replace(p + strlen("\"code\" : "), buf, "\"", "");

			zlm->retcode = abs(atoi(buf));
			yang_free(buf);

			if ((err = zlm->retcode) != 0)	break;

			continue;
		}



		if ((p = strstr(strs.str[i], "\"id\""))) {
			zlm->id = (char*) calloc(1, strlen(p) + 1);
			yang_cstr_replace(p + strlen("\"id\" : "), zlm->id, "\"","");
			continue;
		}

		if ((p = strstr(strs.str[i], "\"sdp\""))) {
			char* sdptmp=(char*) calloc(1, strlen(p) + 1);
			zlm->sdp = (char*) calloc(1, strlen(p) + 1);

            yang_cstr_replace(p + strlen("\"sdp\" : "), sdptmp, "\"", "");
            yang_cstr_replace(sdptmp, zlm->sdp, "\r", "");

			continue;
		}

	}

	//err=1;

    if(err==Yang_Ok) yang_zlm_handlePlaySdp(session,zlm);
	yang_destroy_strings(&strs);
	yang_free(sdp);

	return err;

}

YangRtcTrack* yang_zlm_find_track(YangRtcSession* session,uint32_t ssrc){
	if(session==NULL) return NULL;
	if(yang_has_ssrc(&session->context.audioTrack,ssrc)) return &session->context.audioTrack;
	for(int32_t i=0;i<session->context.videoTracks.vsize;i++){
		if(session->context.videoTracks.payload[i].ssrc==ssrc) return &session->context.videoTracks.payload[i];
	}
	return NULL;
}
int32_t yang_zlm_handlePlaySdp(YangRtcSession* session,ZlmSdpResponseType* zlm) {
	int32_t err = Yang_Ok;
	char* sdpstr=zlm->sdp;
	YangSdp sdp;
	memset(&sdp,0,sizeof(YangSdp));
	yang_create_rtcsdp(&sdp);
	if(session->remote_audio==NULL) session->remote_audio=(YangAudioParam*)calloc(sizeof(YangAudioParam),1);
	if(session->remote_video==NULL) session->remote_video=(YangVideoParam*)calloc(sizeof(YangVideoParam),1);

	yang_rtcsdp_parse(&sdp,sdpstr);

	yang_sdp_parseRemoteSdp(session,&sdp);

	yang_destroy_rtcsdp(&sdp);
	return err;
}
