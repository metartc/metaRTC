//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangipc/YangIpcRtc.h>

#include <yangstream/YangStreamCapture.h>
#include <yangstream/YangStreamType.h>
#include <yangavutil/video/YangNalu.h>
#include <yangavutil/video/YangMeta.h>

#include <yangutil/sys/YangLog.h>

#include <yangutil/yang_unistd.h>


#include <stdio.h>


yang_vector_impl2(YangPeerConnection)

yang_vector_impl(yangint32)

int32_t yang_ipc_rtcrecv_removePeer(YangIpcRtcSession* session,int32_t puid);
void g_ipc_rtcrecv_sendData(void* context,YangFrame* frame){
	//YangP2pRtcSession* rtcHandle=(YangP2pRtcSession*)context;
	//rtcHandle->publishMsg(frame);
}
void g_ipc_rtcrecv_sslAlert(void* context,int32_t uid,char* type,char* desc){
	if(context==NULL||type==NULL||desc==NULL) return;
	YangIpcRtcSession* session=(YangIpcRtcSession*)context;

	if(strcmp(type,"warning")==0&&strcmp(desc,"CN")==0){
		yang_ipc_rtcrecv_removePeer(session,uid);
	}
}
void g_ipc_rtcrecv_receiveAudio(void* user,YangFrame *audioFrame){
	if(user==NULL || audioFrame==NULL) return;
	//YangIpcRtcSession* rtcHandle=(YangIpcRtcSession*)user;
	//if(rtcHandle->out_audioBuffer) rtcHandle->out_audioBuffer->putAudio(audioFrame);
}
void g_ipc_rtcrecv_receiveVideo(void* user,YangFrame *videoFrame){
	if(user==NULL || videoFrame==NULL) return;
	//YangIpcRtcSession* rtcHandle=(YangIpcRtcSession*)user;
	// if(rtcHandle->out_videoBuffer) rtcHandle->out_videoBuffer->putEVideo(videoFrame);
}

void g_ipc_rtcrecv_receiveMsg(void* user,YangFrame *msgFrame){
	if(user==NULL) return;
	yang_trace("recv datachannel:%s",msgFrame->payload);
	//YangIpcRtcSession* rtcHandle=(YangIpcRtcSession*)user;
	//rtcHandle->receiveMsg(msgFrame);
}
int32_t g_ipc_rtcrecv_notify(void* user,int puid,YangRtcMessageType mess){
	if(user==NULL) return 1;
	//YangP2pRtcSession* rtcHandle=(YangP2pRtcSession*)user;
	//	 rtcHandle->sendRtcMessage(puid, mess);
	return Yang_Ok;
}



int32_t yang_ipc_rtcrecv_publishMsg(YangIpcRtcSession* session,YangFrame* msgFrame){
	int32_t ret=0;
	for(int32_t i=0;i<session->pushs.vec.vsize;i++){
		YangPeerConnection* rtc=session->pushs.vec.payload[i];
		if(rtc->isConnected(&rtc->peer)){
			msgFrame->uid=rtc->peer.streamconfig.uid;
			ret = rtc->on_message(&rtc->peer,msgFrame);
		}
	}
	return ret;
}

int32_t yang_ipc_rtcrecv_removePeer(YangIpcRtcSession* session,int32_t puid){

	pthread_mutex_lock(&session->mutex);
	yang_insert_yangint32Vector(&session->removeList,&puid);

	pthread_mutex_unlock(&session->mutex);
	return Yang_Ok;
}
void yang_ipc_rtcrecv_checkAlive(YangIpcRtcSession* session){
	if(session->removeList.vsize>0) return;
	for(int32_t i=0;i<session->pushs.vec.vsize;i++){
		YangPeerConnection* rtc=session->pushs.vec.payload[i];
		if(rtc->isConnected(&rtc->peer)&&!rtc->isAlive(&rtc->peer)) {
			yang_trace("\nremove timeout session:%d",rtc->peer.streamconfig.uid);
			yang_ipc_rtcrecv_removePeer(session,rtc->peer.streamconfig.uid);
		}
	}
}

int32_t yang_ipc_rtcrecv_addPeer(YangIpcRtcSession* session,char* sdp,char* answer,char* remoteIp,int32_t localPort,int* phasplay){
	int32_t ret = 0;
	YangPeerConnection* sh=(YangPeerConnection*)calloc(sizeof(YangPeerConnection),1);
	memset(&sh->peer.streamconfig,0,sizeof(sh->peer.streamconfig));
	sh->peer.streamconfig.uid=session->uidSeq++;
	sh->peer.streamconfig.localPort=localPort;
	sh->peer.streamconfig.isServer=1;
	sh->peer.streamconfig.streamOptType=Yang_Stream_Both;
	strcpy(sh->peer.streamconfig.remoteIp,remoteIp);

	sh->peer.streamconfig.sslCallback.context=session;
	sh->peer.streamconfig.sslCallback.sslAlert=g_ipc_rtcrecv_sslAlert;
	sh->peer.streamconfig.recvCallback.context=session;
	sh->peer.streamconfig.recvCallback.receiveAudio=g_ipc_rtcrecv_receiveAudio;
	sh->peer.streamconfig.recvCallback.receiveVideo=g_ipc_rtcrecv_receiveVideo;
	sh->peer.streamconfig.recvCallback.receiveMsg=g_ipc_rtcrecv_receiveMsg;

	memcpy(&sh->peer.streamconfig.rtcCallback,&session->rtcCallback,sizeof(YangRtcCallback));
	sh->peer.avinfo=session->avinfo;
	yang_create_peerConnection(sh);

	sh->init(&sh->peer);
	if (sh->isConnected(&sh->peer))		return Yang_Ok;
	//if(session->avinfo->rtc.iceCandidateType) sh->requestStunServer(&sh->peer);
	ret = sh->setRemoteDescription(&sh->peer,sdp);

	if (ret)		return ret;
	//取得answer传回对端
	ret = sh->createHttpAnswer(&sh->peer,answer);
	pthread_mutex_lock(&session->mutex);
	session->pushs.insert(&session->pushs.vec,sh);
	pthread_mutex_unlock(&session->mutex);

	if(sh->peer.streamconfig.streamOptType==Yang_Stream_Both||sh->peer.streamconfig.streamOptType==Yang_Stream_Play){

	}

	if(session->pushs.vec.vsize==1){
		//yang_reindex(session->in_audioBuffer);
		yang_reindex2(session->in_videoBuffer);
	}
	//if(m_p2pRtcI) m_p2pRtcI->sendKeyframe();
	*phasplay=sh->peer.streamconfig.streamOptType==Yang_Stream_Both?1:0;

	return Yang_Ok;
}

int32_t yang_ipc_rtcrecv_erasePeer(YangIpcRtcSession* session,int32_t uid){

	for(int32_t i=0;i<session->pushs.vec.vsize;i++){

		if(session->pushs.vec.payload[i]->peer.streamconfig.uid==uid){
			YangStreamOptType streamOpt=session->pushs.vec.payload[i]->peer.streamconfig.streamOptType;
			yang_destroy_peerConnection(session->pushs.vec.payload[i]);
			yang_free(session->pushs.vec.payload[i]);
			session->pushs.remove(&session->pushs.vec,i);

			//if(session->context) session->context->streams.connectNotify(uid,streamOpt, false);
			if(streamOpt==Yang_Stream_Both||streamOpt==Yang_Stream_Play){
				session->playCount--;
				//if(session->playCount<1)   yang_post_message(Yangsession->P2p_Play_Stop,0,NULL);
				//  if(session->p2pRtcI) session->p2pRtcI->removePlayBuffer(uid,session->playCount);
			}

			return Yang_Ok;
		}
	}

	return Yang_Ok;
}

void yang_ipc_rtcrecv_removeStream(YangIpcRtcSession* session){

	for(;session->removeList.vsize>0;){
		int32_t puid=session->removeList.payload[0];
		yang_remove_yangint32Vector(&session->removeList,0);
		yang_ipc_rtcrecv_erasePeer(session,puid);
	}
}

int32_t yang_ipc_rtcrecv_connectMediaServer(YangIpcRtcSession* session) {
	if(session->pushs.vec.vsize>0) return Yang_Ok;

	return Yang_Ok;
}

int32_t yang_ipc_rtcrecv_disConnectPeer(YangIpcRtcSession* session) {
	yang_ipc_rtcrecv_removePeer(session,session->clientUid);
	session->clientUid=-1;
	return Yang_Ok;
}

int32_t yang_ipc_rtcrecv_publishVideoData(YangIpcRtcSession* session,YangStreamCapture* data){
	int32_t ret=0;

	for(int32_t i=0;i<session->pushs.vec.vsize;i++){
		YangPeerConnection* rtc=session->pushs.vec.payload[i];

		if(rtc->isConnected(&rtc->peer)){
			ret = rtc->on_video(&rtc->peer,data->getVideoFrame(data->context));
		}
	}
	return ret;
}
int32_t yang_ipc_rtcrecv_publishAudioData(YangIpcRtcSession* session,YangStreamCapture* data){
	int32_t ret=0;
	for(int32_t i=0;i<session->pushs.vec.vsize;i++){
		YangPeerConnection* rtc=session->pushs.vec.payload[i];
		if(rtc->isConnected(&rtc->peer)){
			ret = rtc->on_audio(&rtc->peer,data->getAudioFrame(data->context));
		}
	}
	return ret;
}
void* yang_ipc_rtcrecv_start_thread(void *obj) {

	YangIpcRtcSession* session=(YangIpcRtcSession*)obj;

	session->isConvert = 1;

	YangStreamCapture data;
	memset(&data,0,sizeof(YangStreamCapture));
	yang_create_streamCapture(&data);
	YangFrame audioFrame;
	YangFrame videoFrame;
	memset(&audioFrame,0,sizeof(YangFrame));
	memset(&videoFrame,0,sizeof(YangFrame));
	data.initAudio(data.context,session->avinfo->sys.transType,session->avinfo->audio.sample, session->avinfo->audio.channel,
			(YangAudioCodec) session->avinfo->audio.audioEncoderType);
	data.initVideo(data.context,session->avinfo->sys.transType);
	YangVideoCodec videoType =	(YangVideoCodec) session->avinfo->video.videoEncoderType;

	int32_t ret = Yang_Ok;

	session->avinfo->enc.createMeta=0;
	YangVideoMeta* vmd=NULL;
	if(session->avinfo->enc.createMeta==0){
		vmd=(YangVideoMeta*)calloc(sizeof( YangVideoMeta),1);
	}

	YangH264NaluData nalu;

	while (session->isConvert == 1) {

		if (session->pushs.vec.vsize == 0) {
			yang_usleep(2000);
			continue;
		}

		if(session->removeList.vsize>0){
			pthread_mutex_lock(&session->mutex);
			yang_ipc_rtcrecv_removeStream(session);
			pthread_mutex_unlock(&session->mutex);
			continue;
		}

		if ((session->in_videoBuffer && session->in_videoBuffer->size(&session->in_videoBuffer->mediaBuffer) == 0)) {
				yang_usleep(2000);
				continue;
			}

		if (session->in_videoBuffer && session->in_videoBuffer->size(&session->in_videoBuffer->mediaBuffer) > 0) {

			videoFrame.payload = session->in_videoBuffer->getEVideoRef(&session->in_videoBuffer->mediaBuffer,&videoFrame);

			if (videoFrame.frametype == YANG_Frametype_I) {

				if (!vmd->isInit) {
					if (videoType == Yang_VED_264) {
						yang_createH264Meta(vmd, &videoFrame);
						yang_getConfig_Flv_H264(&vmd->mp4Meta,vmd->livingMeta.buffer,&vmd->livingMeta.bufLen);
					} else if (videoType == Yang_VED_265) {
						yang_createH265Meta(vmd, &videoFrame);
						yang_getConfig_Flv_H265(&vmd->mp4Meta,vmd->livingMeta.buffer,&vmd->livingMeta.bufLen);
					}

					data.setVideoMeta(data.context,vmd->livingMeta.buffer,vmd->livingMeta.bufLen, videoType);
				}
				data.setVideoFrametype(data.context,YANG_Frametype_Spspps);
				data.setMetaTimestamp(data.context,videoFrame.pts);
				ret = yang_ipc_rtcrecv_publishVideoData(session,&data);

				if (!session->avinfo->enc.createMeta) {
					memset(&nalu, 0, sizeof(YangH264NaluData));
					yang_parseH264Nalu(&videoFrame, &nalu);
					if (nalu.keyframePos > -1) {
						videoFrame.payload += nalu.keyframePos + 4;
						videoFrame.nb -= (nalu.keyframePos + 4);

					} else {
						videoFrame.payload = NULL;
						continue;
					}
				}
			}

			data.setVideoData(data.context,&videoFrame, videoType);

			if((ret = yang_ipc_rtcrecv_publishVideoData(session,&data))!=Yang_Ok){

			}

		}			//end
	}

	yang_destroy_streamCapture(&data);
	yang_free(vmd);
	return NULL;
}
void yang_ipc_rtcrecv_start(YangIpcRtcSession* session)  {
	if(session->isStart) return;
	session->isStart = 1;

	if (pthread_create(&session->threadId, 0, yang_ipc_rtcrecv_start_thread, session)) {
		yang_error("YangThread::start could not start thread");
	}
	session->isStart = 0;
}
void yang_ipc_rtcrecv_stop(YangIpcRtcSession* session){
	session->isConvert=0;
}
void yang_create_p2prtc(YangIpcRtc* rtc,YangAVInfo* avinfo){
	YangIpcRtcSession* session=&rtc->session;
	session->in_videoBuffer = NULL;

	session->avinfo=avinfo;

	session->isStart = 0;
	session->isConvert = 0;
	session->uidSeq=0;


	session->isInit = 0;


	yang_create_YangPeerConnectionVector2(&session->pushs);

	yang_create_yangint32Vector(&session->removeList);

	pthread_mutex_init(&session->mutex, NULL);




	rtc->addPeer=yang_ipc_rtcrecv_addPeer;
	rtc->checkAlive=yang_ipc_rtcrecv_checkAlive;
	rtc->removePeer=yang_ipc_rtcrecv_removePeer;
	rtc->start=yang_ipc_rtcrecv_start;
	rtc->stop=yang_ipc_rtcrecv_stop;

}

void yang_destroy_p2prtc(YangIpcRtc* rtc){
	YangIpcRtcSession* session=&rtc->session;
	if (session->isConvert) {
		session->isConvert=0;
		while (session->isStart) {
			yang_usleep(1000);
		}
	}
	for(int32_t i=0;i<session->pushs.vec.vsize;i++){
		session->pushs.vec.payload[i]->close(&session->pushs.vec.payload[i]->peer);
		yang_destroy_peerConnection(session->pushs.vec.payload[i]);
		yang_free(session->pushs.vec.payload[i]);
	}
	session->pushs.clear(&session->pushs.vec);

	yang_destroy_YangPeerConnectionVector2(&session->pushs);
	yang_destroy_yangint32Vector(&session->removeList);


	session->in_videoBuffer = NULL;
	pthread_mutex_destroy(&session->mutex);
}
