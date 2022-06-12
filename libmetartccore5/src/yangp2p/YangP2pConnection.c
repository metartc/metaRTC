//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangp2p/YangP2pConnection.h>
#include <yangp2p/YangP2pSdp.h>
#include <yangrtc/YangRtcSession.h>
#include <yangrtc/YangRtcStun.h>
#include <yangrtc/YangUdpHandle.h>



#include <yangsdp/YangSdp.h>
#include <yangutil/yangtype.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangCString.h>
#include <yangsrs/YangSrsSdp.h>




int32_t yang_p2p_startRtc(YangRtcSession* session,char* sdp){
	yang_trace("\nyang_p2p_startRtc,port==%d",session->context.streamConfig->localPort);

	session->isServer=session->context.streamConfig->isServer;

	yang_p2p_getRemoteSdp(session,sdp);
	yang_stun_createRequestStunPacket(session,session->remoteIcePwd);

	yang_create_rtcudp(session->context.udp,session->context.streamConfig->remoteIp,session->isServer?0:session->listenPort, session->context.streamConfig->localPort);
	yang_rtcsession_startudp(session);
	yang_start_rtcudp(session->context.udp);
	return Yang_Ok;
}
int32_t yang_p2p_connectRtcServer(YangRtcSession* session){
	int err=Yang_Ok;
	P2pSdpResponseType sdp;
	memset(&sdp,0,sizeof(P2pSdpResponseType));
    if ((err=yang_p2p_doHandleSignal(session,&sdp,session->context.streamConfig->localPort,session->context.streamConfig->streamOptType))  == Yang_Ok) {
		yang_p2p_startRtc(session,sdp.sdp);
	}
	yang_destroy_p2presponse(&sdp);
    return err;
}

YangRtcTrack* yang_p2p_find_track(YangRtcSession* session,uint32_t ssrc){
	if(session==NULL) return NULL;
	if(yang_has_ssrc(&session->context.audioTrack,ssrc)) return &session->context.audioTrack;
	for(int32_t i=0;i<session->context.videoTracks.vsize;i++){
		if(session->context.videoTracks.payload[i].ssrc==ssrc) return &session->context.videoTracks.payload[i];
	}
	return NULL;
}
int32_t yang_p2p_getRemoteSdp(YangRtcSession* session,char* sdpstr) {
	int32_t err = Yang_Ok;

	YangSdp sdp;
	memset(&sdp,0,sizeof(YangSdp));
	yang_create_rtcsdp(&sdp);
	if(session->remote_audio==NULL) session->remote_audio=(YangAudioParam*)calloc(sizeof(YangAudioParam),1);
	if(session->remote_video==NULL) session->remote_video=(YangVideoParam*)calloc(sizeof(YangVideoParam),1);

	yang_rtcsdp_parse(&sdp,sdpstr);


	for(int32_t i=0;i<sdp.media_descs.vsize;i++){
		YangMediaDesc* desc=&sdp.media_descs.payload[i];


		if(desc->sendonly){
			session->context.streamConfig->streamOptType=Yang_Stream_Play;
		}else if(desc->recvonly){
			session->context.streamConfig->streamOptType=Yang_Stream_Publish;
		}else if(desc->sendrecv){
			session->context.streamConfig->streamOptType=Yang_Stream_Both;
		}
	}

	yang_sdp_parseRemoteSdp(session,&sdp);
	yang_destroy_rtcsdp(&sdp);
	return err;
}



