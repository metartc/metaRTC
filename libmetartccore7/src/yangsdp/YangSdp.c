//
// Copyright (c) 2019-2022 yanggaofeng
//

#include "YangSdp.h"
#include "YangRtcSdp.h"
#include "YangMediaDesc.h"

#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangSocket.h>
#include <yangutil/sys/YangCString.h>

void yang_sdp_genLocalSdp_payloadType(YangMediaPayloadType *videotype){
	videotype->clock_rate = 90000;
	yang_strcpy(videotype->format_specific_param,"level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42e01f");
	yang_create_stringVector(&videotype->rtcp_fb);
	yang_insert_stringVector(&videotype->rtcp_fb, "transport-cc");
	yang_insert_stringVector(&videotype->rtcp_fb, "nack");
	yang_insert_stringVector(&videotype->rtcp_fb, "nack pli");
}

void  yang_sdp_genLocalSdp_candidate(YangRtcSession *session,YangMediaDesc *media_desc,char* localIp,int32_t localport){
    //candidate   
    yang_insert_YangCandidateVector(&media_desc->candidates, NULL);
    int32_t index=media_desc->candidates.vsize-1;
    yang_strcpy(media_desc->candidates.payload[index].ip,localIp);
    media_desc->candidates.payload[index].port = localport;

    media_desc->candidates.payload[index].socketProtocol=session->context.avinfo->rtc.rtcSocketProtocol;

    if(session->ice.session.candidateType>YangIceHost){
    	yang_strcpy(media_desc->candidates.payload[index].type, session->ice.session.candidateType==YangIceTurn?"relay":"srflx");
    }else{
    	yang_strcpy(media_desc->candidates.payload[index].type, "host");
    }

}


void yang_sdp_init_direction(YangMediaDesc* media_desc,YangRtcDirection role){
	if (role == YangRecvonly) {
		media_desc->recvonly = yangtrue;


	} else if (role == YangSendonly) {
		media_desc->sendonly = yangtrue;

	} else if (role == YangSendrecv) {
		media_desc->sendrecv = yangtrue;

	}
}

#define Yang_SDP_BUFFERLEN 1024*12
int32_t yang_sdp_genLocalSdp2(YangRtcSession *session, int32_t localport,char *dst, YangRtcDirection role) {
	int32_t mediaServer=session->context.avinfo->sys.mediaServer;
	//int32_t redPayloadtype=1;
	int32_t midNum=0;
	char *src = (char*) yang_calloc(1, Yang_SDP_BUFFERLEN);

	char randstr[128];


	YangSdp *local_sdp = (YangSdp*) yang_calloc(sizeof(YangSdp), 1);
	yang_create_rtcsdp(local_sdp);
	yang_strcpy(local_sdp->version, "0");

	yang_strcpy(local_sdp->username, "MetaRtc");
	yang_memset(randstr, 0, sizeof(randstr));
	yang_snprintf(randstr, 22, "%" PRId64, (int64_t) &local_sdp);
	yang_strcpy(local_sdp->session_id, randstr);
	yang_strcpy(local_sdp->session_version, "2");
	yang_strcpy(local_sdp->nettype, "IN");
	yang_strcpy(local_sdp->addrtype, "IP4");
	yang_strcpy(local_sdp->unicast_address, "0.0.0.0");

	yang_strcpy(local_sdp->session_name, "MetaRtcSession");
	yang_strcpy(local_sdp->msid_semantic, "WMS");
	yang_create_stringVector(&local_sdp->msids);
	char streamnames[164];
	yang_memset(streamnames, 0, sizeof(streamnames));
	if(yang_strlen(session->context.streamConfig->stream)==0)
		yang_sprintf(streamnames, "metaRTCLiveStream");
	else
		yang_sprintf(streamnames, "%s/%s", session->context.streamConfig->app,session->context.streamConfig->stream);

	yang_insert_stringVector(&local_sdp->msids, streamnames);

	char tmps[5][16];
	yang_create_YangMediaDescVector(&local_sdp->media_descs);

#if Yang_Enable_RTC_Audio
	yang_insert_YangMediaDescVector(&local_sdp->media_descs, NULL);
	YangMediaDesc *audio_media_desc = &local_sdp->media_descs.payload[local_sdp->media_descs.vsize-1];

	yang_strcpy(audio_media_desc->type, "audio");
	audio_media_desc->port = 9;
	yang_strcpy(audio_media_desc->protos, "UDP/TLS/RTP/SAVPF");
	audio_media_desc->rtcp_mux = yangtrue;
	audio_media_desc->rtcp_rsize = yangtrue;
	yang_sprintf(audio_media_desc->mid,"%d",midNum++);

#endif
#if Yang_Enable_RTC_Video
	yang_insert_YangMediaDescVector(&local_sdp->media_descs, NULL);
	YangMediaDesc *video_media_desc = &local_sdp->media_descs.payload[local_sdp->media_descs.vsize-1];
	yang_strcpy(video_media_desc->type, "video");
	video_media_desc->port = 9;
	yang_strcpy(video_media_desc->protos, "UDP/TLS/RTP/SAVPF");
	video_media_desc->rtcp_mux = yangtrue;
	video_media_desc->rtcp_rsize = yangtrue;
	yang_sprintf(video_media_desc->mid,"%d",midNum++);

#endif
#if Yang_Enable_Datachannel
	YangMediaDesc *data_media_desc = NULL;
	if(session->enableDatachannel){
		yang_insert_YangMediaDescVector(&local_sdp->media_descs, NULL);
		data_media_desc=&local_sdp->media_descs.payload[local_sdp->media_descs.vsize-1];
		yang_strcpy(data_media_desc->type, "application");
		data_media_desc->port = 9;
		yang_strcpy(data_media_desc->protos, "UDP/DTLS/SCTP webrtc-datachannel");
		data_media_desc->rtcp_mux = yangfalse;
		data_media_desc->rtcp_rsize = yangfalse;
		yang_sprintf(data_media_desc->mid,"%d",midNum++);

	}
#endif

	yang_sprintf(local_sdp->group_policy,"%s","BUNDLE");
	char* group_policy=NULL;
	for(int32_t a=0;a<midNum;a++){
		group_policy=local_sdp->group_policy+yang_strlen(local_sdp->group_policy);
		yang_sprintf(group_policy," %d",a);
	}

	//sendrecv
#if Yang_Enable_RTC_Audio
	yang_sdp_init_direction(audio_media_desc,role);
#endif
#if Yang_Enable_RTC_Video
	yang_sdp_init_direction(video_media_desc,role);
#endif

#if Yang_Enable_Datachannel
	if(data_media_desc) data_media_desc->sctp_port = yangtrue;
#endif
	//session info
	yang_memset(randstr, 0, sizeof(randstr));
	yang_cstr_random(4, randstr);
	yang_memset(session->local_ufrag,0,sizeof(session->local_ufrag));
	yang_strcpy(session->local_ufrag, randstr);
#if Yang_Enable_RTC_Audio
	yang_strcpy(audio_media_desc->session_info.ice_ufrag, randstr);
#endif
#if Yang_Enable_RTC_Video
	yang_strcpy(video_media_desc->session_info.ice_ufrag, randstr);
#endif
#if Yang_Enable_Datachannel
	if(data_media_desc) yang_strcpy(data_media_desc->session_info.ice_ufrag, randstr);
#endif

	yang_memset(randstr, 0, sizeof(randstr));
	yang_cstr_random(32, randstr);
#if Yang_Enable_RTC_Audio
	yang_strcpy(audio_media_desc->session_info.ice_pwd, randstr);
#endif
#if Yang_Enable_RTC_Video
	yang_strcpy(video_media_desc->session_info.ice_pwd, randstr);
#endif
#if Yang_Enable_Datachannel
	if(data_media_desc) yang_strcpy(data_media_desc->session_info.ice_pwd, randstr);
#endif
	yang_memset(session->localIcePwd,0,sizeof(session->localIcePwd));
	yang_strcpy(session->localIcePwd,randstr);
#if Yang_Enable_RTC_Audio
	yang_strcpy(audio_media_desc->session_info.fingerprint_algo, "sha-256");
#endif
#if Yang_Enable_RTC_Video
	yang_strcpy(video_media_desc->session_info.fingerprint_algo, "sha-256");
#endif
#if Yang_Enable_Datachannel
	if(data_media_desc) yang_strcpy(data_media_desc->session_info.fingerprint_algo, "sha-256");
	if(data_media_desc) yang_strcpy(data_media_desc->session_info.ice_options, "trickle");
#endif
#if Yang_Enable_Dtls
	#if Yang_Enable_RTC_Audio
	yang_strcpy(audio_media_desc->session_info.fingerprint,session->context.cer->fingerprint);
	#endif
	#if Yang_Enable_RTC_Video
	yang_strcpy(video_media_desc->session_info.fingerprint,session->context.cer->fingerprint);
	#endif
	#if Yang_Enable_Datachannel
	if(data_media_desc) yang_strcpy(data_media_desc->session_info.fingerprint,session->context.cer->fingerprint);
	#endif
#else
	#if Yang_Enable_RTC_Audio
	yang_strcpy(audio_media_desc->session_info.fingerprint,"EF:7A:50:9C:05:8C:EF:84:4D:72:B2:74:30:BA:FD:82:76:D1:C3:FE:0C:A0:10:43:B8:6C:B2:ED:B3:F7:77:8B");
	#endif
	#if Yang_Enable_RTC_Video
	yang_strcpy(video_media_desc->session_info.fingerprint,"EF:7A:50:9C:05:8C:EF:84:4D:72:B2:74:30:BA:FD:82:76:D1:C3:FE:0C:A0:10:43:B8:6C:B2:ED:B3:F7:77:8B");
	#endif
	if(data_media_desc) yang_strcpy(data_media_desc->session_info.fingerprint,"EF:7A:50:9C:05:8C:EF:84:4D:72:B2:74:30:BA:FD:82:76:D1:C3:FE:0C:A0:10:43:B8:6C:B2:ED:B3:F7:77:8B");
#endif
	#if Yang_Enable_RTC_Audio
	yang_strcpy(audio_media_desc->session_info.setup, session->isControlled?"passive":"active");
	#endif
	#if Yang_Enable_RTC_Video
	yang_strcpy(video_media_desc->session_info.setup, session->isControlled?"passive":"active");
	#endif
#if Yang_Enable_Datachannel
	if(data_media_desc) yang_strcpy(data_media_desc->session_info.setup, session->isControlled?"passive":"active");
#endif
	//extmaps twcc

#if Yang_Enable_RTC_Audio
/*	yang_create_YangExtmapVector(&audio_media_desc->extmaps);
	yang_insert_YangExtmapVector(&audio_media_desc->extmaps, NULL);
	audio_media_desc->extmaps.payload[0].mapid = session->context.twccId;
	yang_strcpy(audio_media_desc->extmaps.payload[0].extmap,Yang_SDP_kTWCCExt);*/
#endif

#if Yang_Enable_RTC_Video
	yang_create_YangExtmapVector(&video_media_desc->extmaps);
	yang_insert_YangExtmapVector(&video_media_desc->extmaps, NULL);
	video_media_desc->extmaps.payload[0].mapid = session->context.twccId;
	yang_strcpy(video_media_desc->extmaps.payload[0].extmap,Yang_SDP_kTWCCExt);
#endif

#if Yang_Enable_RTC_Audio
	//media pay
	yang_create_YangMediaPayloadTypeVector(&audio_media_desc->payload_types);
	yang_insert_YangMediaPayloadTypeVector(&audio_media_desc->payload_types,
			NULL);
	YangMediaPayloadType *audiotype =
			&audio_media_desc->payload_types.payload[0];
	audiotype->payload_type = session->audioPayloadType;
	yang_strcpy(audiotype->encoding_name, "opus");
	audiotype->clock_rate = session->context.avinfo->audio.sample; //48000;
	yang_itoa2(session->context.avinfo->audio.channel,audiotype->encoding_param, 10);


	yang_strcpy(audiotype->format_specific_param,
				session->context.avinfo->audio.enableAudioFec?"minptime=10;useinbandfec=1":"minptime=10");
	//yang_create_stringVector(&audiotype->rtcp_fb);
	//yang_insert_stringVector(&audiotype->rtcp_fb, "transport-cc");
#endif

#if Yang_Enable_RTC_Video
	yang_create_YangMediaPayloadTypeVector(&video_media_desc->payload_types);
	//insert h264/h265
	yang_insert_YangMediaPayloadTypeVector(&video_media_desc->payload_types,NULL);
	YangMediaPayloadType *videotype =&video_media_desc->payload_types.payload[0];
	if(mediaServer==Yang_Server_Zlm&&role==YangRecvonly){
		videotype->payload_type = session->h264PayloadType;
		yang_strcpy(videotype->encoding_name, "H264");
		yang_sdp_genLocalSdp_payloadType(videotype);
		yang_insert_YangMediaPayloadTypeVector(&video_media_desc->payload_types,NULL);
		videotype =&video_media_desc->payload_types.payload[1];
		videotype->payload_type = session->h265PayloadType;
		yang_strcpy(videotype->encoding_name, "H265");
		yang_sdp_genLocalSdp_payloadType(videotype);
		//redPayloadtype=2;
	}else{
		if (session->context.avinfo->video.videoEncoderType	== Yang_VED_H264) {
			videotype->payload_type = session->h264PayloadType;
			yang_strcpy(videotype->encoding_name, "H264");
		} else if (session->context.avinfo->video.videoEncoderType == Yang_VED_H265) {
			videotype->payload_type = session->h265PayloadType;
			yang_strcpy(videotype->encoding_name, "H265");
		} else if(session->context.avinfo->video.videoEncoderType	== Yang_VED_AV1){
			videotype->payload_type = YangAV1PayloadType;
			yang_strcpy(videotype->encoding_name, Yang_AV1_Name);
		}else if(session->context.avinfo->video.videoEncoderType	== Yang_VED_MJPEG){
			videotype->payload_type = YangMjpegPayloadType;
			yang_strcpy(videotype->encoding_name, "MJPEG");
		}


		yang_sdp_genLocalSdp_payloadType(videotype);
	}

	//insert red
	//yang_insert_YangMediaPayloadTypeVector(&video_media_desc->payload_types,NULL);
	//videotype =&video_media_desc->payload_types.payload[redPayloadtype];
	//yang_strcpy(videotype->encoding_name, "red");
	//videotype->payload_type = 114;
	//videotype->clock_rate = 90000;
#endif
	//ssrc info

#if Yang_Enable_RTC_Audio
	yang_insert_YangSSRCInfoVector(&audio_media_desc->ssrc_infos, NULL);
	audio_media_desc->ssrc_infos.payload[0].ssrc = session->context.audioSsrc;

#endif

#if Yang_Enable_RTC_Video
	yang_insert_YangSSRCInfoVector(&video_media_desc->ssrc_infos, NULL);
	video_media_desc->ssrc_infos.payload[0].ssrc = session->context.videoSsrc;

#endif

	yang_memset(randstr, 0, sizeof(randstr));
	yang_cstr_random(16, randstr);
#if Yang_Enable_RTC_Audio
	yang_strcpy(audio_media_desc->ssrc_infos.payload[0].cname, "metaRTC");
	yang_strcpy(audio_media_desc->ssrc_infos.payload[0].mslabel, "metaRTC");
#endif
#if Yang_Enable_RTC_Video
	yang_strcpy(video_media_desc->ssrc_infos.payload[0].cname, "metaRTC");
	yang_strcpy(video_media_desc->ssrc_infos.payload[0].mslabel, "metaRTC");
#endif

#if Yang_Enable_RTC_Audio
	yang_memset(tmps, 0, 16 * 5);
	yang_cstr_random(8, tmps[0]);
	yang_cstr_random(4, tmps[1]);
	yang_cstr_random(4, tmps[2]);
	yang_cstr_random(4, tmps[3]);
	yang_cstr_random(12, tmps[4]);
	yang_memset(randstr, 0, sizeof(randstr));
	yang_sprintf(randstr, "%s-%s-%s-%s-%s", tmps[0], tmps[1], tmps[2], tmps[3],tmps[4]);

	yang_strcpy(audio_media_desc->ssrc_infos.payload[0].msid, "metaRTC");
	yang_strcpy(audio_media_desc->ssrc_infos.payload[0].msid_tracker, randstr);
	yang_strcpy(audio_media_desc->ssrc_infos.payload[0].label, randstr);
#endif

#if Yang_Enable_RTC_Video
	yang_memset(tmps, 0, 16 * 5);
	yang_cstr_random(8, tmps[0]);
	yang_cstr_random(4, tmps[1]);
	yang_cstr_random(4, tmps[2]);
	yang_cstr_random(4, tmps[3]);
	yang_cstr_random(12, tmps[4]);
	yang_memset(randstr, 0, sizeof(randstr));
	yang_sprintf(randstr, "%s-%s-%s-%s-%s", tmps[0], tmps[1], tmps[2], tmps[3],
			tmps[4]);
    yang_strcpy(video_media_desc->ssrc_infos.payload[0].msid, "metaRTC");
    yang_strcpy(video_media_desc->ssrc_infos.payload[0].msid_tracker, randstr);
    yang_strcpy(video_media_desc->ssrc_infos.payload[0].label, randstr);
#endif
   if(mediaServer==Yang_Server_P2p){
#if Yang_Enable_RTC_Audio
	    yang_create_YangCandidateVector(&audio_media_desc->candidates);
#endif
#if Yang_Enable_RTC_Video
	    yang_create_YangCandidateVector(&video_media_desc->candidates);
#endif
	   	int32_t localport=session->context.streamConfig->localPort;
	   	char* localip=session->context.avinfo->rtc.localIp;
	   	int32_t hasIplist=0;

	   	if(session->context.avinfo->rtc.iceUsingLocalIp){
	   		localip=session->context.avinfo->rtc.iceLocalIP;
	   	}else if(session->ice.session.candidateType==YangIceStun&&session->ice.session.iceState==YangIceSuccess){
	   		struct sockaddr_in addr;
	   		addr.sin_addr.s_addr= session->ice.session.server.stunIp;
	   		localip=yang_inet_ntoa(addr.sin_addr);
	   		localport=session->ice.session.server.stunPort;
	   	}else if(session->ice.session.candidateType==YangIceTurn&&(session->ice.session.isTurnReady||session->ice.session.isTurnAllocated)){
	   		struct sockaddr_in addr;
	   		addr.sin_addr.s_addr= session->ice.session.server.relayIp;	   	
            localip=yang_inet_ntoa(addr.sin_addr);
	   		localport=session->ice.session.server.relayPort;
	   	}else{
	   		hasIplist=1;
	   	}

	   	if(hasIplist){
			YangStringVector iplists;
			yang_create_stringVector(&iplists);
			yang_getLocalInfoList(session->context.avinfo->sys.familyType,&iplists);
			int32_t j=0;
#if Yang_Enable_RTC_Audio
			for(j=0;j<iplists.vsize;j++)  yang_sdp_genLocalSdp_candidate(session,audio_media_desc,iplists.payload[j],localport);
#endif
#if Yang_Enable_RTC_Video
			for(j=0;j<iplists.vsize;j++)  yang_sdp_genLocalSdp_candidate(session,video_media_desc,iplists.payload[j],localport);
#endif
#if Yang_Enable_Datachannel
			if(data_media_desc){
				 yang_create_YangCandidateVector(&data_media_desc->candidates);
				for(j=0;j<iplists.vsize;j++)  yang_sdp_genLocalSdp_candidate(session,data_media_desc,iplists.payload[j],localport);
			}
#endif
			yang_destroy_stringVector(&iplists);
	   	}else if(localip&&yang_strlen(localip)>0){
#if Yang_Enable_RTC_Audio
			yang_sdp_genLocalSdp_candidate(session,audio_media_desc,localip,localport);
#endif
#if Yang_Enable_RTC_Video
			yang_sdp_genLocalSdp_candidate(session,video_media_desc,localip,localport);
#endif
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

	if(mediaServer==Yang_Server_Zlm||(mediaServer==Yang_Server_P2p&&!session->isControlled)){
		yang_cstr_replace(src, dst, (char*) "\r\n", (char*) "\n");
	}
	else{
		if(session->context.avinfo->rtc.enableHttpServerSdp)
			yang_cstr_replace(src, dst, (char*) "\r\n", (char*) "\\r\\n");
		else
			yang_strcpy(dst,src);
			//yang_cstr_replace(src, dst, (char*) "\r\n", (char*) "\n");
	}

	yang_destroy_rtcsdp(local_sdp);
	yang_free(local_sdp);
	yang_free(src);
	return Yang_Ok;

}

int32_t yang_sdp_genLocalSdp(YangRtcSession *session, int32_t localport,char **psdp, YangRtcDirection role){
	char *dst = (char*) yang_calloc(1, Yang_SDP_BUFFERLEN);
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

static yangbool yang_is_h2645PayoadType(char* str) {
	if (yang_strstr(str, "packetization-mode=1") && yang_strstr(str, "profile-level-id=42e01f")) return yangtrue;
	return yangfalse;
}

int32_t yang_sdp_parseRemoteSdp(YangRtcSession* session,YangSdp* sdp){
	int32_t k=0;
		for(int32_t i=0;i<sdp->media_descs.vsize;i++){
			YangMediaDesc* desc=&sdp->media_descs.payload[i];
			if(yang_strlen(session->remoteIcePwd)==0){
				yang_memset(session->remoteIcePwd,0,sizeof(session->remoteIcePwd));
				yang_strcpy(session->remoteIcePwd,desc->session_info.ice_pwd);
			}

			if(yang_strlen(session->remote_ufrag)==0){
				yang_memset(session->remote_ufrag,0,sizeof(session->remote_ufrag));
				yang_strcpy(session->remote_ufrag,desc->session_info.ice_ufrag);
			}

			if(yang_yang_strcmp(desc->type,"audio")==0){
#if Yang_Enable_RTC_Audio
				if(session->context.avinfo->sys.mediaServer==Yang_Server_P2p){
					if(desc->sendonly){
						session->context.streamConfig->direction=YangRecvonly;
					}else if(desc->recvonly){
						session->context.streamConfig->direction=YangSendonly;
					}else if(desc->sendrecv){
						session->context.streamConfig->direction=YangSendrecv;
					}
				}
				if(desc->ssrc_infos.vsize>0) session->context.audioTrack.ssrc=desc->ssrc_infos.payload[0].ssrc;

				for(k=0;k<desc->payload_types.vsize;k++){

					if(yang_yang_strcmp(desc->payload_types.payload[k].encoding_name,"opus")==0){
						session->remote_audio->encode=Yang_AED_OPUS;
	                    session->remote_audio->sample=desc->payload_types.payload[k].clock_rate;
						session->remote_audio->channel=atoi(desc->payload_types.payload[k].encoding_param);
	                    session->remote_audio->audioClock=desc->payload_types.payload[k].clock_rate;
	                    session->audioPayloadType=desc->payload_types.payload[k].payload_type;
					}
				}
#endif

			}else if(yang_yang_strcmp(desc->type,"video")==0){
#if Yang_Enable_RTC_Video
				if(session->context.avinfo->sys.mediaServer==Yang_Server_P2p){
					if(desc->sendonly){
						session->context.streamConfig->direction=YangRecvonly;
					}else if(desc->recvonly){
						session->context.streamConfig->direction=YangSendonly;
					}else if(desc->sendrecv){
						session->context.streamConfig->direction=YangSendrecv;
					}
				}
				if(desc->ssrc_infos.vsize>0) {
					for(k=0;k<desc->ssrc_infos.vsize;k++){
						YangRtcTrack videoTrack;
						yang_memset(&videoTrack,0,sizeof(YangRtcTrack));
						videoTrack.ssrc=desc->ssrc_infos.payload[k].ssrc;
						//msid=desc->ssrc_infos.payload[k].msid
						//id=desc->ssrc_infos.payload[k].msid_tracker
						yang_insert_YangRtcTrackVector(&session->context.videoTracks,&videoTrack);

					}
				}


				//int32_t hasRtcpfbTwcc=0;
				for(k=0;k<desc->payload_types.vsize;k++){
					YangMediaPayloadType* payload=&desc->payload_types.payload[k];
					//for(int j=0;j<payload->rtcp_fb.vsize;j++){
						//if(yang_strcmp(payload->rtcp_fb.payload[j],"transport-cc")==0){
							//hasRtcpfbTwcc=1;
						//}
					//}
					if(yang_yang_strcmp(payload->encoding_name,"H264")==0){
						session->remote_video->encode=Yang_VED_H264;
						session->context.videoCodec=Yang_VED_H264;

						if(yang_is_h2645PayoadType(payload->format_specific_param)){
								session->h264PayloadType=payload->payload_type;
						}
					}else if(yang_yang_strcmp(payload->encoding_name,"H265")==0){
						session->context.videoCodec=Yang_VED_H265;
						session->remote_video->encode=Yang_VED_H265;

						if(yang_is_h2645PayoadType(payload->format_specific_param)){
								session->h265PayloadType=payload->payload_type;
						}
					}else if(yang_yang_strcmp(payload->encoding_name,Yang_AV1_Name)==0){
						session->context.videoCodec=Yang_VED_AV1;
						session->remote_video->encode=Yang_VED_AV1;
					}else if(yang_yang_strcmp(payload->encoding_name,"MJPEG")==0){
						session->context.videoCodec=Yang_VED_MJPEG;
						session->remote_video->encode=Yang_VED_MJPEG;
					}

					session->remote_video->videoClock=90000;
				}
				for (k = 0; k < desc->extmaps.vsize; k++) {
					if (yang_strstr(desc->extmaps.payload[k].extmap,Yang_SDP_kTWCCExt)) {
						session->context.twccId = desc->extmaps.payload[k].mapid;
					}
				}
#endif
			}

			// set track fec_ssrc and rtx_ssrc
			if (desc->ssrc_groups.vsize > 0) {
				for (k = 0; k < desc->ssrc_groups.vsize; k++) {
					YangSSRCGroup *ssrc_group = &desc->ssrc_groups.payload[k];
					YangRtcTrack *track_desc = yang_sdp_find_track(session,	ssrc_group->groups.payload[0]);
					if (track_desc) {
						if (yang_strcmp(ssrc_group->semantic, "FID") == 0) {
							track_desc->rtxSsrc = ssrc_group->groups.payload[1];
						} else if (yang_strcmp(ssrc_group->semantic, "FEC") == 0) {
							track_desc->fecSsrc = ssrc_group->groups.payload[1];
						}
					}
				}
			}

			if(desc->candidates.vsize>0){
				yangbool replaceIp=yangtrue;
				for (k = 0; k < desc->candidates.vsize; k++) {
					YangCandidate *candidate = &desc->candidates.payload[k];
					if(yang_memcmp(session->context.streamConfig->remoteIp,candidate->ip,yang_strlen(candidate->ip))==0){
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
								yang_memset(session->context.streamConfig->remoteIp,0,sizeof(session->context.streamConfig->remoteIp));
								yang_strcpy(session->context.streamConfig->remoteIp,candidate->ip);
								replaceIp=yangfalse;
							}
						}
						yang_trace("\ncandidate:ip==%s,type=%s,port=%d",candidate->ip,candidate->type,candidate->port);

					}
				}
			}
		}

		//if(session->context.videoTracks.vsize){
		//	yang_trace("\nremote audiossrc==%d,remote videossrc==%d,audio sample==%d,audio channel==%d",session->context.audioTrack.ssrc,
			//	session->context.videoTracks.payload[0].ssrc,session->remote_audio->sample,session->remote_audio->channel);
		//}

		if(session->context.streamConfig&&session->context.streamConfig->rtcCallback.setMediaConfig){
			YangAudioParam* remoteAudio=NULL;
			YangVideoParam* remoteVideo=NULL;
#if Yang_Enable_RTC_Audio
			remoteAudio=session->remote_audio;
#endif

#if Yang_Enable_RTC_Video
			remoteVideo=session->remote_video;
#endif
			session->context.streamConfig->rtcCallback.setMediaConfig(session->context.streamConfig->rtcCallback.context,session->context.streamConfig->uid,
					remoteAudio,
					remoteVideo);
		}
		return Yang_Ok;
}
