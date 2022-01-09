#include "../yangcsrs/YangSrsSdp.h"
#include <yangutil/sys/YangSsrc.h>
#include <yangutil/sys/YangMath.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangCString.h>
#include <yangutil/sys/YangHttpSocket.h>
#include <stdio.h>
#include <stdlib.h>
#include <yangwebrtc/YangRtcSessionH.h>
#define Yang_SDP_BUFFERLEN 1024*12
int32_t yang_srs_handlePlaySdp(YangRtcSession* session,SrsSdpResponseType* srs) ;
void yang_destroy_srsresponse(SrsSdpResponseType* srs){
	if(srs==NULL) return;
	yang_free(srs->sdp);
	yang_free(srs->serverIp);
	yang_free(srs->sessionid);
}

int32_t yang_sdp_querySrs(YangRtcSession* session,SrsSdpResponseType* srs,int32_t isplay,char* ip,int32_t port,char* purl, char* psdp)
{

	int32_t err=Yang_Ok;
	yang_trace("\n************************querySrs****************************\n");

	char* sdp=(char*)calloc(1,Yang_SDP_BUFFERLEN);
	if(yang_http_post(sdp,ip, port, purl, (uint8_t*)psdp, strlen(psdp))){
		yang_free(sdp);
		return yang_error_wrap(1,"query srs sdp failure!");
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
	yang_cstr_replace(sdp,sBuffer, "\\r\\n", "\r\n");


	//vector<string> sb=yang_split(sBuffer, ',');
	YangStrings strs;
	memset(&strs,0,sizeof(YangStrings));
	yang_cstr_split(sBuffer, (char*)",",&strs);


	char* p=NULL;
	for (int32_t i = 0; i < strs.vlen; i++) {

		if ((p = strstr(strs.str[i], "\"code\":"))) {
			char *buf = (char*) calloc(1, strlen(p) + 1);
			yang_cstr_replace(p + strlen("\"code\":"), buf, "\"", "");
			srs->retcode = atoi(buf);
			yang_free(buf);
			if ((err = srs->retcode) != 0)	break;

			continue;
		}

		if ((p = strstr(strs.str[i], "\"server\":"))) {
			srs->serverIp = (char*) calloc(1, strlen(p) + 1);
			yang_cstr_replace(p + strlen("\"server\":"), srs->serverIp, "\"", "");

			continue;
		}

		if ((p = strstr(strs.str[i], "\"sessionid\":"))) {

			srs->sessionid = (char*) calloc(1, strlen(p) + 1);
			yang_cstr_replace(p + strlen("\"sessionid\":"), srs->sessionid, "\"","");

			continue;
		}

		if ((p = strstr(strs.str[i], "\"sdp\":"))) {
			srs->sdp = (char*) calloc(1, strlen(p) + 1);
			yang_cstr_replace(p + strlen("\"sdp\":"), srs->sdp, "\"", "");

			continue;
		}

	}


    if(err==Yang_Ok) yang_srs_handlePlaySdp(session,srs);
	yang_destroy_strings(&strs);
	yang_free(sdp);

	return err;

}

int32_t yang_cstr_userindex(char* p,char c){
	int slen=strlen(p);
	for(int32_t i=0;i<slen;i++){
		if(p[i]==c) return i+1;//\n
	}
	return 0;
}
int32_t yang_srs_handlePlaySdp(YangRtcSession* session,SrsSdpResponseType* srs) {
	int32_t err = Yang_Ok;
	char* sdp=srs->sdp;

	if(session->context.remote_audio==NULL) session->context.remote_audio=(YangAudioParam*)calloc(sizeof(YangAudioParam),1);//new YangAudioParam();
	if(session->context.remote_video==NULL) session->context.remote_video=(YangVideoParam*)calloc(sizeof(YangVideoParam),1);//new YangVideoParam();
	char* p=strstr(sdp,"opus");
	char tmp[1024];
	int32_t ind=0;
	if(strstr(sdp,"m=audio")&&p){
		ind=yang_cstr_userindex(p,'\n');
		memset(tmp,0,sizeof(tmp));
		memcpy(tmp,p,ind);
		YangStrings strs;
		memset(&strs,0,sizeof(YangStrings));
		yang_cstr_split(tmp, (char*)"/",&strs);
		session->context.remote_audio->encode=Yang_AED_OPUS;
		session->context.remote_audio->sample=atoi(strs.str[1]);//payload.m_clock_rate;
		session->context.remote_audio->channel=atoi(strs.str[2]);//atoi(payload.m_encoding_param.c_str());
		session->context.remote_audio->audioClock=atoi(strs.str[1]);
		yang_destroy_strings(&strs);
	}

	p=strstr(sdp,"H264/90000");
	if(p){
		session->context.remote_video->encode=Yang_VED_264;
		session->context.remote_video->videoClock=90000;//payload.m_clock_rate;
	}else{
		p=strstr(sdp,"H265/90000");
		if(p){
			session->context.remote_video->encode=Yang_VED_265;
			session->context.remote_video->videoClock=90000;//payload.m_clock_rate;
		}

	}

	p=strstr(sdp,"ice-pwd:");
	if(p){
		ind=yang_cstr_userindex(p,'\n');
		memcpy(session->remoteIcePwd,p,ind);
	}
	p=strstr(sdp,"m=audio");
	char* p1=strstr(sdp,"m=video");
	char audiosdp[1024*10];
	char ssrc[20];
	if(p&&p1){
		//int32_t ind=yang_cstr_spaceindex(p);
		memset(audiosdp,0,sizeof(audiosdp));
		memcpy(audiosdp,p,p1-p);

		p=strstr(audiosdp,"ssrc:");


		if(p&&session->context.audioTrack.ssrc_==0){

			ind=yang_cstr_userindex(p,' ');
			if(ind<20) {
				memset(ssrc,0,sizeof(ssrc));
				memcpy(ssrc,p+5,ind-5);
				session->context.audioTrack.ssrc_=atoi(ssrc);

			}
		}
		p=strstr(p1,"ssrc:");

		if(p&&session->context.videoTrack.ssrc_==0){
			ind=yang_cstr_userindex(p,' ');
			if(ind<20) {
				memset(ssrc,0,sizeof(ssrc));
				memcpy(ssrc,p+5,ind-5);
				session->context.videoTrack.ssrc_=atoi(ssrc);

			}
		}


	}


	//if(m_conf){
	//	m_conf->streams.setMediaConfig(session->context.streamConf->uid,	session->context.remote_audio,session->context.remote_video);
	//}
	return err;
}
