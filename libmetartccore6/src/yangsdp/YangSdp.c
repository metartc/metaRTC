//
// Copyright (c) 2019-2022 yanggaofeng
//

#include "YangSdp.h"
#include "YangRtcSdp.h"
#include "YangMediaDesc.h"
#include <yangutil/yangtype.h>
#include <yangutil/sys/YangCString.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangSocket.h>
#include <string.h>

void yang_sdp_genLocalSdp_payloadType(YangMediaPayloadType *videotype){
	videotype->clock_rate = 90000;
	strcpy(videotype->format_specific_param,"level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42e01f");
	yang_create_stringVector(&videotype->rtcp_fb);
	yang_insert_stringVector(&videotype->rtcp_fb, "transport-cc");
	yang_insert_stringVector(&videotype->rtcp_fb, "nack");
	yang_insert_stringVector(&videotype->rtcp_fb, "nack pli");
}

void  yang_sdp_genLocalSdp_candidate(YangRtcSession *session,YangMediaDesc *media_desc,char* localIp,int32_t localport){
    //candidate   
    yang_insert_YangCandidateVector(&media_desc->candidates, NULL);
    int32_t index=media_desc->candidates.vsize-1;
    strcpy(media_desc->candidates.payload[index].ip,localIp);
    media_desc->candidates.payload[index].port = localport;

    if(session->ice.session.candidateType>YangIceHost){
    	strcpy(media_desc->candidates.payload[index].type, session->ice.session.candidateType==YangIceTurn?"relay":"srflx");
    }else{
    	strcpy(media_desc->candidates.payload[index].type, "host");
    }

}

#define Yang_SDP_BUFFERLEN 1024*12
int32_t yang_sdp_genLocalSdp2(YangRtcSession *session, int32_t localport,char *dst, YangStreamOptType role) {
	int32_t mediaServer=session->context.avinfo->sys.mediaServer;
	int32_t redPayloadtype=1;
	char *src = (char*) calloc(1, Yang_SDP_BUFFERLEN);

	char randstr[128];


	YangSdp *local_sdp = (YangSdp*) calloc(sizeof(YangSdp), 1);
	yang_create_rtcsdp(local_sdp);
	strcpy(local_sdp->version, "0");

	strcpy(local_sdp->username, "MetaRtc");
	memset(randstr, 0, sizeof(randstr));
	snprintf(randstr, 22, "%" PRId64, (int64_t) &local_sdp);
	strcpy(local_sdp->session_id, randstr);
	strcpy(local_sdp->session_version, "2");
	strcpy(local_sdp->nettype, "IN");
	strcpy(local_sdp->addrtype, "IP4");
	strcpy(local_sdp->unicast_address, "0.0.0.0");

	strcpy(local_sdp->session_name, "MetaRtcSession");
	strcpy(local_sdp->msid_semantic, "WMS");
	yang_create_stringVector(&local_sdp->msids);
	char streamnames[164];
	memset(streamnames, 0, sizeof(streamnames));
	sprintf(streamnames, "%s/%s", session->context.streamConfig->app,session->context.streamConfig->stream);
	yang_insert_stringVector(&local_sdp->msids, streamnames);

	strcpy(local_sdp->group_policy,"BUNDLE 0 1");

	char tmps[5][16];

	yang_create_YangMediaDescVector(&local_sdp->media_descs);
	yang_insert_YangMediaDescVector(&local_sdp->media_descs, NULL);
	yang_insert_YangMediaDescVector(&local_sdp->media_descs, NULL);

	YangMediaDesc *audio_media_desc = &local_sdp->media_descs.payload[0];

	strcpy(audio_media_desc->type, "audio");
	audio_media_desc->port = 9;
	strcpy(audio_media_desc->protos, "UDP/TLS/RTP/SAVPF");
	audio_media_desc->rtcp_mux = true;
	audio_media_desc->rtcp_rsize = true;
	strcpy(audio_media_desc->mid, "0");

	YangMediaDesc *video_media_desc = &local_sdp->media_descs.payload[1];
	strcpy(video_media_desc->type, "video");
	video_media_desc->port = 9;
	strcpy(video_media_desc->protos, "UDP/TLS/RTP/SAVPF");
	video_media_desc->rtcp_mux = true;
	video_media_desc->rtcp_rsize = true;
	strcpy(video_media_desc->mid, "1");
#if Yang_Enable_Datachannel
	YangMediaDesc *data_media_desc = NULL;
	if(session->context.avinfo->rtc.enableDatachannel){
		yang_insert_YangMediaDescVector(&local_sdp->media_descs, NULL);
		data_media_desc=&local_sdp->media_descs.payload[2];
		strcpy(data_media_desc->type, "application");
		data_media_desc->port = 9;
		strcpy(data_media_desc->protos, "UDP/DTLS/SCTP webrtc-datachannel");
		data_media_desc->rtcp_mux = false;
		data_media_desc->rtcp_rsize = false;
		strcpy(data_media_desc->mid, "2");
		strcpy(local_sdp->group_policy,"BUNDLE 0 1 2");
	}
#endif

	//sendrecv
	if (role == Yang_Stream_Play) {
		audio_media_desc->recvonly = true;
		video_media_desc->recvonly = true;

	} else if (role == Yang_Stream_Publish) {
		audio_media_desc->sendonly = true;
		video_media_desc->sendonly = true;
	} else if (role == Yang_Stream_Both) {
		audio_media_desc->sendrecv = true;
		video_media_desc->sendrecv = true;
	}
#if Yang_Enable_Datachannel
	//if(data_media_desc) data_media_desc->sendrecv = true;
	if(data_media_desc) data_media_desc->sctp_port = true;
#endif
	//session info
	memset(randstr, 0, sizeof(randstr));
	yang_cstr_random(4, randstr);
	memset(session->local_ufrag,0,sizeof(session->local_ufrag));
	strcpy(session->local_ufrag, randstr);
	strcpy(audio_media_desc->session_info.ice_ufrag, randstr);
	strcpy(video_media_desc->session_info.ice_ufrag, randstr);
#if Yang_Enable_Datachannel
	if(data_media_desc) strcpy(data_media_desc->session_info.ice_ufrag, randstr);
#endif

	memset(randstr, 0, sizeof(randstr));
	yang_cstr_random(32, randstr);
	strcpy(audio_media_desc->session_info.ice_pwd, randstr);
	strcpy(video_media_desc->session_info.ice_pwd, randstr);
#if Yang_Enable_Datachannel
	if(data_media_desc) strcpy(data_media_desc->session_info.ice_pwd, randstr);
#endif
	memset(session->localIcePwd,0,sizeof(session->localIcePwd));
	strcpy(session->localIcePwd,randstr);

	strcpy(audio_media_desc->session_info.fingerprint_algo, "sha-256");
	strcpy(video_media_desc->session_info.fingerprint_algo, "sha-256");
#if Yang_Enable_Datachannel
	if(data_media_desc) strcpy(data_media_desc->session_info.fingerprint_algo, "sha-256");
	if(data_media_desc) strcpy(data_media_desc->session_info.ice_options, "trickle");
#endif
#if Yang_Enable_Dtls
	strcpy(audio_media_desc->session_info.fingerprint,session->context.cer->fingerprint);
	strcpy(video_media_desc->session_info.fingerprint,session->context.cer->fingerprint);
#if Yang_Enable_Datachannel
	if(data_media_desc) strcpy(data_media_desc->session_info.fingerprint,session->context.cer->fingerprint);
#endif
#else
	strcpy(audio_media_desc->session_info.fingerprint,"EF:7A:50:9C:05:8C:EF:84:4D:72:B2:74:30:BA:FD:82:76:D1:C3:FE:0C:A0:10:43:B8:6C:B2:ED:B3:F7:77:8B");
	strcpy(video_media_desc->session_info.fingerprint,"EF:7A:50:9C:05:8C:EF:84:4D:72:B2:74:30:BA:FD:82:76:D1:C3:FE:0C:A0:10:43:B8:6C:B2:ED:B3:F7:77:8B");
	if(data_media_desc) strcpy(data_media_desc->session_info.fingerprint,"EF:7A:50:9C:05:8C:EF:84:4D:72:B2:74:30:BA:FD:82:76:D1:C3:FE:0C:A0:10:43:B8:6C:B2:ED:B3:F7:77:8B");
#endif
	strcpy(audio_media_desc->session_info.setup, session->isServer?"passive":"actpass");
	strcpy(video_media_desc->session_info.setup, session->isServer?"passive":"actpass");
#if Yang_Enable_Datachannel
	if(data_media_desc) strcpy(data_media_desc->session_info.setup, session->isServer?"passive":"actpass");
#endif
	//extmaps twcc
	yang_create_YangExtmapVector(&audio_media_desc->extmaps);
	yang_insert_YangExtmapVector(&audio_media_desc->extmaps, NULL);
	audio_media_desc->extmaps.payload[0].mapid = session->sendTwccId;
	strcpy(audio_media_desc->extmaps.payload[0].extmap,
			"http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01");

	yang_create_YangExtmapVector(&video_media_desc->extmaps);
	yang_insert_YangExtmapVector(&video_media_desc->extmaps, NULL);
	video_media_desc->extmaps.payload[0].mapid = session->sendTwccId;
	strcpy(video_media_desc->extmaps.payload[0].extmap,
			"http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01");

	//media pay
	yang_create_YangMediaPayloadTypeVector(&audio_media_desc->payload_types);
	yang_insert_YangMediaPayloadTypeVector(&audio_media_desc->payload_types,
			NULL);
	YangMediaPayloadType *audiotype =
			&audio_media_desc->payload_types.payload[0];
	audiotype->payload_type = 111;
	strcpy(audiotype->encoding_name, "opus");
	audiotype->clock_rate = session->context.avinfo->audio.sample; //48000;
	yang_itoa(session->context.avinfo->audio.channel,
			audiotype->encoding_param, 10);

	strcpy(audiotype->format_specific_param, "minptime=10;useinbandfec=1");
	yang_create_stringVector(&audiotype->rtcp_fb);
	yang_insert_stringVector(&audiotype->rtcp_fb, "transport-cc");

	yang_create_YangMediaPayloadTypeVector(&video_media_desc->payload_types);
	//insert h264/h265
	yang_insert_YangMediaPayloadTypeVector(&video_media_desc->payload_types,NULL);
	YangMediaPayloadType *videotype =&video_media_desc->payload_types.payload[0];
	if(mediaServer==Yang_Server_Zlm&&role==Yang_Stream_Play){
		videotype->payload_type = 125;
		strcpy(videotype->encoding_name, "H264");
		yang_sdp_genLocalSdp_payloadType(videotype);
		yang_insert_YangMediaPayloadTypeVector(&video_media_desc->payload_types,NULL);
		videotype =&video_media_desc->payload_types.payload[1];
		videotype->payload_type = 126;
		strcpy(videotype->encoding_name, "H265");
		yang_sdp_genLocalSdp_payloadType(videotype);
		redPayloadtype=2;
	}else{
		if (session->context.avinfo->video.videoEncoderType	== Yang_VED_264) {
			videotype->payload_type = 125;
			strcpy(videotype->encoding_name, "H264");
		} else if (session->context.avinfo->video.videoEncoderType == Yang_VED_265) {
			videotype->payload_type = 126;
			strcpy(videotype->encoding_name, "H265");
		}
		yang_sdp_genLocalSdp_payloadType(videotype);
	}

	//insert red
	yang_insert_YangMediaPayloadTypeVector(&video_media_desc->payload_types,NULL);
	videotype =&video_media_desc->payload_types.payload[redPayloadtype];
	strcpy(videotype->encoding_name, "red");
	videotype->payload_type = 114;
	videotype->clock_rate = 90000;

	//ssrc info
	yang_insert_YangSSRCInfoVector(&audio_media_desc->ssrc_infos, NULL);
	yang_insert_YangSSRCInfoVector(&video_media_desc->ssrc_infos, NULL);
	audio_media_desc->ssrc_infos.payload[0].ssrc = session->context.audioSsrc;
	video_media_desc->ssrc_infos.payload[0].ssrc = session->context.videoSsrc;

	memset(randstr, 0, sizeof(randstr));
	yang_cstr_random(16, randstr);
	strcpy(audio_media_desc->ssrc_infos.payload[0].cname, randstr);
	strcpy(video_media_desc->ssrc_infos.payload[0].cname, randstr);
	strcpy(audio_media_desc->ssrc_infos.payload[0].mslabel, "-");
	strcpy(video_media_desc->ssrc_infos.payload[0].mslabel, "-");

	memset(tmps, 0, 16 * 5);
	yang_cstr_random(8, tmps[0]);
	yang_cstr_random(4, tmps[1]);
	yang_cstr_random(4, tmps[2]);
	yang_cstr_random(4, tmps[3]);
	yang_cstr_random(12, tmps[4]);
	memset(randstr, 0, sizeof(randstr));
	sprintf(randstr, "%s-%s-%s-%s-%s", tmps[0], tmps[1], tmps[2], tmps[3],tmps[4]);
	strcpy(audio_media_desc->ssrc_infos.payload[0].msid, "-");
	strcpy(audio_media_desc->ssrc_infos.payload[0].msid_tracker, randstr);
	strcpy(audio_media_desc->ssrc_infos.payload[0].label, randstr);

	memset(tmps, 0, 16 * 5);
	yang_cstr_random(8, tmps[0]);
	yang_cstr_random(4, tmps[1]);
	yang_cstr_random(4, tmps[2]);
	yang_cstr_random(4, tmps[3]);
	yang_cstr_random(12, tmps[4]);
	memset(randstr, 0, sizeof(randstr));
	sprintf(randstr, "%s-%s-%s-%s-%s", tmps[0], tmps[1], tmps[2], tmps[3],
			tmps[4]);
    strcpy(video_media_desc->ssrc_infos.payload[0].msid, "-");
    strcpy(video_media_desc->ssrc_infos.payload[0].msid_tracker, randstr);
    strcpy(video_media_desc->ssrc_infos.payload[0].label, randstr);
   if(mediaServer==Yang_Server_P2p){
	    yang_create_YangCandidateVector(&audio_media_desc->candidates);
	    yang_create_YangCandidateVector(&video_media_desc->candidates);
	   	int32_t localport=session->context.streamConfig->localPort;
	   	char* localip=session->context.avinfo->sys.localIp;
	   	int32_t hasIplist=0;

	   	if(session->context.avinfo->rtc.iceUsingLocalIp){
	   		localip=session->context.avinfo->rtc.iceLocalIP;
	   	}else if(session->ice.session.candidateType==YangIceStun&&session->ice.session.server.stunPort>0){
	   		struct sockaddr_in addr;
	   		addr.sin_addr.s_addr= session->ice.session.server.stunIp;
	   		localip=inet_ntoa(addr.sin_addr);
	   		localport=session->ice.session.server.stunPort;
	   	}else if(session->ice.session.candidateType==YangIceTurn&&(session->ice.session.isTurnReady||session->ice.session.isTurnAllocated)){
	   		struct sockaddr_in addr;
	   		addr.sin_addr.s_addr= session->ice.session.server.relayIp;	   	
            localip=inet_ntoa(addr.sin_addr);       
	   		localport=session->ice.session.server.relayPort;
	   	}else{
#ifdef _WIN32
	   		hasIplist=1;
#endif
	   	}

	   	if(hasIplist){
			YangStringVector iplists;
			yang_create_stringVector(&iplists);
			yang_getLocalInfoList(&iplists);
			int32_t j=0;
			for(j=0;j<iplists.vsize;j++)  yang_sdp_genLocalSdp_candidate(session,audio_media_desc,iplists.payload[j],localport);
			for(j=0;j<iplists.vsize;j++)  yang_sdp_genLocalSdp_candidate(session,video_media_desc,iplists.payload[j],localport);
#if Yang_Enable_Datachannel
			if(data_media_desc){
				 yang_create_YangCandidateVector(&data_media_desc->candidates);
				for(j=0;j<iplists.vsize;j++)  yang_sdp_genLocalSdp_candidate(session,data_media_desc,iplists.payload[j],localport);
			}
#endif
			yang_destroy_stringVector(&iplists);
	   	}else{
			yang_sdp_genLocalSdp_candidate(session,audio_media_desc,localip,localport);
			yang_sdp_genLocalSdp_candidate(session,video_media_desc,localip,localport);
#if Yang_Enable_Datachannel
			if(data_media_desc){
				yang_create_YangCandidateVector(&data_media_desc->candidates);
				yang_sdp_genLocalSdp_candidate(session,data_media_desc,localip,localport);
			}
#endif
	   	}


    }

	YangBuffer buf;
	yang_init_buffer(&buf, src, Yang_SDP_BUFFERLEN);
	yang_rtcsdp_encode(local_sdp, &buf);

	if(mediaServer==Yang_Server_Zlm||(mediaServer==Yang_Server_P2p&&!session->isServer)){
		yang_cstr_replace(src, dst, (char*) "\r\n", (char*) "\n");
	}
	else{
		yang_cstr_replace(src, dst, (char*) "\r\n", (char*) "\\r\\n");
	}

	yang_destroy_rtcsdp(local_sdp);
	yang_free(local_sdp);
	yang_free(src);
	return Yang_Ok;

}

int32_t yang_sdp_genLocalSdp(YangRtcSession *session, int32_t localport,char **psdp, YangStreamOptType role){
	char *dst = (char*) calloc(1, Yang_SDP_BUFFERLEN);
	*psdp=dst;
	return yang_sdp_genLocalSdp2(session,localport,dst,role);
}

YangRtcTrack* yang_sdp_find_track(YangRtcSession* session,uint32_t ssrc){
	if(session==NULL) return NULL;
	if(yang_has_ssrc(&session->context.audioTrack,ssrc)) return &session->context.audioTrack;
	for(int32_t i=0;i<session->context.videoTracks.vsize;i++){
		if(session->context.videoTracks.payload[i].ssrc==ssrc) return &session->context.videoTracks.payload[i];
	}
	return NULL;
}

int32_t yang_sdp_parseRemoteSdp(YangRtcSession* session,YangSdp* sdp){
	int32_t k=0;
		for(int32_t i=0;i<sdp->media_descs.vsize;i++){
			YangMediaDesc* desc=&sdp->media_descs.payload[i];
			if(strlen(session->remoteIcePwd)==0){
				memset(session->remoteIcePwd,0,sizeof(session->remoteIcePwd));
				strcpy(session->remoteIcePwd,desc->session_info.ice_pwd);
			}

			if(strlen(session->remote_ufrag)==0){
				memset(session->remote_ufrag,0,sizeof(session->remote_ufrag));
				strcpy(session->remote_ufrag,desc->session_info.ice_ufrag);
			}

			if(yang_strcmp(desc->type,"audio")==0){
				if(session->context.avinfo->sys.mediaServer==Yang_Server_P2p){
					if(desc->sendonly){
						session->context.streamConfig->streamOptType=Yang_Stream_Play;
					}else if(desc->recvonly){
						session->context.streamConfig->streamOptType=Yang_Stream_Publish;
					}else if(desc->sendrecv){
						session->context.streamConfig->streamOptType=Yang_Stream_Both;
					}
				}
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
				if(session->context.avinfo->sys.mediaServer==Yang_Server_P2p){
					if(desc->sendonly){
						session->context.streamConfig->streamOptType=Yang_Stream_Play;
					}else if(desc->recvonly){
						session->context.streamConfig->streamOptType=Yang_Stream_Publish;
					}else if(desc->sendrecv){
						session->context.streamConfig->streamOptType=Yang_Stream_Both;
					}
				}
				if(desc->ssrc_infos.vsize>0) {
					for(k=0;k<desc->ssrc_infos.vsize;k++){
						YangRtcTrack videoTrack;
						memset(&videoTrack,0,sizeof(YangRtcTrack));
						videoTrack.ssrc=desc->ssrc_infos.payload[k].ssrc;
						//msid=desc->ssrc_infos.payload[k].msid
						//id=desc->ssrc_infos.payload[k].msid_tracker
						yang_insert_YangRtcTrackVector(&session->context.videoTracks,&videoTrack);

					}
				}


				int32_t hasRtcpfbTwcc=0;
				for(k=0;k<desc->payload_types.vsize;k++){
					YangMediaPayloadType* payload=&desc->payload_types.payload[k];
					for(int j=0;j<payload->rtcp_fb.vsize;j++){
						if(strcmp(payload->rtcp_fb.payload[j],"transport-cc")==0){
							hasRtcpfbTwcc=1;
						}
					}
					if(yang_strcmp(payload->encoding_name,"H264")==0){
						session->remote_video->encode=Yang_VED_264;
						session->context.codec=Yang_VED_264;


					}else if(yang_strcmp(payload->encoding_name,"H265")==0){
						session->context.codec=Yang_VED_265;
						session->remote_video->encode=Yang_VED_265;
					}

					session->remote_video->videoClock=90000;
				}
				for (k = 0; k < desc->extmaps.vsize; k++) {
					if (strstr(desc->extmaps.payload[k].extmap,	"http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01")&&hasRtcpfbTwcc) {
						session->context.twccId = desc->extmaps.payload[k].mapid;
					}
				}
			}else if(yang_strcmp(desc->type,"application")==0) {
				session->usingDatachannel=1;
			}

			// set track fec_ssrc and rtx_ssrc
			if (desc->ssrc_groups.vsize > 0) {
				for (k = 0; k < desc->ssrc_groups.vsize; k++) {
					YangSSRCGroup *ssrc_group = &desc->ssrc_groups.payload[k];
					YangRtcTrack *track_desc = yang_sdp_find_track(session,	ssrc_group->groups.payload[0]);
					if (track_desc) {
						if (strcmp(ssrc_group->semantic, "FID") == 0) {
							track_desc->rtxSsrc = ssrc_group->groups.payload[1];
						} else if (strcmp(ssrc_group->semantic, "FEC") == 0) {
							track_desc->fecSsrc = ssrc_group->groups.payload[1];
						}
					}
				}
			}

			if(desc->candidates.vsize>0){
				yangbool replaceIp=yangtrue;
				for (k = 0; k < desc->candidates.vsize; k++) {
					YangCandidate *candidate = &desc->candidates.payload[k];
					if(memcmp(session->context.streamConfig->remoteIp,candidate->ip,strlen(session->context.streamConfig->remoteIp))==0){
						replaceIp=yangfalse;
						break;
					}
				}
				for (k = 0; k < desc->candidates.vsize; k++) {
					YangCandidate *candidate = &desc->candidates.payload[k];
					if(candidate){
						if(candidate->port>0) {
							session->context.streamConfig->remotePort=candidate->port;
							if(replaceIp){
								memset(session->context.streamConfig->remoteIp,0,sizeof(session->context.streamConfig->remoteIp));
								strcpy(session->context.streamConfig->remoteIp,candidate->ip);
								replaceIp=yangfalse;
							}
						}
						yang_trace("\ncandidate:ip==%s,type=%s,port=%d",candidate->ip,candidate->type,candidate->port);

					}
				}
			}
		}

		if(session->context.videoTracks.vsize){
			yang_trace("\nremote audiossrc==%d,remote videossrc==%d,audio sample==%d,audio channel==%d",session->context.audioTrack.ssrc,
				session->context.videoTracks.payload[0].ssrc,session->remote_audio->sample,session->remote_audio->channel);
		}

		if(session->context.streamConfig&&session->context.streamConfig->rtcCallback.setMediaConfig){
			session->context.streamConfig->rtcCallback.setMediaConfig(session->context.streamConfig->rtcCallback.context,session->context.streamConfig->uid,session->remote_audio,session->remote_video);
		}
		return Yang_Ok;
}
