//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtc/YangMetaConnection.h>
#include <yangutil/yangtype.h>
#include <yangutil/yangavtype.h>
#include <yangutil/sys/YangLog.h>

#include <yangavutil/video/YangCMeta.h>
#include <yangavutil/video/YangCNalu.h>

#include <yangrtc/YangRtcConnection.h>
#include <yangrtc/YangRtcConnection.h>
#include <yangsrs/YangSrsConnection.h>
#include <yangzlm/YangZlmConnection.h>
#include <yangp2p/YangP2pConnection.h>
#include <stdio.h>
#include <string.h>

#include <yangrtc/YangStreamUrl.h>

typedef struct{
	YangRtcConnection* conn;
	YangAVInfo* avinfo;
	YangStreamConfig* streamconfig;
	YangFrame extradata;

	int64_t videoTimestamp ;
	int64_t baseTimestamp;
	int64_t curVideotimestamp ;
	int64_t preTimestamp ;

	int64_t unitAudioTime;
	int64_t audioTime;

}YangMetaSession;


void g_yang_mt_init(void* pcontext,YangStreamConfig* stream,void* user){
	if(pcontext==NULL) return;
	YangMetaSession* context=(YangMetaSession*)pcontext;
	if(context->conn == NULL) context->conn=(YangRtcConnection*)calloc(1, sizeof(YangRtcConnection));
	if (context->conn->session == NULL)
		context->conn->session = (YangRtcSession*)calloc(1, sizeof(YangRtcSession));

	context->avinfo=(YangAVInfo*)calloc(sizeof(YangAVInfo),1);
	context->streamconfig=(YangStreamConfig*)calloc(sizeof(YangStreamConfig),1);
	memcpy(context->streamconfig,stream,sizeof(YangStreamConfig));


	context->unitAudioTime=960;
	context->avinfo->rtc.enableDatachannel=0;
    yang_create_rtcConnection(context->conn, context->streamconfig,context->avinfo);

}
int32_t g_yang_mt_connectServer(void* peer,int32_t pmediaServer){
	if(peer==NULL) return 1;

	((YangMetaSession*)peer)->avinfo->sys.mediaServer=pmediaServer;
	YangRtcConnection* conn=((YangMetaSession*)peer)->conn;
	if(conn==NULL) return 1;
	if(pmediaServer==Yang_Server_Zlm) return yang_zlm_connectRtcServer(conn);
//	if(pmediaServer==Yang_Server_P2p) return yang_p2p_connectRtcServer(conn);
	return yang_srs_connectRtcServer(conn);
}

int32_t g_yang_mt_initParam(void* pcontext,char* url,YangStreamOptType opt){
	if(pcontext==NULL) return 1;
	YangMetaSession* context=(YangMetaSession*)pcontext;
	return yang_stream_parseUrl(url, context->streamconfig, context->avinfo, opt);
}

int32_t g_yang_mt_disconnectServer(void* peer){
	if(peer==NULL) return 1;
	YangRtcConnection *conn = ((YangMetaSession*) peer)->conn;
	if(conn==NULL) return 1;
	yang_trace("\nwebrtc disconnect\n");
	if (conn->session->context.state)		conn->close(conn->session);
	conn->session->context.state = 0;
	yang_destroy_rtcConnection(conn);
	yang_free(((YangMetaSession*) peer)->conn);
	return Yang_Ok;
}
void g_yang_mt_setExtradata(void* peer,YangVideoCodec codec,uint8_t *extradata,int32_t extradata_size){
	if(peer==NULL) return;
	YangMetaSession *session = (YangMetaSession*) peer;

	int32_t vpsPos = 0, vpsLen = 0;
	int32_t spsPos = 0, ppsPos = 0;
	int32_t spsLen = 0, ppsLen = 0;
	yang_find_start_code(codec, extradata, extradata_size, &vpsPos, &vpsLen,
			&spsPos, &spsLen, &ppsPos, &ppsLen);
	uint8_t tmp[1024];
	int32_t len=0;
	YangSample sps,pps;
	sps.bytes=(char*)extradata + spsPos;
	sps.nb=spsLen;
	pps.bytes=(char*)extradata+ppsPos;
	pps.nb=ppsLen;
	yang_getConfig_Meta_H264(&sps,&pps,tmp,&len);
	if(session->extradata.payload==NULL){
		session->extradata.payload=(uint8_t*)malloc(len);
		session->extradata.nb=len;
		session->extradata.frametype=YANG_Frametype_Spspps;
		memcpy(session->extradata.payload,tmp,len);
	}


}
int32_t g_yang_mt_publishVideo(void* peer,YangFrame* videoFrame){
	if(peer==NULL||videoFrame==NULL) return 1;

	YangMetaSession *session = (YangMetaSession*) peer;
	if(session->extradata.nb==0) return 1;
	int32_t startLen=4;
	session->videoTimestamp=videoFrame->pts;
	if (session->preTimestamp == 0) {
		session->baseTimestamp = session->videoTimestamp;
		session->curVideotimestamp = 0;

	} else {
		if (session->videoTimestamp <= session->preTimestamp)
			return 0;

		session->curVideotimestamp = session->videoTimestamp - session->baseTimestamp;
	}
	session->preTimestamp = session->videoTimestamp;
	videoFrame->pts=session->curVideotimestamp * 9 / 100;
	videoFrame->dts=videoFrame->pts;
	if(videoFrame->payload[0]==0x00&&videoFrame->payload[1]==0x00&&videoFrame->payload[2]==0x01){
		if((videoFrame->payload[3]& kNalTypeMask) == YangAvcNaluTypeIDR){
			session->extradata.pts=videoFrame->pts;
			session->extradata.frametype=YANG_Frametype_Spspps;
			session->conn->on_video(session->conn->session,&session->extradata);

			startLen=3;

			videoFrame->frametype = YANG_Frametype_I;
			goto sendframe;
		}
	}
	YangH264NaluData nalu;
	memset(&nalu, 0, sizeof(YangH264NaluData));
	yang_parseH264Nalu(videoFrame, &nalu);
	if ((videoFrame->payload[4]& kNalTypeMask) == YangAvcNaluTypeIDR) {
		session->extradata.pts=videoFrame->pts;
		session->conn->on_video(session->conn->session,&session->extradata);

		videoFrame->frametype = YANG_Frametype_I;
			goto sendframe;
	} else {
		videoFrame->frametype = YANG_Frametype_P;
		goto sendframe;

	}
	return  1;

	sendframe:
	videoFrame->payload = videoFrame->payload+ startLen;
	videoFrame->nb -=   startLen;
	return session->conn->on_video(session->conn->session,
			videoFrame);



}
int32_t g_yang_mt_publishAudio(void* peer,YangFrame *audioFrame){
	if(peer==NULL||audioFrame==NULL) return 1;
	YangMetaSession *session = (YangMetaSession*) peer;

	session->audioTime+=session->unitAudioTime;
	audioFrame->pts=session->audioTime;
	audioFrame->dts=audioFrame->pts;

	return  session->conn->on_audio(session->conn->session,audioFrame);
}
int32_t g_yang_mt_recvvideo_notify(void* peer, YangRtcMessageType mess){
	if(peer==NULL) return 1;
	YangMetaSession *session = (YangMetaSession*) peer;
	return session->conn->notify(session->conn->session,mess);
}

int32_t g_yang_mt_getState(void* peer){
	if(peer==NULL) return 0;
	return ((YangMetaSession*) peer)->conn->session->context.state;
}
void g_yang_mt_parseHeader(YangVideoCodec codec,uint8_t *buf, uint8_t *src, int32_t *hLen){
	if(codec==Yang_VED_264) yang_getH264RtmpHeader(buf,src,hLen);
	if(codec==Yang_VED_265) yang_getH265RtmpHeader(buf,src,hLen);
}



void yang_create_metaConnection(YangMetaConnection* metaconn){

	metaconn->session=calloc(sizeof(YangMetaSession),1);
	yang_setLogLevel(5);
	yang_setLogFile(1);

	metaconn->init=g_yang_mt_init;
	metaconn->initParam=g_yang_mt_initParam;
	metaconn->parseHeader=g_yang_mt_parseHeader;
	metaconn->connectSfuServer=g_yang_mt_connectServer;
	metaconn->close=g_yang_mt_disconnectServer;
	metaconn->setExtradata=g_yang_mt_setExtradata;
	metaconn->publishAudio=g_yang_mt_publishAudio;
	metaconn->publishVideo=g_yang_mt_publishVideo;
	metaconn->isConnected=g_yang_mt_getState;
	metaconn->recvvideoNotify=g_yang_mt_recvvideo_notify;
}
void yang_destroy_metaConnection(YangMetaConnection* metaconn){
	if(metaconn==NULL) return;
	 if(((YangRtcSession*)metaconn->session)->context.state) {
		 g_yang_mt_disconnectServer(metaconn->session);
	 }else{
		 if(((YangMetaSession*)metaconn->session)->conn){
			yang_destroy_rtcConnection( ((YangMetaSession*)metaconn->session)->conn);
			yang_free(((YangMetaSession*)metaconn->session)->conn);
		 }
	 }
	 yang_free(((YangMetaSession*)metaconn->session)->streamconfig);
	 yang_free(((YangMetaSession*)metaconn->session)->avinfo);
	 yang_free(((YangMetaSession*)metaconn->session)->extradata.payload);
}




