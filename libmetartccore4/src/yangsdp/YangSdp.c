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

void yang_sdp_genLocalSdp_payloadType(YangMediaPayloadType *videotype){
	videotype->clock_rate = 90000;
	strcpy(videotype->format_specific_param,"level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42e01f");
	yang_create_stringVector(&videotype->rtcp_fb);
	yang_insert_stringVector(&videotype->rtcp_fb, "transport-cc");
	yang_insert_stringVector(&videotype->rtcp_fb, "nack");
	yang_insert_stringVector(&videotype->rtcp_fb, "nack pli");
}

#define Yang_SDP_BUFFERLEN 1024*12
int32_t yang_sdp_genLocalSdp(YangRtcSession *session, int32_t localport,char **psdp, YangStreamOptType role) {
	int32_t mediaServer=session->context.avcontext->avinfo->sys.mediaServer;
	int32_t redPayloadtype=1;
	char *src = (char*) calloc(1, Yang_SDP_BUFFERLEN);
	char *dst = (char*) calloc(1, Yang_SDP_BUFFERLEN);
	char randstr[64];


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
	char streamnames[128];
	memset(streamnames, 0, sizeof(streamnames));
	sprintf(streamnames, "%s/%s", session->context.streamConf->app,session->context.streamConf->stream);
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

	YangMediaDesc *data_media_desc = NULL;
	if(session->context.avcontext->avinfo->rtc.usingDatachannel){
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
	//if(data_media_desc) data_media_desc->sendrecv = true;
	if(data_media_desc) data_media_desc->sctp_port = true;
	//session info
	memset(randstr, 0, sizeof(randstr));
	yang_cstr_random(4, randstr);
	memset(session->local_ufrag,0,sizeof(session->local_ufrag));
	strcpy(session->local_ufrag, randstr);
	strcpy(audio_media_desc->session_info.ice_ufrag, randstr);
	strcpy(video_media_desc->session_info.ice_ufrag, randstr);
	if(data_media_desc) strcpy(data_media_desc->session_info.ice_ufrag, randstr);

	memset(randstr, 0, sizeof(randstr));
	yang_cstr_random(32, randstr);
	strcpy(audio_media_desc->session_info.ice_pwd, randstr);
	strcpy(video_media_desc->session_info.ice_pwd, randstr);
	if(data_media_desc) strcpy(data_media_desc->session_info.ice_pwd, randstr);
	memset(session->localIcePwd,0,sizeof(session->localIcePwd));
	strcpy(session->localIcePwd,randstr);

	strcpy(audio_media_desc->session_info.fingerprint_algo, "sha-256");
	strcpy(video_media_desc->session_info.fingerprint_algo, "sha-256");
	if(data_media_desc) strcpy(data_media_desc->session_info.fingerprint_algo, "sha-256");
	if(data_media_desc) strcpy(data_media_desc->session_info.ice_options, "trickle");
#if Yang_HaveDtls
	strcpy(audio_media_desc->session_info.fingerprint,session->context.cer->fingerprint);
	strcpy(video_media_desc->session_info.fingerprint,session->context.cer->fingerprint);
	if(data_media_desc) strcpy(data_media_desc->session_info.fingerprint,session->context.cer->fingerprint);
#else
	strcpy(audio_media_desc->session_info.fingerprint,"EF:7A:50:9C:05:8C:EF:84:4D:72:B2:74:30:BA:FD:82:76:D1:C3:FE:0C:A0:10:43:B8:6C:B2:ED:B3:F7:77:8B");
	strcpy(video_media_desc->session_info.fingerprint,"EF:7A:50:9C:05:8C:EF:84:4D:72:B2:74:30:BA:FD:82:76:D1:C3:FE:0C:A0:10:43:B8:6C:B2:ED:B3:F7:77:8B");
	if(data_media_desc) strcpy(data_media_desc->session_info.fingerprint,"EF:7A:50:9C:05:8C:EF:84:4D:72:B2:74:30:BA:FD:82:76:D1:C3:FE:0C:A0:10:43:B8:6C:B2:ED:B3:F7:77:8B");
#endif
	strcpy(audio_media_desc->session_info.setup, session->isServer?"passive":"actpass");
	strcpy(video_media_desc->session_info.setup, session->isServer?"passive":"actpass");
	if(data_media_desc) strcpy(data_media_desc->session_info.setup, session->isServer?"passive":"actpass");
	//extmaps
	yang_create_YangExtmapVector(&audio_media_desc->extmaps);
	yang_insert_YangExtmapVector(&audio_media_desc->extmaps, NULL);
	audio_media_desc->extmaps.payload[0].mapid = 3;
	strcpy(audio_media_desc->extmaps.payload[0].extmap,
			"http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01");

	yang_create_YangExtmapVector(&video_media_desc->extmaps);
	yang_insert_YangExtmapVector(&video_media_desc->extmaps, NULL);
	video_media_desc->extmaps.payload[0].mapid = 3;
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
	audiotype->clock_rate = session->context.avcontext->avinfo->audio.sample; //48000;
	yang_itoa(session->context.avcontext->avinfo->audio.channel,
			audiotype->encoding_param, 10);
	//strcpy(audiotype->encoding_param,"2");
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
		if (session->context.avcontext->avinfo->video.videoEncoderType	== Yang_VED_264) {
			videotype->payload_type = 125;
			strcpy(videotype->encoding_name, "H264");
		} else if (session->context.avcontext->avinfo->video.videoEncoderType == Yang_VED_265) {
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
		//candidate
		yang_create_YangCandidateVector(&audio_media_desc->candidates);
		yang_insert_YangCandidateVector(&audio_media_desc->candidates, NULL);
		strcpy(audio_media_desc->candidates.payload[0].ip,	session->context.avcontext->avinfo->sys.localIp);
		audio_media_desc->candidates.payload[0].port = session->context.streamConf->localPort;
		strcpy(audio_media_desc->candidates.payload[0].type, "host");

		yang_create_YangCandidateVector(&video_media_desc->candidates);
		yang_insert_YangCandidateVector(&video_media_desc->candidates, NULL);
		strcpy(video_media_desc->candidates.payload[0].ip,	session->context.avcontext->avinfo->sys.localIp);
		video_media_desc->candidates.payload[0].port = session->context.streamConf->localPort;
		strcpy(video_media_desc->candidates.payload[0].type, "host");
		if(data_media_desc) {
			yang_create_YangCandidateVector(&data_media_desc->candidates);
			yang_insert_YangCandidateVector(&data_media_desc->candidates, NULL);
			strcpy(data_media_desc->candidates.payload[0].ip,	session->context.avcontext->avinfo->sys.localIp);
			data_media_desc->candidates.payload[0].port = session->context.streamConf->localPort;
			strcpy(data_media_desc->candidates.payload[0].type, "host");
		}
	}
	YangBuffer buf;
	yang_init_buffer(&buf, src, Yang_SDP_BUFFERLEN);
	yang_rtcsdp_encode(local_sdp, &buf);

	if(mediaServer==Yang_Server_Zlm||(mediaServer==Yang_Server_P2p&&!session->isServer)){
		yang_cstr_replace(src, dst, (char*) "\r\n", (char*) "\n");
		//if(dst[strlen(dst)-1]=='\n') dst[strlen(dst)-1]=0x00;
	}
	else{
		yang_cstr_replace(src, dst, (char*) "\r\n", (char*) "\\r\\n");
	}
	*psdp = dst;
	yang_destroy_rtcsdp(local_sdp);
	yang_free(local_sdp);
	yang_free(src);
	return Yang_Ok;

}

