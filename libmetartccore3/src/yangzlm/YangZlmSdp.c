//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangzlm/YangZlmSdp.h>
#include <yangutil/sys/YangSsrc.h>
#include <yangutil/sys/YangMath.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangCString.h>
#include <yangutil/sys/YangHttpSocket.h>
#include <yangsdp/YangRtcSdp.h>
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



	int32_t k=0;
	for(int32_t i=0;i<sdp.media_descs.vsize;i++){
		YangMediaDesc* desc=&sdp.media_descs.payload[i];
		if(strlen(session->remoteIcePwd)==0){
			memset(session->remoteIcePwd,0,sizeof(session->remoteIcePwd));
			strcpy(session->remoteIcePwd,desc->session_info.ice_pwd);
		}
		if(strlen(session->remote_ufrag)==0){
			memset(session->remote_ufrag,0,sizeof(session->remote_ufrag));
			strcpy(session->remote_ufrag,desc->session_info.ice_ufrag);
		}
		if(yang_strcmp(desc->type,"audio")==0){
			if(desc->ssrc_infos.vsize>0) session->context.audioTrack.ssrc=desc->ssrc_infos.payload[0].ssrc;

			for(k=0;k<desc->payload_types.vsize;k++){

				if(yang_strcmp(desc->payload_types.payload[k].encoding_name,"opus")==0){
					session->remote_audio->encode=Yang_AED_OPUS;
                    session->remote_audio->sample=desc->payload_types.payload[k].clock_rate;
					session->remote_audio->channel=atoi(desc->payload_types.payload[k].encoding_param);
                    session->remote_audio->audioClock=desc->payload_types.payload[k].clock_rate;
				}
			}

		}else if(yang_strcmp(desc->type,"video")==0){

			if(desc->ssrc_infos.vsize>0) {
				for(k=0;k<desc->ssrc_infos.vsize;k++){
					YangRtcTrack videoTrack;
					memset(&videoTrack,0,sizeof(YangRtcTrack));
					videoTrack.ssrc=desc->ssrc_infos.payload[k].ssrc;
					yang_insert_YangRtcTrackVector(&session->context.videoTracks,&videoTrack);

				}
			}



			for(k=0;k<desc->payload_types.vsize;k++){

				if(yang_strcmp(desc->payload_types.payload[k].encoding_name,"H264")==0){
					session->remote_video->encode=Yang_VED_264;
					session->context.codec=Yang_VED_264;
				}else if(yang_strcmp(desc->payload_types.payload[k].encoding_name,"H265")==0){
					session->context.codec=Yang_VED_265;
					session->remote_video->encode=Yang_VED_265;
				}

				session->remote_video->videoClock=90000;
			}
			for (k = 0; k < desc->extmaps.vsize; k++) {
				if (strstr(desc->extmaps.payload[k].extmap,
						"http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01")) {
					session->context.twcc_id = desc->extmaps.payload[k].mapid;
				}
			}
		}
		// set track fec_ssrc and rtx_ssrc
		if (desc->ssrc_groups.vsize > 0) {
			for (k = 0; k < desc->ssrc_groups.vsize; k++) {
				YangSSRCGroup *ssrc_group = &desc->ssrc_groups.payload[k];
				YangRtcTrack *track_desc = yang_zlm_find_track(session,	ssrc_group->groups.payload[0]);
				if (track_desc) {
					if (strcmp(ssrc_group->semantic, "FID") == 0) {
						track_desc->rtx_ssrc = ssrc_group->groups.payload[1];
					} else if (strcmp(ssrc_group->semantic, "FEC") == 0) {
						track_desc->fec_ssrc = ssrc_group->groups.payload[1];
					}
				}
			}
		}
	}

	if(session->context.videoTracks.vsize){
		yang_trace("\nremote audiossrc==%d,remote videossrc==%d,audio sample==%d,audio channel==%d",session->context.audioTrack.ssrc,
			session->context.videoTracks.payload[0].ssrc,session->remote_audio->sample,session->remote_audio->channel);
	}

	if(session->context.avcontext&&session->context.avcontext->setPlayMediaConfig){
		session->context.avcontext->setPlayMediaConfig(session->remote_audio,session->remote_video,session->context.user);
	}

	yang_destroy_rtcsdp(&sdp);
	return err;
}
