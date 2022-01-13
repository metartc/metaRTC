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


int32_t yang_srs_handlePlaySdp(YangRtcSession* session,SrsSdpResponseType* srs) {
	int32_t err = Yang_Ok;
	char* sdp=srs->sdp;

	if(session->remote_audio==NULL) session->remote_audio=(YangAudioParam*)calloc(sizeof(YangAudioParam),1);
	if(session->remote_video==NULL) session->remote_video=(YangVideoParam*)calloc(sizeof(YangVideoParam),1);
	char* p=strstr(sdp,"opus");
	char tmp[1024];
	int32_t ind=0;
	if(strstr(sdp,"m=audio")&&p){
		ind=yang_cstr_userfindindex(p,'\n');
		memset(tmp,0,sizeof(tmp));
		memcpy(tmp,p,ind);
		YangStrings strs;
		memset(&strs,0,sizeof(YangStrings));
		yang_cstr_split(tmp, (char*)"/",&strs);
		session->remote_audio->encode=Yang_AED_OPUS;
		session->remote_audio->sample=atoi(strs.str[1]);
		session->remote_audio->channel=atoi(strs.str[2]);
		session->remote_audio->audioClock=atoi(strs.str[1]);
		yang_destroy_strings(&strs);
	}

	p=strstr(sdp,"H264/90000");
	if(p){
		session->remote_video->encode=Yang_VED_264;
		session->remote_video->videoClock=90000;
	}else{
		p=strstr(sdp,"H265/90000");
		if(p){
			session->remote_video->encode=Yang_VED_265;
			session->remote_video->videoClock=90000;
		}

	}

	p=strstr(sdp,"ice-pwd:");
	if(p){
		ind=yang_cstr_userfindindex(p,'\n');
		memcpy(session->remoteIcePwd,p,ind);
	}
	p=strstr(sdp,"m=audio");
	char* p1=strstr(sdp,"m=video");
	char audiosdp[1024*10];
	char sid[20];
	if(p&&p1){

		memset(audiosdp,0,sizeof(audiosdp));
		memcpy(audiosdp,p,p1-p);

		p=strstr(audiosdp,"ssrc:");


		if(p&&session->context.audioTrack.ssrc_==0){

			ind=yang_cstr_userfindindex(p,' ');
			if(ind<20) {
				memset(sid,0,sizeof(sid));
				memcpy(sid,p+5,ind-5);
				session->context.audioTrack.ssrc_=atoi(sid);

			}
		}
		p=strstr(p1,"ssrc:");

		if(p&&session->context.videoTrack.ssrc_==0){
			ind=yang_cstr_userfindindex(p,' ');
			if(ind<20) {
				memset(sid,0,sizeof(sid));
				memcpy(sid,p+5,ind-5);
				session->context.videoTrack.ssrc_=atoi(sid);

			}
		}
		yang_trace("\nremote audiossrc==%d,remote videossrc==%d",session->context.audioTrack.ssrc_,session->context.videoTrack.ssrc_);

	}

	p =strstr(sdp,	"http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01");
	if (p) {
		ind = yang_cstr_userfindupindex(p, '\n', 20);
		p1 = p - ind;
		p = strstr(p1, "a=extmap:");
		if (p) {
			ind = yang_cstr_userfindindex(p, ' ');
			if (ind < 20) {
				memset(sid, 0, sizeof(sid));
				memcpy(sid, p + strlen("a=extmap:"), ind - strlen("a=extmap:"));
				session->context.twcc_id = atoi(sid);
			}
		}

	}


	if(session->context.avcontext&&session->context.avcontext->setPlayMediaConfig){
		session->context.avcontext->setPlayMediaConfig(session->remote_audio,session->remote_video,session->context.user);
	}


	return err;
}
