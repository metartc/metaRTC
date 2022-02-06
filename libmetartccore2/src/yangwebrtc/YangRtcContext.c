//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangwebrtc/YangRtcContext.h>
#include <yangwebrtc/YangRtcDtls.h>
#include <yangwebrtc/YangUdpHandle.h>
#include <yangstream/YangStreamType.h>
#include <yangutil/sys/YangCSsl.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangSsrc.h>
YangCertificate* g_yang_certificate=NULL;
int32_t g_yang_cerCount=0;

yang_vector_impl(YangRtcTrack)

void yang_init_rtcContext(YangRtcContext *context) {
	if (context == NULL)
		return;
	context->codec = Yang_VED_264;

	context->audioSsrc = yang_generate_ssrc();
	context->videoSsrc = yang_generate_ssrc();
	context->avcontext = (YangAVContext*) calloc(sizeof(YangAVContext), 1);
	yang_trace("\naudiossrc===%u,videossrc==%u\n", context->audioSsrc,
			context->videoSsrc);
	context->state = 0;
	memset(&context->srtp, 0, sizeof(context->srtp));
	if(g_yang_certificate==NULL){
		g_yang_certificate = (YangCertificate*) malloc(sizeof(YangCertificate));
		memset(g_yang_certificate, 0, sizeof(YangCertificate));
		yang_init_certificate(g_yang_certificate);
	}
	context->cer=g_yang_certificate;
	g_yang_cerCount++;
	context->udp = ( YangUdpHandle*) malloc(sizeof(YangUdpHandle));
	memset(context->udp, 0, sizeof(YangUdpHandle));

	context->dtls = ( YangRtcDtls*) malloc(sizeof( YangRtcDtls));
	memset(context->dtls, 0, sizeof(YangRtcDtls));

	context->dtls->udp = context->udp;
	context->dtls->srtp = &context->srtp;
	context->dtls->cer = context->cer;
	yang_init_YangRtcTrackVector(&context->videoTracks);

}
void yang_destroy_rtcContext(YangRtcContext *context) {
	if (context == NULL)
		return;
	context->streamConf = NULL;

	yang_free(context->avcontext);



	yang_free(context->stun.data);
	if (context->udp)
		yang_stop_rtcudp(context->udp);
	yang_destroy_rtcudp(context->udp);
	yang_free(context->udp);

	yang_destroy_rtcdtls(context->dtls);
	yang_free(context->dtls);
	g_yang_cerCount--;
	if(g_yang_cerCount==0){
		yang_destroy_certificate(g_yang_certificate);
		yang_free(g_yang_certificate);
		g_yang_destroy_srtp();
	}
	yang_destroy_YangRtcTrackVector(&context->videoTracks);

}
int32_t yang_has_ssrc(YangRtcTrack *track, uint32_t ssrc) {
	if (ssrc == track->ssrc || ssrc == track->rtx_ssrc
			|| ssrc == track->fec_ssrc) {
		return 1;
	}

	return 0;
}
