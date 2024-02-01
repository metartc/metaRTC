//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangice/YangRtcSocket.h>
#include <yangrtc/YangRtcContext.h>

#include <yangssl/YangSsl.h>
#include <yangssl/YangRtcDtls.h>

#include <yangstream/YangStreamType.h>

#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangSsrc.h>

#if Yang_Enable_Dtls
YangCertificate* g_yang_certificate=NULL;
int32_t g_yang_cerCount=0;
void* yang_get_global_certificate(){
	if(g_yang_certificate==NULL){
		g_yang_certificate = (YangCertificate*) yang_calloc(sizeof(YangCertificate),1);
		yang_create_certificate(g_yang_certificate);
	}
	g_yang_cerCount++;
	return g_yang_certificate;
}
void yang_destroy_global_certificate(){
	g_yang_cerCount--;
	if(g_yang_cerCount==0){
		yang_destroy_certificate(g_yang_certificate);
		yang_free(g_yang_certificate);
		g_yang_destroy_srtp();
	}
}

#endif
yang_vector_impl(YangRtcTrack)

void yang_create_rtcContext(YangRtcContext *context) {
	if (context == NULL)
		return;
	context->state = Yang_Conn_State_New;
	context->videoCodec = Yang_VED_H264;

	context->audioSsrc = yang_generate_ssrc();
	context->videoSsrc = yang_generate_ssrc();

	context->twccId = Yang_TWCC_ID;

	context->sock = ( YangRtcSocket*) yang_calloc(sizeof(YangRtcSocket),1);

	yang_memset(context->sock, 0, sizeof(YangRtcSocket));

#if Yang_Enable_Dtls
	yang_memset(&context->srtp, 0, sizeof(context->srtp));

	context->cer=(YangCertificate*) yang_get_global_certificate();

	context->dtls = ( YangRtcDtls*) yang_calloc(sizeof(YangRtcDtls),1);
	context->dtls->session.sock = context->sock;
	context->dtls->session.srtp = &context->srtp;
	context->dtls->session.cer = context->cer;
#endif
	yang_create_YangRtcTrackVector(&context->videoTracks);

	yang_create_bandwidth(&context->bandwidth);
	yang_create_rtcstats(&context->stats);
#if Yang_Enable_TWCC
	yang_create_rtctwcc(&context->twcc);
#endif

}

void yang_destroy_rtcContext(YangRtcContext *context) {
	if (context == NULL)	return;
	context->streamConfig = NULL;
	yang_free(context->stun.data);
	if (context->sock&&context->sock->stop&&context->sock->session.isStart)
		context->sock->stop(&context->sock->session);
	yang_destroy_rtcsocket(context->sock);
	yang_free(context->sock);
#if Yang_Enable_Dtls
	yang_destroy_rtcdtls(context->dtls);
	yang_free(context->dtls);
	yang_destroy_global_certificate();
#endif
	yang_destroy_YangRtcTrackVector(&context->videoTracks);

	yang_destroy_bandwidth(&context->bandwidth);
	yang_destroy_rtcstats(&context->stats);
#if Yang_Enable_TWCC
	yang_destroy_rtctwcc(&context->twcc);
#endif

}

int32_t yang_has_ssrc(YangRtcTrack *track, uint32_t ssrc) {
	if (ssrc == track->ssrc || ssrc == track->rtxSsrc
			|| ssrc == track->fecSsrc) {
		return 1;
	}

	return 0;
}

