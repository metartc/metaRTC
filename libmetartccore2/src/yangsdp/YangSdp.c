/*
 * YangSdp.c
 *
 *  Created on: 2022年1月7日
 *      Author: yang
 */

//#include <yangsdp/YangSdp.h>
#include "YangSdp.h"
#include <yangutil/yangtype.h>
#include <yangutil/sys/YangCString.h>
#include <yangutil/sys/YangLog.h>
#include <string.h>
#include "YangLocalH264Sdp.h"
#include "YangLocalH265Sdp.h"

#define Yang_SDP_BUFFERLEN 1024*12
int32_t yang_sdp_genLocalSdp(YangRtcSession *session, int32_t localport,
		char **sdp, YangStreamOptType role) {

	char *src = (char*) calloc(1, Yang_SDP_BUFFERLEN);
	char *dst = (char*) calloc(1, Yang_SDP_BUFFERLEN);
	char randstr[50];
	char *ss;
	if (session->context.avcontext->avinfo->video.videoEncoderType == Yang_VED_264) {
		ss = (char*) local_h264_sdp_str;
	}
#if	Yang_H265_Encoding
	if (session->context.avcontext->avinfo->video.videoEncoderType == Yang_VED_265) {
		ss = (char*) local_h265_sdp_str;
	}
#endif
	memset(randstr, 0, sizeof(randstr));
	yang_cstr_random(16, randstr);
	yang_cstr_replace(ss, src, (char*) "5hcuv2i7jtwg6bj1", randstr);

	char tmps[5][15];
	memset(tmps, 0, 15 * 5);
	yang_cstr_random(8, tmps[0]);
	yang_cstr_random(4, tmps[1]);
	yang_cstr_random(4, tmps[2]);
	yang_cstr_random(4, tmps[3]);
	yang_cstr_random(12, tmps[4]);
	memset(randstr, 0, sizeof(randstr));
	sprintf(randstr, "%s-%s-%s-%s-%s", tmps[0], tmps[1], tmps[2], tmps[3],	tmps[4]);
	memset(dst, 0, Yang_SDP_BUFFERLEN);
	yang_cstr_replace(src, dst, (char*) "79ac70ad-c572-4017-86a1-330d56089c70",	randstr);
	memset(src, 0, Yang_SDP_BUFFERLEN);
	strcpy(src, dst);
	memset(dst, 0, Yang_SDP_BUFFERLEN);

	char ssrc[20];
	memset(ssrc, 0, sizeof(ssrc));
	sprintf(ssrc, "%d", session->context.audioSsrc);
    yang_cstr_replace(src, dst, (char*)"144706935", ssrc);
	memset(src, 0, Yang_SDP_BUFFERLEN);
	strcpy(src, dst);
	memset(dst, 0, Yang_SDP_BUFFERLEN);

	memset(tmps, 0, 15 * 5);
	yang_cstr_random(8, tmps[0]);
	yang_cstr_random(4, tmps[1]);
	yang_cstr_random(4, tmps[2]);
	yang_cstr_random(4, tmps[3]);
	yang_cstr_random(12, tmps[4]);
	memset(randstr, 0, sizeof(randstr));
	sprintf(randstr, "%s-%s-%s-%s-%s", tmps[0], tmps[1], tmps[2], tmps[3],tmps[4]);
    yang_cstr_replace(src, dst, (char*)"549dbfe5-fd12-4c5f-9182-d513f6defce5",	randstr);
	memset(src, 0, Yang_SDP_BUFFERLEN);
	strcpy(src, dst);
	memset(dst, 0, Yang_SDP_BUFFERLEN);

	memset(ssrc, 0, sizeof(ssrc));
	sprintf(ssrc, "%d", session->context.videoSsrc);
    yang_cstr_replace(src, dst, (char*)"144706936", ssrc);
	memset(src, 0, Yang_SDP_BUFFERLEN);
	strcpy(src, dst);
	memset(dst, 0, Yang_SDP_BUFFERLEN);

	memset(randstr, 0, sizeof(randstr));
	yang_cstr_random(4,randstr);
    yang_cstr_replace(src, dst, (char*)"7nlf8226", randstr);
	memset(src, 0, Yang_SDP_BUFFERLEN);
	strcpy(src, dst);
	memset(dst, 0, Yang_SDP_BUFFERLEN);


	memset(randstr, 0, sizeof(randstr));
	yang_cstr_random(32,randstr);
    yang_cstr_replace(src, dst, (char*)"86971b13h9723f8oc0z72h82a42x6184", randstr);
	memset(src, 0, Yang_SDP_BUFFERLEN);
	strcpy(src, dst);
	memset(dst, 0, Yang_SDP_BUFFERLEN);

    yang_cstr_replace(src, dst, (char*)"A0:8B:98:29:DE:2E:16:0C:60:C6:D0:09:1C:A4:E9:3F:7C:06:98:53:03:A3:6F:0E:67:2B:9A:9D:F0:4F:A3:D6", session->context.cer->fingerprint);
	memset(src, 0, Yang_SDP_BUFFERLEN);
	strcpy(src, dst);
	memset(dst, 0, Yang_SDP_BUFFERLEN);


	char cand[256];
	memset(cand, 0, sizeof(cand));
	sprintf(cand, "%s %d", session->context.streamConf->serverIp, localport);
    yang_cstr_replace(src, dst, (char*)"127.0.0.1 8000", cand);
	memset(src, 0, Yang_SDP_BUFFERLEN);
	strcpy(src, dst);
	memset(dst, 0, Yang_SDP_BUFFERLEN);

	if (session->context.avcontext->avinfo->audio.audioEncoderType == Yang_AED_OPUS) {
		char opus[64];
		memset(opus, 0, sizeof(opus));
		sprintf(opus, "opus/%d/%d", session->context.avcontext->avinfo->audio.sample,
				session->context.avcontext->avinfo->audio.channel);
        yang_cstr_replace(src, dst, (char*)"opus/48000/2", opus);
		memset(src, 0, Yang_SDP_BUFFERLEN);
		strcpy(src, dst);
		memset(dst, 0, Yang_SDP_BUFFERLEN);
	}
	if (role == Yang_Stream_Play) {
        yang_cstr_replace(src, dst, (char*)"a=sendonly\n", (char*)"a=recvonly\n");
		memset(src, 0, Yang_SDP_BUFFERLEN);
		strcpy(src, dst);
		memset(dst, 0, Yang_SDP_BUFFERLEN);
	}

    yang_cstr_replace(src, dst, (char*)"\n", (char*)"\\r\\n");
	memset(src, 0, Yang_SDP_BUFFERLEN);
	strcpy(src, dst);
	memset(dst, 0, Yang_SDP_BUFFERLEN);
	*sdp = src;
	yang_free(dst);

	return Yang_Ok;

}

