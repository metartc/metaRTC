//
// Copyright (c) 2019-2026 yanggaofeng
//

#include "YangSdp.h"
#include "YangRtcSdp.h"
#include "YangMediaDesc.h"

#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangSocket.h>
#include <yangutil/sys/YangCString.h>

void yang_sdp_genLocalSdp_payloadType(YangVideoCodec codec,YangMediaPayloadType *videotype){
	videotype->clock_rate = 90000;
	if(codec==Yang_VED_H264){
		yang_strcpy(videotype->format_specific_param,"level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42e01f");
	}else if(codec==Yang_VED_H265){
		yang_strcpy(videotype->format_specific_param,"level-id=93;profile-id=1;tier-flag=0;tx-mode=SRST");
	}

	yang_create_stringVector(&videotype->rtcp_fb);
	yang_insert_stringVector(&videotype->rtcp_fb, "transport-cc");
	yang_insert_stringVector(&videotype->rtcp_fb, "nack");
	yang_insert_stringVector(&videotype->rtcp_fb, "nack pli");
}

void  yang_sdp_genLocalSdp_candidate(YangRtcSession *session,YangMediaDesc *media_desc,char* localIp,int32_t localport){
	//candidate
	yang_insert_YangCandidateVector(&media_desc->candidates, NULL);
	YangCandidate* candidate=&media_desc->candidates.payload[media_desc->candidates.vsize-1];
	candidate->socketProtocol=session->context.peerInfo->rtc.rtcSocketProtocol;
	yang_addr_set(&candidate->address,localIp,localport,session->context.peerInfo->familyType,candidate->socketProtocol);

	candidate->candidateType=session->ice.session.candidateType;
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

static void genAudioCodecPaylaod(YangRtcSession *session,YangMediaPayloadType *audiotype,YangAudioCodec audioCodec){

	if(audioCodec==Yang_AED_OPUS){
		yang_strcpy(audiotype->encoding_name, "opus");
		audiotype->clock_rate = 48000; //48000;
		yang_itoa2(2,audiotype->encoding_param, 10);

		yang_strcpy(audiotype->format_specific_param,"minptime=10");
		audiotype->payload_type=session->context.opusPayloadType;
	}else if(audioCodec==Yang_AED_PCMU){
		yang_strcpy(audiotype->encoding_name, "PCMU");
		audiotype->clock_rate = 8000;
		audiotype->payload_type=session->context.pcmuPayloadType;
	}else if(audioCodec==Yang_AED_PCMA){
		yang_strcpy(audiotype->encoding_name, "PCMA");
		audiotype->clock_rate = 8000;
		audiotype->payload_type=session->context.pcmaPayloadType;
	}
}

#define Yang_SDP_BUFFERLEN 1024*12
int32_t yang_sdp_genLocalSdp2(YangRtcSession *session, int32_t localport,char *dst, YangRtcDirection audioRole,YangRtcDirection videoRole,YangIceMode iceMode) {

	int32_t midNum=0;
	char *src = (char*) yang_calloc(1, Yang_SDP_BUFFERLEN);

	char randstr[128];


	YangSdp *local_sdp = (YangSdp*) yang_calloc(sizeof(YangSdp), 1);
	yang_create_rtcsdp(local_sdp);
	local_sdp->iceMode=iceMode;
	yang_strcpy(local_sdp->version, "0");

	yang_strcpy(local_sdp->username, "MetaRtc");
	yang_memset(randstr, 0, sizeof(randstr));
	yang_snprintf(randstr, 22, "%" PRId64, (int64_t) &local_sdp);
	yang_strcpy(local_sdp->session_id, randstr);
	yang_strcpy(local_sdp->session_version, "2");
	yang_strcpy(local_sdp->nettype, "IN");
	yang_strcpy(local_sdp->addrtype, session->context.peerInfo->familyType==Yang_IpFamilyType_IPV4?"IP4":"IP6");
	yang_strcpy(local_sdp->unicast_address, "0.0.0.0");

	yang_strcpy(local_sdp->session_name, "MetaRtcSession");
	yang_strcpy(local_sdp->msid_semantic, "WMS");
	yang_create_stringVector(&local_sdp->msids);
	char streamnames[164];
	yang_memset(streamnames, 0, sizeof(streamnames));


	yang_sprintf(streamnames, "metaRTCLiveStream");

	yang_insert_stringVector(&local_sdp->msids, streamnames);

	char tmps[5][16];
	yang_create_YangMediaDescVector(&local_sdp->media_descs);
	YangMediaDesc *audio_media_desc=NULL;
#if Yang_Enable_RTC_Audio
	if(session->context.enableAudioTrack){
		yang_insert_YangMediaDescVector(&local_sdp->media_descs, NULL);
		audio_media_desc = &local_sdp->media_descs.payload[local_sdp->media_descs.vsize-1];
		audio_media_desc->familyType=session->context.peerInfo->familyType;
		audio_media_desc->iceMode=local_sdp->iceMode;

		yang_strcpy(audio_media_desc->type, "audio");
		audio_media_desc->port = 9;
		yang_strcpy(audio_media_desc->protos, "UDP/TLS/RTP/SAVPF");
		audio_media_desc->rtcp_mux = yangtrue;
		audio_media_desc->rtcp_rsize = yangtrue;
		yang_sprintf(audio_media_desc->mid,"%d",midNum++);
	}
#endif

	YangMediaDesc *video_media_desc=NULL;
#if Yang_Enable_RTC_Video
	if(session->context.enableVideoTrack){
		yang_insert_YangMediaDescVector(&local_sdp->media_descs, NULL);
		video_media_desc = &local_sdp->media_descs.payload[local_sdp->media_descs.vsize-1];
		video_media_desc->familyType=session->context.peerInfo->familyType;
		video_media_desc->iceMode=local_sdp->iceMode;
		yang_strcpy(video_media_desc->type, "video");
		video_media_desc->port = 9;
		yang_strcpy(video_media_desc->protos, "UDP/TLS/RTP/SAVPF");
		video_media_desc->rtcp_mux = yangtrue;
		video_media_desc->rtcp_rsize = yangtrue;
		yang_sprintf(video_media_desc->mid,"%d",midNum++);
	}
#endif


#if Yang_Enable_Datachannel
	YangMediaDesc *data_media_desc = NULL;
	if(session->enableDatachannel){
		yang_insert_YangMediaDescVector(&local_sdp->media_descs, NULL);
		data_media_desc=&local_sdp->media_descs.payload[local_sdp->media_descs.vsize-1];
		data_media_desc->familyType=session->context.peerInfo->familyType;
		data_media_desc->iceMode=local_sdp->iceMode;
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
	if(audio_media_desc) yang_sdp_init_direction(audio_media_desc,audioRole);
#endif
#if Yang_Enable_RTC_Video
	if(video_media_desc) yang_sdp_init_direction(video_media_desc,videoRole);
#endif

#if Yang_Enable_Datachannel
	if(data_media_desc) data_media_desc->sctp_port = yangtrue;
#endif
	//session info
	yang_memset(randstr, 0, sizeof(randstr));
	yang_cstr_random(7, randstr);
	yang_memset(session->ice.session.local_ufrag,0,sizeof(session->ice.session.local_ufrag));
	yang_strcpy(session->ice.session.local_ufrag, randstr);
#if Yang_Enable_RTC_Audio
	if(audio_media_desc) yang_strcpy(audio_media_desc->session_info.ice_ufrag, randstr);
#endif
#if Yang_Enable_RTC_Video
	if(video_media_desc) yang_strcpy(video_media_desc->session_info.ice_ufrag, randstr);
#endif
#if Yang_Enable_Datachannel
	if(data_media_desc) yang_strcpy(data_media_desc->session_info.ice_ufrag, randstr);
#endif

	yang_memset(randstr, 0, sizeof(randstr));
	yang_cstr_random(32, randstr);
#if Yang_Enable_RTC_Audio
	if(audio_media_desc) yang_strcpy(audio_media_desc->session_info.ice_pwd, randstr);
#endif
#if Yang_Enable_RTC_Video
	if(video_media_desc) yang_strcpy(video_media_desc->session_info.ice_pwd, randstr);
#endif
#if Yang_Enable_Datachannel
	if(data_media_desc) yang_strcpy(data_media_desc->session_info.ice_pwd, randstr);
#endif
	yang_memset(session->ice.session.localIcePwd,0,sizeof(session->ice.session.localIcePwd));
	yang_strcpy(session->ice.session.localIcePwd,randstr);
#if Yang_Enable_RTC_Audio
	if(audio_media_desc) yang_strcpy(audio_media_desc->session_info.fingerprint_algo, "sha-256");
	if(audio_media_desc&&iceMode==YangIceModeFull) yang_strcpy(audio_media_desc->session_info.ice_options, "trickle");
#endif
#if Yang_Enable_RTC_Video
	if(video_media_desc) yang_strcpy(video_media_desc->session_info.fingerprint_algo, "sha-256");
	if(video_media_desc&&iceMode==YangIceModeFull) yang_strcpy(video_media_desc->session_info.ice_options, "trickle");
#endif
#if Yang_Enable_Datachannel
	if(data_media_desc) yang_strcpy(data_media_desc->session_info.fingerprint_algo, "sha-256");
	if(data_media_desc&&iceMode==YangIceModeFull) yang_strcpy(data_media_desc->session_info.ice_options, "trickle");
#endif
#if Yang_Enable_Dtls
#if Yang_Enable_RTC_Audio
	if(audio_media_desc) yang_strcpy(audio_media_desc->session_info.fingerprint,session->context.cert->fingerprint);
#endif
#if Yang_Enable_RTC_Video
	if(video_media_desc) yang_strcpy(video_media_desc->session_info.fingerprint,session->context.cert->fingerprint);
#endif
#if Yang_Enable_Datachannel
	if(data_media_desc) yang_strcpy(data_media_desc->session_info.fingerprint,session->context.cert->fingerprint);
#endif
#else
#if Yang_Enable_RTC_Audio
	if(audio_media_desc) yang_strcpy(audio_media_desc->session_info.fingerprint,"EF:7A:50:9C:05:8C:EF:84:4D:72:B2:74:30:BA:FD:82:76:D1:C3:FE:0C:A0:10:43:B8:6C:B2:ED:B3:F7:77:8B");
#endif
#if Yang_Enable_RTC_Video
	if(video_media_desc) yang_strcpy(video_media_desc->session_info.fingerprint,"EF:7A:50:9C:05:8C:EF:84:4D:72:B2:74:30:BA:FD:82:76:D1:C3:FE:0C:A0:10:43:B8:6C:B2:ED:B3:F7:77:8B");
#endif
	if(data_media_desc) yang_strcpy(data_media_desc->session_info.fingerprint,"EF:7A:50:9C:05:8C:EF:84:4D:72:B2:74:30:BA:FD:82:76:D1:C3:FE:0C:A0:10:43:B8:6C:B2:ED:B3:F7:77:8B");
#endif
#if Yang_Enable_RTC_Audio
	if(audio_media_desc) yang_strcpy(audio_media_desc->session_info.setup, session->isControlled?"passive":"active");
#endif
#if Yang_Enable_RTC_Video
	if(video_media_desc) yang_strcpy(video_media_desc->session_info.setup, session->isControlled?"passive":"active");
#endif
#if Yang_Enable_Datachannel
	if(data_media_desc) yang_strcpy(data_media_desc->session_info.setup, session->isControlled?"passive":"active");
#endif
	//extmaps twcc

#if Yang_Enable_RTC_Audio
	//yang_create_YangExtmapVector(&audio_media_desc->extmaps);
	//	yang_insert_YangExtmapVector(&audio_media_desc->extmaps, NULL);
	//audio_media_desc->extmaps.payload[0].mapid = session->context.twccId;
	//yang_strcpy(audio_media_desc->extmaps.payload[0].extmap,Yang_SDP_kTWCCExt);
#endif

#if Yang_Enable_RTC_Video
	if(video_media_desc) {
		yang_create_YangExtmapVector(&video_media_desc->extmaps);
		yang_insert_YangExtmapVector(&video_media_desc->extmaps, NULL);
		video_media_desc->extmaps.payload[0].mapid = session->context.twccId;
		yang_strcpy(video_media_desc->extmaps.payload[0].extmap,Yang_SDP_kTWCCExt);
	}
#endif

#if Yang_Enable_RTC_Audio
	if(audio_media_desc) {
		//media pay
		yang_create_YangMediaPayloadTypeVector(&audio_media_desc->payload_types);
		YangMediaPayloadType *audiotype=NULL;
		if(session->context.audioDirection==YangRecvonly){
			yang_insert_YangMediaPayloadTypeVector(&audio_media_desc->payload_types,NULL);
			audiotype = &audio_media_desc->payload_types.payload[0];
			genAudioCodecPaylaod(session,audiotype,Yang_AED_OPUS);
			yang_insert_YangMediaPayloadTypeVector(&audio_media_desc->payload_types,NULL);
			audiotype = &audio_media_desc->payload_types.payload[1];
			genAudioCodecPaylaod(session,audiotype,Yang_AED_PCMU);
			yang_insert_YangMediaPayloadTypeVector(&audio_media_desc->payload_types,NULL);
			audiotype = &audio_media_desc->payload_types.payload[2];
			genAudioCodecPaylaod(session,audiotype,Yang_AED_PCMA);
		}else{
			yang_insert_YangMediaPayloadTypeVector(&audio_media_desc->payload_types,NULL);
			audiotype = &audio_media_desc->payload_types.payload[0];
			audiotype->payload_type = session->context.audioPayloadType;
			genAudioCodecPaylaod(session,audiotype,session->context.audioEncodeCodec);
		}
	}
#endif

#if Yang_Enable_RTC_Video
	if(video_media_desc) {
		yang_create_YangMediaPayloadTypeVector(&video_media_desc->payload_types);
		//insert h264/h265
		yang_insert_YangMediaPayloadTypeVector(&video_media_desc->payload_types,NULL);
		YangMediaPayloadType *videotype =&video_media_desc->payload_types.payload[0];
		if(videoRole==YangRecvonly){
			videotype->payload_type = session->context.h264PayloadType;
			yang_strcpy(videotype->encoding_name, "H264");
			yang_sdp_genLocalSdp_payloadType(Yang_VED_H264,videotype);
			yang_insert_YangMediaPayloadTypeVector(&video_media_desc->payload_types,NULL);
			videotype =&video_media_desc->payload_types.payload[1];
			videotype->payload_type = session->context.h265PayloadType;
			yang_strcpy(videotype->encoding_name, "H265");
			yang_sdp_genLocalSdp_payloadType(Yang_VED_H264,videotype);
			//redPayloadtype=2;
		}else{
			if (session->context.peerInfo->pushVideo.videoEncoderType	== Yang_VED_H264) {
				videotype->payload_type = session->context.h264PayloadType;
				yang_strcpy(videotype->encoding_name, "H264");
			} else if (session->context.peerInfo->pushVideo.videoEncoderType == Yang_VED_H265) {
				videotype->payload_type = session->context.h265PayloadType;
				yang_strcpy(videotype->encoding_name, "H265");
			} else if(session->context.peerInfo->pushVideo.videoEncoderType	== Yang_VED_AV1){
				videotype->payload_type = YangAV1PayloadType;
				yang_strcpy(videotype->encoding_name, Yang_AV1_Name);
			}else if(session->context.peerInfo->pushVideo.videoEncoderType	== Yang_VED_MJPEG){
				videotype->payload_type = YangMjpegPayloadType;
				yang_strcpy(videotype->encoding_name, "MJPEG");
			}

			yang_sdp_genLocalSdp_payloadType(session->context.peerInfo->pushVideo.videoEncoderType,videotype);
		}

	}
#endif
	//ssrc info

#if Yang_Enable_RTC_Audio
	if(audio_media_desc) {
		yang_insert_YangSSRCInfoVector(&audio_media_desc->ssrc_infos, NULL);
		audio_media_desc->ssrc_infos.payload[0].ssrc = session->context.audioSsrc;
	}
#endif

#if Yang_Enable_RTC_Video
	if(video_media_desc) {
		yang_insert_YangSSRCInfoVector(&video_media_desc->ssrc_infos, NULL);
		video_media_desc->ssrc_infos.payload[0].ssrc = session->context.videoSsrc;
	}
#endif

	yang_memset(randstr, 0, sizeof(randstr));
	yang_cstr_random(16, randstr);
#if Yang_Enable_RTC_Audio
	if(audio_media_desc){
		yang_strcpy(audio_media_desc->ssrc_infos.payload[0].cname, "metaRTC");
		yang_strcpy(audio_media_desc->ssrc_infos.payload[0].mslabel, "metaRTC");
	}
#endif
#if Yang_Enable_RTC_Video
	if(video_media_desc) {
		yang_strcpy(video_media_desc->ssrc_infos.payload[0].cname, "metaRTC");
		yang_strcpy(video_media_desc->ssrc_infos.payload[0].mslabel, "metaRTC");
	}
#endif

#if Yang_Enable_RTC_Audio
	if(audio_media_desc) {
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
	}
#endif

#if Yang_Enable_RTC_Video
	if(video_media_desc) {
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
	}
#endif
	if(session->isControlled&&session->context.peerInfo->rtc.enableSdpCandidate){
#if Yang_Enable_RTC_Audio
		if(audio_media_desc)   yang_create_YangCandidateVector(&audio_media_desc->candidates);
#endif
#if Yang_Enable_RTC_Video
		if(video_media_desc) yang_create_YangCandidateVector(&video_media_desc->candidates);
#endif
		int32_t localport=session->context.peerInfo->rtc.rtcLocalPort;

		YangStringVector iplists;
		yang_create_stringVector(&iplists);
		yang_getLocalInfoList(session->context.peerInfo->familyType,&iplists);

		int32_t j=0;
#if Yang_Enable_RTC_Audio
		if(audio_media_desc) {
			for(j=0;j<iplists.vsize;j++)  yang_sdp_genLocalSdp_candidate(session,audio_media_desc,iplists.payload[j],localport);
		}
#endif
#if Yang_Enable_RTC_Video
		if(video_media_desc) {
			for(j=0;j<iplists.vsize;j++)  yang_sdp_genLocalSdp_candidate(session,video_media_desc,iplists.payload[j],localport);
		}
#endif
#if Yang_Enable_Datachannel
		if(data_media_desc){
			yang_create_YangCandidateVector(&data_media_desc->candidates);
			for(j=0;j<iplists.vsize;j++)  yang_sdp_genLocalSdp_candidate(session,data_media_desc,iplists.payload[j],localport);
		}
#endif
		yang_destroy_stringVector(&iplists);
	}

	YangBuffer buf;
	yang_init_buffer(&buf, src, Yang_SDP_BUFFERLEN);
	yang_rtcsdp_encode(local_sdp, &buf);

	if(session->context.peerInfo->rtc.enableSdpCandidate)
		yang_cstr_replace(src, dst, (char*) "\r\n", (char*) "\\r\\n");
	else
		yang_strcpy(dst,src);

	yang_destroy_rtcsdp(local_sdp);
	yang_free(local_sdp);
	yang_free(src);
	return Yang_Ok;

}

int32_t yang_sdp_genLocalSdp(YangRtcSession *session, int32_t localport,char **psdp, YangRtcDirection audioRole,YangRtcDirection videoRole,YangIceMode iceMode){
	char *dst = (char*) yang_calloc(1, Yang_SDP_BUFFERLEN);
	*psdp=dst;
	return yang_sdp_genLocalSdp2(session,localport,dst,audioRole,videoRole,iceMode);
}

YangRtcTrack* yang_sdp_find_track(YangRtcSession* session,uint32_t ssrc){
	int32_t i;
	if(session==NULL) return NULL;
	if(yang_has_ssrc(&session->context.audioTrack,ssrc))
		return &session->context.audioTrack;

	for(i=0;i<session->context.videoTracks.vsize;i++){
		if(session->context.videoTracks.payload[i].ssrc==ssrc)
			return &session->context.videoTracks.payload[i];
	}
	return NULL;
}


static yangbool yang_is_h264PayoadType(char* str) {
	if (yang_strstr(str, "packetization-mode=1") && yang_strstr(str, "profile-level-id=42e01f")) return yangtrue;
	return yangfalse;
}

static yangbool yang_is_h265PayoadType(char* str) {
	if (yang_strstr(str, "profile-id=1")) return yangtrue;
	return yangfalse;
}

static void yang_getRemoteAudioCodec(YangRtcSession* session,YangMediaDesc* desc){
	int32_t k;
	if(!session->isControlled)
		return;

	for(k=0;k<desc->payload_types.vsize;k++){
		if(yang_strcmp2(desc->payload_types.payload[k].encoding_name,"OPUS")==0){
			session->context.opusPayloadType=desc->payload_types.payload[k].payload_type;

		}

		if(yang_strcmp2(desc->payload_types.payload[k].encoding_name,"PCMU")==0){
			session->context.pcmuPayloadType=desc->payload_types.payload[k].payload_type;

		}

		if(yang_strcmp2(desc->payload_types.payload[k].encoding_name,"PCMA")==0){
			session->context.pcmaPayloadType=desc->payload_types.payload[k].payload_type;

		}
	}
	switch(session->context.audioEncodeCodec){
	case Yang_AED_OPUS:session->context.audioPayloadType=session->context.opusPayloadType;break;
	case Yang_AED_PCMA:session->context.audioPayloadType=session->context.pcmaPayloadType;break;
	case Yang_AED_PCMU:session->context.audioPayloadType=session->context.pcmuPayloadType;break;
	default:break;
	}

}

int32_t yang_sdp_parseRemoteSdp(YangRtcSession* session,YangSdp* sdp){
	int32_t i;
	int32_t k=0;
	session->ice.session.remoteIceMode=sdp->iceMode;
	for(i=0;i<sdp->media_descs.vsize;i++){
		YangMediaDesc* desc=&sdp->media_descs.payload[i];
		if(yang_strlen(session->ice.session.remoteIcePwd)==0){
			yang_memset(session->ice.session.remoteIcePwd,0,sizeof(session->ice.session.remoteIcePwd));
			yang_strcpy(session->ice.session.remoteIcePwd,desc->session_info.ice_pwd);
		}

		if(yang_strlen(session->ice.session.remote_ufrag)==0){
			yang_memset(session->ice.session.remote_ufrag,0,sizeof(session->ice.session.remote_ufrag));
			yang_strcpy(session->ice.session.remote_ufrag,desc->session_info.ice_ufrag);
		}

		if(yang_strcmp2(desc->type,"audio")==0){
#if Yang_Enable_RTC_Audio

			if(desc->sendonly){
				session->context.audioDirection=YangRecvonly;
			}else if(desc->recvonly){
				session->context.audioDirection=YangSendonly;
			}else if(desc->sendrecv){
				session->context.audioDirection=YangSendrecv;
			}

			if(desc->ssrc_infos.vsize>0) session->context.audioTrack.ssrc=desc->ssrc_infos.payload[0].ssrc;

			yang_getRemoteAudioCodec(session,desc);
#endif

		}else if(yang_strcmp2(desc->type,"video")==0){
#if Yang_Enable_RTC_Video

			if(desc->sendonly){
				session->context.videoDirection=YangRecvonly;
			}else if(desc->recvonly){
				session->context.videoDirection=YangSendonly;
			}else if(desc->sendrecv){
				session->context.videoDirection=YangSendrecv;
			}


			if(desc->ssrc_infos.vsize>0) {
				for(k=0;k<desc->ssrc_infos.vsize;k++){
					YangRtcTrack videoTrack;
					yang_memset(&videoTrack,0,sizeof(YangRtcTrack));
					videoTrack.ssrc=desc->ssrc_infos.payload[k].ssrc;
					yang_insert_YangRtcTrackVector(&session->context.videoTracks,&videoTrack);
				}
			}


			for(k=0;k<desc->payload_types.vsize;k++){
				YangMediaPayloadType* payload=&desc->payload_types.payload[k];
				if(yang_strcmp2(payload->encoding_name,"H264")==0){
					if(yang_is_h264PayoadType(payload->format_specific_param)){
						session->context.h264PayloadType=payload->payload_type;
					}
				}else if(yang_strcmp2(payload->encoding_name,"H265")==0){
					if(yang_is_h265PayoadType(payload->format_specific_param)){
						session->context.h265PayloadType=payload->payload_type;
					}
				}else if(yang_strcmp2(payload->encoding_name,Yang_AV1_Name)==0){

				}else if(yang_strcmp2(payload->encoding_name,"MJPEG")==0){

				}

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
			session->ice.session.sdpWithCandidate=yangtrue;
			for (k = 0; k < desc->candidates.vsize; k++) {
				if(session->ice.addSdpCandidate) session->ice.addSdpCandidate(&session->ice.session,&desc->candidates.payload[k]);
			}
		}
	}

	return Yang_Ok;
}
