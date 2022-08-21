//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangipc/YangIpcPublish.h>



void yang_ipcPub_startAudioEncoding(YangIpcPublishSession* session){

}
void yang_ipcPub_startVideoEncoding(YangIpcPublishSession* session){
	session->encoder.start(&session->encoder.session);
}

void yang_ipcPub_stopAudioEncoding(YangIpcPublishSession* session){

}
void yang_ipcPub_stopVideoEncoding(YangIpcPublishSession* session){
	session->encoder.stop(&session->encoder.session);
}
void yang_ipcPub_startRtc(YangIpcPublishSession* session){
	session->rtc.start(&session->rtc.session);
}
void yang_ipcPub_stopRtc(YangIpcPublishSession* session){
	session->rtc.stop(&session->rtc.session);
}


int32_t yang_ipcPub_addPeer(YangIpcPublishSession* session,char* sdp,char* answer,char* remoteIp,int32_t localPort,int* phasplay){
	return session->rtc.addPeer(&session->rtc.session,sdp,answer,remoteIp,localPort,phasplay);
}
void yang_ipcpub_sendRequest(YangIpcPublishSession* session,int32_t puid,uint32_t ssrc,YangRequestType req){
	session->encoder.sendMsgToEncoder(&session->encoder.session,req);
}


void yang_ipcPub_init(YangIpcPublishSession* session){
    if(session)
		session->encoder.init(&session->encoder.session);

}


void yang_create_ipcPublish(YangIpcPublish* publish,YangAVInfo* avinfo){
	YangIpcPublishSession* session=&publish->session;

	yang_create_videoEncoder(&session->encoder);
	yang_create_p2prtc(&session->rtc,avinfo);

	publish->startAudioEncoding=yang_ipcPub_startAudioEncoding;
	publish->stopAudioEncoding=yang_ipcPub_stopAudioEncoding;

	publish->startVideoEncoding=yang_ipcPub_startVideoEncoding;
	publish->stopVideoEncoding=yang_ipcPub_stopVideoEncoding;

	publish->startRtc=yang_ipcPub_startRtc;
	publish->stopRtc=yang_ipcPub_stopRtc;

	publish->addPeer=yang_ipcPub_addPeer;
	publish->sendRequest=yang_ipcpub_sendRequest;
	
	publish->init=yang_ipcPub_init;
}

void yang_destroy_ipcPublish(YangIpcPublish* publish){
	YangIpcPublishSession* session=&publish->session;
	yang_destroy_videoEncoder(&session->encoder);
	yang_destroy_p2prtc(&session->rtc);
}



