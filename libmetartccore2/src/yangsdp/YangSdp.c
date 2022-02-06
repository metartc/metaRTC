//
// Copyright (c) 2019-2022 yanggaofeng
//
//#include <yangsdp/YangSdp.h>
#include "YangSdp.h"
#include "YangRtcSdp.h"
#include "YangMediaDesc.h"
#include <yangutil/yangtype.h>
#include <yangutil/sys/YangCString.h>
#include <yangutil/sys/YangLog.h>
#include <string.h>


#define Yang_SDP_BUFFERLEN 1024*12
int32_t yang_sdp_genLocalSdp(YangRtcSession *session, int32_t localport,
		char **psdp, YangStreamOptType role) {

	char *src = (char*) calloc(1, Yang_SDP_BUFFERLEN);
	char *dst = (char*) calloc(1, Yang_SDP_BUFFERLEN);
	char randstr[50];


	YangSdp* local_sdp=(YangSdp*)calloc(sizeof(YangSdp),1);
	yang_init_rtcsdp(local_sdp);
		strcpy(local_sdp->version , "0");

		strcpy(local_sdp->username ,"YangRtc");
		memset(randstr,0,sizeof(randstr));
		snprintf(randstr, 22, "%" PRId64, (int64_t) &local_sdp);
		strcpy(local_sdp->session_id ,randstr);
		strcpy(local_sdp->session_version , "2");
		strcpy(local_sdp->nettype , "IN");
		strcpy(local_sdp->addrtype , "IP4");
		strcpy(local_sdp->unicast_address , "0.0.0.0");


		strcpy(local_sdp->session_name , "YangRtcSession");
		strcpy(local_sdp->msid_semantic , "WMS");
		yang_init_stringVector(&local_sdp->msids);
		char streamnames[128];
		memset(streamnames,0,sizeof(streamnames));
		sprintf(streamnames,"%s/%s",session->context.streamConf->app,session->context.streamConf->stream);
		yang_insert_stringVector(&local_sdp->msids,streamnames);

		strcpy(local_sdp->group_policy , "BUNDLE");


		char tmps[5][15];


		yang_init_YangMediaDescVector(&local_sdp->media_descs);
		yang_insert_YangMediaDescVector(&local_sdp->media_descs,NULL);
		yang_insert_YangMediaDescVector(&local_sdp->media_descs,NULL);
		YangMediaDesc *audio_media_desc = &local_sdp->media_descs.payload[0];

		strcpy(audio_media_desc->type,"audio");
		audio_media_desc->port= 9;
		strcpy(audio_media_desc->protos , "UDP/TLS/RTP/SAVPF");
		audio_media_desc->rtcp_mux = true;
		audio_media_desc->rtcp_rsize = true;
		strcpy(audio_media_desc->mid,"0");

		YangMediaDesc *video_media_desc =&local_sdp->media_descs.payload[1];
		strcpy(video_media_desc->type,"video");
		video_media_desc->port = 9;
		strcpy(video_media_desc->protos , "UDP/TLS/RTP/SAVPF");
		video_media_desc->rtcp_mux = true;
		video_media_desc->rtcp_rsize = true;
		strcpy(video_media_desc->mid,"1");
		//sendrecv
		if (role==0) {
			audio_media_desc->recvonly = true;
			video_media_desc->recvonly = true;
		} else if(role==1) {
			audio_media_desc->sendonly = true;
			video_media_desc->sendonly = true;
		}else if(role==2){
			audio_media_desc->sendrecv=true;
			video_media_desc->sendrecv = true;
		}

//session info
		memset(randstr, 0, sizeof(randstr));
		yang_cstr_random(4,randstr);
		strcpy(audio_media_desc->session_info.ice_ufrag,randstr);
		strcpy(video_media_desc->session_info.ice_ufrag,randstr);
		memset(randstr, 0, sizeof(randstr));
		yang_cstr_random(32,randstr);
		strcpy(audio_media_desc->session_info.ice_pwd,randstr);
		strcpy(video_media_desc->session_info.ice_pwd,randstr);
		strcpy(audio_media_desc->session_info.fingerprint_algo,"sha-256");
		strcpy(video_media_desc->session_info.fingerprint_algo,"sha-256");
		strcpy(audio_media_desc->session_info.fingerprint,session->context.cer->fingerprint);
		strcpy(video_media_desc->session_info.fingerprint,session->context.cer->fingerprint);
		strcpy(audio_media_desc->session_info.setup,"actpass");
		strcpy(video_media_desc->session_info.setup,"actpass");


//extmaps
		yang_init_YangExtmapVector(&audio_media_desc->extmaps);
		yang_insert_YangExtmapVector(&audio_media_desc->extmaps,NULL);
		audio_media_desc->extmaps.payload[0].mapid=3;
		strcpy(audio_media_desc->extmaps.payload[0].extmap,"http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01");

		yang_init_YangExtmapVector(&video_media_desc->extmaps);
		yang_insert_YangExtmapVector(&video_media_desc->extmaps,NULL);
		video_media_desc->extmaps.payload[0].mapid=3;
		strcpy(video_media_desc->extmaps.payload[0].extmap,"http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01");

//media pay
		yang_init_YangMediaPayloadTypeVector(&audio_media_desc->payload_types);
		yang_insert_YangMediaPayloadTypeVector(&audio_media_desc->payload_types,NULL);
		YangMediaPayloadType* audiotype=&audio_media_desc->payload_types.payload[0];
		audiotype->payload_type=111;
		strcpy(audiotype->encoding_name,"opus");
		audiotype->clock_rate=session->context.avcontext->avinfo->audio.sample;//48000;
		yang_itoa(session->context.avcontext->avinfo->audio.channel,audiotype->encoding_param,10);
		//strcpy(audiotype->encoding_param,"2");
		strcpy(audiotype->format_specific_param,"minptime=10;useinbandfec=1");
		yang_init_stringVector(&audiotype->rtcp_fb);
		yang_insert_stringVector(&audiotype->rtcp_fb,"transport-cc");

		yang_init_YangMediaPayloadTypeVector(&video_media_desc->payload_types);
		yang_insert_YangMediaPayloadTypeVector(&video_media_desc->payload_types,NULL);
		YangMediaPayloadType* videotype=&video_media_desc->payload_types.payload[0];
		if (session->context.avcontext->avinfo->video.videoEncoderType == Yang_VED_264) {
			videotype->payload_type=125;
			strcpy(videotype->encoding_name,"H264");
		}else if (session->context.avcontext->avinfo->video.videoEncoderType == Yang_VED_265){
			videotype->payload_type=126;
			strcpy(videotype->encoding_name,"H265");
		}
		videotype->clock_rate=90000;
		strcpy(videotype->format_specific_param,"level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42e01f");
		yang_init_stringVector(&videotype->rtcp_fb);
		yang_insert_stringVector(&videotype->rtcp_fb,"transport-cc");
		yang_insert_stringVector(&videotype->rtcp_fb,"nack");
		yang_insert_stringVector(&videotype->rtcp_fb,"nack pli");




	//ssrc info
		yang_insert_YangSSRCInfoVector(&audio_media_desc->ssrc_infos,NULL);
		yang_insert_YangSSRCInfoVector(&video_media_desc->ssrc_infos,NULL);
		audio_media_desc->ssrc_infos.payload[0].ssrc=session->context.audioSsrc;
		video_media_desc->ssrc_infos.payload[0].ssrc=session->context.videoSsrc;

		memset(randstr, 0, sizeof(randstr));
		yang_cstr_random(16, randstr);
		strcpy(audio_media_desc->ssrc_infos.payload[0].cname,randstr);
		strcpy(video_media_desc->ssrc_infos.payload[0].cname,randstr);
		strcpy(audio_media_desc->ssrc_infos.payload[0].mslabel,"-");
		strcpy(video_media_desc->ssrc_infos.payload[0].mslabel,"-");

		memset(tmps, 0, 15 * 5);
		yang_cstr_random(8, tmps[0]);
		yang_cstr_random(4, tmps[1]);
		yang_cstr_random(4, tmps[2]);
		yang_cstr_random(4, tmps[3]);
		yang_cstr_random(12, tmps[4]);
		memset(randstr, 0, sizeof(randstr));
		sprintf(randstr, "%s-%s-%s-%s-%s", tmps[0], tmps[1], tmps[2], tmps[3],	tmps[4]);
		strcpy(audio_media_desc->ssrc_infos.payload[0].msid,"-");
		strcpy(audio_media_desc->ssrc_infos.payload[0].msid_tracker,randstr);
		strcpy(audio_media_desc->ssrc_infos.payload[0].label,randstr);

		memset(tmps, 0, 15 * 5);
		yang_cstr_random(8, tmps[0]);
		yang_cstr_random(4, tmps[1]);
		yang_cstr_random(4, tmps[2]);
		yang_cstr_random(4, tmps[3]);
		yang_cstr_random(12, tmps[4]);
		memset(randstr, 0, sizeof(randstr));
		sprintf(randstr, "%s-%s-%s-%s-%s", tmps[0], tmps[1], tmps[2], tmps[3],tmps[4]);
		strcpy(video_media_desc->ssrc_infos.payload[0].msid,"-");
		strcpy(video_media_desc->ssrc_infos.payload[0].msid_tracker,randstr);
		strcpy(video_media_desc->ssrc_infos.payload[0].label,randstr);
//candidate
		yang_init_YangCandidateVector(&audio_media_desc->candidates);
		yang_insert_YangCandidateVector(&audio_media_desc->candidates,NULL);
		strcpy(audio_media_desc->candidates.payload[0].ip_,session->context.streamConf->serverIp);
		audio_media_desc->candidates.payload[0].port_=8000;
		strcpy(audio_media_desc->candidates.payload[0].type_,"host");

		yang_init_YangCandidateVector(&video_media_desc->candidates);
		yang_insert_YangCandidateVector(&video_media_desc->candidates,NULL);
		strcpy(video_media_desc->candidates.payload[0].ip_,session->context.streamConf->serverIp);
		video_media_desc->candidates.payload[0].port_=8000;
		strcpy(video_media_desc->candidates.payload[0].type_,"host");



	YangBuffer buf;
	yang_init_buffer(&buf,src,Yang_SDP_BUFFERLEN);
	yang_rtcsdp_encode(local_sdp,&buf);

	yang_cstr_replace(src, dst, (char*)"\r\n", (char*)"\\r\\n");
	*psdp = dst;
	yang_destroy_rtcsdp(local_sdp);
	yang_free(local_sdp);
	yang_free(src);
	return Yang_Ok;

}

