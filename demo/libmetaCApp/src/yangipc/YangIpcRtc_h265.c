//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangipc/YangIpcRtc.h>

#include <yangstream/YangStreamCapture.h>
#include <yangstream/YangStreamType.h>
#include <yangavutil/video/YangNalu.h>
#include <yangavutil/video/YangMeta.h>

#include <yangutil/sys/YangLog.h>

#include <stdio.h>

#include "YangIpc_H.h"
#if Yang_IPCRTC_Enable_Datachannel_H265
yang_vector_impl2(YangPeerConnection)

yang_vector_impl(yangint32)

int32_t yang_ipc_rtcrecv_removePeer(YangIpcRtcSession* session,int32_t puid);

enum NalUnitType
{
    NAL_UNIT_CODED_SLICE_TRAIL_N = 0, // 0
    NAL_UNIT_CODED_SLICE_TRAIL_R,     // 1

    NAL_UNIT_CODED_SLICE_TSA_N,       // 2
    NAL_UNIT_CODED_SLICE_TSA_R,       // 3

    NAL_UNIT_CODED_SLICE_STSA_N,      // 4
    NAL_UNIT_CODED_SLICE_STSA_R,      // 5

    NAL_UNIT_CODED_SLICE_RADL_N,      // 6
    NAL_UNIT_CODED_SLICE_RADL_R,      // 7

    NAL_UNIT_CODED_SLICE_RASL_N,      // 8
    NAL_UNIT_CODED_SLICE_RASL_R,      // 9

    NAL_UNIT_RESERVED_VCL_N10,
    NAL_UNIT_RESERVED_VCL_R11,
    NAL_UNIT_RESERVED_VCL_N12,
    NAL_UNIT_RESERVED_VCL_R13,
    NAL_UNIT_RESERVED_VCL_N14,
    NAL_UNIT_RESERVED_VCL_R15,

    NAL_UNIT_CODED_SLICE_BLA_W_LP,    // 16
    NAL_UNIT_CODED_SLICE_BLA_W_RADL,  // 17
    NAL_UNIT_CODED_SLICE_BLA_N_LP,    // 18
    NAL_UNIT_CODED_SLICE_IDR_W_RADL,  // 19
    NAL_UNIT_CODED_SLICE_IDR_N_LP,    // 20
    NAL_UNIT_CODED_SLICE_CRA,         // 21
    NAL_UNIT_RESERVED_IRAP_VCL22,
    NAL_UNIT_RESERVED_IRAP_VCL23,

    NAL_UNIT_RESERVED_VCL24,
    NAL_UNIT_RESERVED_VCL25,
    NAL_UNIT_RESERVED_VCL26,
    NAL_UNIT_RESERVED_VCL27,
    NAL_UNIT_RESERVED_VCL28,
    NAL_UNIT_RESERVED_VCL29,
    NAL_UNIT_RESERVED_VCL30,
    NAL_UNIT_RESERVED_VCL31,

    // non-VCL
    NAL_UNIT_VPS,                     // 32
    NAL_UNIT_SPS,                     // 33
    NAL_UNIT_PPS,                     // 34
    NAL_UNIT_AUD,                     // 35
    NAL_UNIT_EOS,                     // 36
    NAL_UNIT_EOB,                     // 37
    NAL_UNIT_FILLER_DATA,             // 38
    NAL_UNIT_PREFIX_SEI,              // 39
    NAL_UNIT_SUFFIX_SEI,              // 40

    NAL_UNIT_RESERVED_NVCL41,
    NAL_UNIT_RESERVED_NVCL42,
    NAL_UNIT_RESERVED_NVCL43,
    NAL_UNIT_RESERVED_NVCL44,
    NAL_UNIT_RESERVED_NVCL45,
    NAL_UNIT_RESERVED_NVCL46,
    NAL_UNIT_RESERVED_NVCL47,
    NAL_UNIT_UNSPECIFIED_48,
    NAL_UNIT_UNSPECIFIED_49,
    NAL_UNIT_UNSPECIFIED_50,
    NAL_UNIT_UNSPECIFIED_51,
    NAL_UNIT_UNSPECIFIED_52,
    NAL_UNIT_UNSPECIFIED_53,
    NAL_UNIT_UNSPECIFIED_54,
    NAL_UNIT_UNSPECIFIED_55,
    NAL_UNIT_UNSPECIFIED_56,
    NAL_UNIT_UNSPECIFIED_57,
    NAL_UNIT_UNSPECIFIED_58,
    NAL_UNIT_UNSPECIFIED_59,
    NAL_UNIT_UNSPECIFIED_60,
    NAL_UNIT_UNSPECIFIED_61,
    NAL_UNIT_UNSPECIFIED_62,
    NAL_UNIT_UNSPECIFIED_63,
    NAL_UNIT_INVALID,
};

int64_t get_time()
{
    struct timespec time = {0, 0};
    clock_gettime(1, &time);
    return (int64_t)time.tv_sec * 1000000 + (int64_t)time.tv_nsec / 1000;
}

// int type = (NALU头第一字节 & 0x7E) >> 1
// hvcC extradata是一种头描述的格式。而annex-b格式中，则是将VPS, SPS和PPS等同于普通NAL，用start code分隔，非常简单。Annex-B格式的”extradata”：
// start code+VPS+start code+SPS+start code+PPS
//格式详情参见以下博客
// 作者：一川烟草i蓑衣
// 链接：https://www.jianshu.com/p/909071e8f8c6
 
char*  GetFrameTypeName(int frametype) {
	switch (frametype){
	case NAL_UNIT_VPS:
		return "H265_FRAME_VPS";
	case NAL_UNIT_SPS:
		return "H265_FRAME_SPS";
	case NAL_UNIT_PPS:
		return "H265_FRAME_PPS";
	case NAL_UNIT_PREFIX_SEI:
		return "H265_FRAME_SEI";
	case NAL_UNIT_CODED_SLICE_CRA:
		return "H265_FRAME_I";
	case NAL_UNIT_RESERVED_VCL_R15:
		return "H265_FRAME_P";
	default:
		return "";
	}
}
int  FindStartCode2(unsigned char *Buf) {
	if ((Buf[0] == 0) &&  (Buf[1] == 0 ) && (Buf[2] == 1)) {
		return  1; //判断是否为0x000001,如果是返回1
	} else {
		return  0;
	}
}

int  FindStartCode3(unsigned char *Buf) {
	if ((Buf[0] == 0) &&  (Buf[1] == 0 ) && (Buf[2] == 0)  && (Buf[3] == 1)) {
		return  1; //判断是否为0x00000001,如果是返回1
	} else {
		return  0;
	}
}
 
char * GetFrameType(unsigned char *pdata,int *ptype) {
	 int destcount  = 0;
	// naluendptr := 0
	if (FindStartCode2(pdata)) {
		destcount = 3;
	} else if (FindStartCode3(pdata)) {
		destcount = 4;
	} else {
		return NULL;
	}
	int temptype;
	temptype =  (pdata[destcount] & 0x7E) >> 1;
	*ptype = temptype;
	return GetFrameTypeName(temptype);
}
//metaRTC 发送datachannel 的函数
void g_ipc_datasendData(YangIpcRtcSession* session,u_int8_t* data,int len,int mediatype){
 
	
	if(len<=0 || data== NULL) return;
	int ret;
	YangFrame H265Frame;

	for(int32_t i=0;i<session->pushs.vec.vsize;i++){
		YangPeerConnection* rtc=session->pushs.vec.payload[i];

		if(rtc->isConnected(&rtc->peer)){
			//memcpy(H265Frame.payload,data,len);
            H265Frame.payload=data;
            H265Frame.mediaType=mediatype;
			H265Frame.nb=len;
			H265Frame.pts=H265Frame.dts=get_time();
			//printf("datachannel send out %s\n",(char*)H265Frame.payload);
            ret = rtc->on_message(&rtc->peer,&H265Frame);
		}
	}

}

#define MAXPACKETSIZE 65536
void SendH265FrameData(YangIpcRtcSession* session, uint8_t* data,int len, int64_t timestamp ) {
 
	if(data!=NULL &&len >0)  {
		char *frametypestr;
        char *endchar="h265 end";
        char startstr[200+1]; 
        int   frametype=0;
        int count=0,rem=0;
		count = len/ MAXPACKETSIZE;
		rem = len % MAXPACKETSIZE;
		frametypestr = GetFrameType(data,&frametype);
		snprintf(startstr,200,"h265 start ,FrameType:%s,nalutype:%d,pts:%lld,Packetslen:%d,packets:%d,rem:%d",frametypestr,frametype,timestamp,len, count,rem);
		//	YANG_DATA_CHANNEL_STRING = 51,
		//	YANG_DATA_CHANNEL_BINARY = 53,
		g_ipc_datasendData(session,startstr,strlen(startstr),YANG_DATA_CHANNEL_STRING);
		//printf("SendH265FrameData start ", startstr);
		int i = 0,lenth=0;
		for (i = 0; i < count; i++) {
			lenth = i * MAXPACKETSIZE;
			g_ipc_datasendData(session,data+lenth,MAXPACKETSIZE,YANG_DATA_CHANNEL_BINARY);
		}
		if (rem != 0) {
			g_ipc_datasendData(session,data+len-rem,rem,YANG_DATA_CHANNEL_BINARY);
		}
		g_ipc_datasendData(session,endchar,strlen(endchar),YANG_DATA_CHANNEL_STRING);
	}
}
uint32_t  get_be32(uint8_t *output) {
		return output[3]|output[2]<<8|output[1]<<16|output[0]<<24;
}

void find_start_code(YangFrame *in_videoFrame,int32_t *vpsPos,int32_t *vpsLen,int32_t *spsPos,int32_t *spsLen,int32_t *ppsPos,int32_t *ppsLen,int32_t  *pidr,int32_t *idrLen)
{
		uint8_t s1[4];
		s1[0]=0x00;
		s1[1]=0x00;
		s1[2]=0x00;
		s1[3]=0x01;
		*vpsPos = 0;
		//
		*vpsLen =get_be32(in_videoFrame->payload) +4;
		memcpy(in_videoFrame->payload + *vpsPos,s1,4);
		*spsPos = *vpsLen + *vpsPos;
		*spsLen = get_be32(in_videoFrame->payload + *spsPos) + 4;
		memcpy(in_videoFrame->payload +*spsPos,s1,4);
		*ppsPos = *spsPos + *spsLen;
		*ppsLen=get_be32(in_videoFrame->payload+ *ppsPos) + 4;;
		memcpy(in_videoFrame->payload+*ppsPos,s1,4);
		*pidr = *ppsPos + *ppsLen;
		*idrLen =get_be32(in_videoFrame->payload+ *pidr) + 4;
		memcpy(in_videoFrame->payload+*pidr,s1,4);
}


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

	yang_thread_mutex_lock(&session->mutex);
	yang_insert_yangint32Vector(&session->removeList,&puid);

	yang_thread_mutex_unlock(&session->mutex);
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
	printf("%s %d\n",__FUNCTION__,__LINE__);
	sh->init(&sh->peer);
	printf("%s %d\n",__FUNCTION__,__LINE__);
	if (sh->isConnected(&sh->peer))		return Yang_Ok;
	//if(session->avinfo->rtc.iceCandidateType) sh->requestStunServer(&sh->peer);
	ret = sh->setRemoteDescription(&sh->peer,sdp);
	printf("%s %d %d\n",__FUNCTION__,__LINE__,ret);
	if (ret)		return ret;
	printf("%s %d\n",__FUNCTION__,__LINE__);
	//取得answer传回对端
	ret = sh->createHttpAnswer(&sh->peer,answer);
	printf("%s %d %s\n",__FUNCTION__,__LINE__,answer);
	yang_thread_mutex_lock(&session->mutex);
	session->pushs.insert(&session->pushs.vec,sh);
	yang_thread_mutex_unlock(&session->mutex);

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

#define H265_SEND_BY_DATACHANNEL 
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
			yang_thread_mutex_lock(&session->mutex);
			yang_ipc_rtcrecv_removeStream(session);
			yang_thread_mutex_unlock(&session->mutex);
			continue;
		}

		if ((session->in_videoBuffer && session->in_videoBuffer->size(&session->in_videoBuffer->mediaBuffer) == 0)) {
				yang_usleep(2000);
				continue;
			}

		if (session->in_videoBuffer && session->in_videoBuffer->size(&session->in_videoBuffer->mediaBuffer) > 0) {

			videoFrame.payload = session->in_videoBuffer->getEVideoRef(&session->in_videoBuffer->mediaBuffer,&videoFrame);
#ifdef H265_SEND_BY_DATACHANNEL
		if (videoFrame.frametype == YANG_Frametype_I) {
			int vpspos,vpslen,spspos,spslen,ppspos,ppslen,idrpos,idrlen;
			find_start_code(&videoFrame,&vpspos,&vpslen,&spspos,&spslen,&ppspos,&ppslen,&idrpos,&idrlen);
			SendH265FrameData(session,videoFrame.payload,videoFrame.nb,videoFrame.pts);
			//SendH265FrameData(session,videoFrame.payload +spspos,spslen,videoFrame.pts);
			//SendH265FrameData(session,videoFrame.payload +ppspos,ppslen,videoFrame.pts);
			//SendH265FrameData(session,videoFrame.payload +idrpos,idrlen,videoFrame.pts);
		}
		else
		{
			uint8_t s1[4];
			s1[0]=0x00;
			s1[1]=0x00;
			s1[2]=0x00;
			s1[3]=0x01;
			memcpy(videoFrame.payload,s1,4);
			SendH265FrameData(session,videoFrame.payload,videoFrame.nb,videoFrame.pts);
		}

#else
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
#endif

		}			//end
	}

	yang_destroy_streamCapture(&data);
	yang_free(vmd);
	return NULL;
}
void yang_ipc_rtcrecv_start(YangIpcRtcSession* session)  {
	if(session->isStart) return;
	session->isStart = 1;

	if (yang_thread_create(&session->threadId, 0, yang_ipc_rtcrecv_start_thread, session)) {
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

	yang_thread_mutex_init(&session->mutex, NULL);




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
	yang_thread_mutex_destroy(&session->mutex);
}
#endif
