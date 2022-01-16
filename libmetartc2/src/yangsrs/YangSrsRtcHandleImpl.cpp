
#include <yangsrs/YangSrsRtcHandleImpl.h>

#include <yangutil/buffer/YangBuffer.h>

#include <yangutil/sys/YangString.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangSocket.h>
#ifdef _WIN32
#include <winsock2.h>
#define be32toh ntohl
#endif



void g_yang_setPlayMediaConfig(	YangAudioParam *remote_audio,YangVideoParam *remote_video,void* user){
	if(user==NULL) return;
		YangSrsRtcHandleImpl* rtcHandle=(YangSrsRtcHandleImpl*)user;

	if(rtcHandle->m_conf&&rtcHandle->m_stream){
		rtcHandle->m_conf->streams.setMediaConfig(rtcHandle->m_stream->uid,	remote_audio,remote_video);
	}
}

void g_yang_sendRequest(int32_t puid,uint32_t ssrc,YangRequestType req,void* user){
	if(user==NULL) return;
		YangSrsRtcHandleImpl* rtcHandle=(YangSrsRtcHandleImpl*)user;
		rtcHandle->m_conf->streams.sendRequest(puid, ssrc,req);
}
void g_rtc_receiveAudio(YangFrame *audioFrame,void* user){
	if(user==NULL) return;
	YangSrsRtcHandleImpl* rtcHandle=(YangSrsRtcHandleImpl*)user;
	if(rtcHandle&&rtcHandle->m_recvcb) rtcHandle->m_recvcb->receiveAudio(audioFrame);
}
void g_rtc_receiveVideo(YangFrame *videoFrame,void* user){
	if(user==NULL) return;
	YangSrsRtcHandleImpl* rtcHandle=(YangSrsRtcHandleImpl*)user;
	if(rtcHandle&&rtcHandle->m_recvcb) rtcHandle->m_recvcb->receiveVideo(videoFrame);
}


YangRtcHandle::YangRtcHandle(){
	m_recvcb=NULL;
	m_conf=NULL;
}

YangRtcHandle::~YangRtcHandle(){
	m_recvcb=NULL;
	m_conf=NULL;
}

void YangRtcHandle::setReceiveCallback(YangReceiveCallback* cbk){

	m_recvcb=cbk;
}

YangRtcHandle* YangRtcHandle::createRtcHandle(YangContext* pcontext){
	return new YangSrsRtcHandleImpl(pcontext);
}

YangSrsRtcHandleImpl::YangSrsRtcHandleImpl(YangContext* pcontext) {
	memset(&m_peerconn,0,sizeof(YangPeerConnection));
    yang_init_peerConnection(&m_peerconn);
	m_conf=pcontext;
	m_stream=NULL;
	m_isInit=0;

	memset(&m_audioFrame,0,sizeof(YangFrame));
	memset(&m_videoFrame,0,sizeof(YangFrame));
}

YangSrsRtcHandleImpl::~YangSrsRtcHandleImpl() {
	yang_destroy_peerConnection(&m_peerconn);
}

int32_t YangSrsRtcHandleImpl::disconnectServer(){
	if(m_peerconn.disconnectServer) m_peerconn.disconnectServer(&m_peerconn.peer);
	return Yang_Ok;
}

int32_t YangSrsRtcHandleImpl::getState(){
    if(m_peerconn.getState)	return m_peerconn.getState(&m_peerconn.peer);
    return 0;
}

int32_t YangSrsRtcHandleImpl::publishVideo(YangStreamCapture* p){
	m_videoFrame.payload=p->getVideoData();
	m_videoFrame.nb=p->getVideoLen();
	m_videoFrame.pts=p->getVideoTimestamp();
	m_videoFrame.frametype=p->getVideoFrametype();

	if(m_peerconn.publishVideo) return  m_peerconn.publishVideo(&m_peerconn.peer,&m_videoFrame);
	return Yang_Ok;
}
int32_t YangSrsRtcHandleImpl::publishAudio(YangStreamCapture* p){
	m_audioFrame.payload=p->getAudioData();
	m_audioFrame.nb=p->getAudioLen();
	m_audioFrame.pts=p->getAudioTimestamp();
	if(m_peerconn.publishAudio) return  m_peerconn.publishAudio(&m_peerconn.peer,&m_audioFrame);
	return Yang_Ok;
}
int YangSrsRtcHandleImpl::notify(int puid,YangRtcMessageType mess){
	if(m_peerconn.recvvideo_notify) m_peerconn.recvvideo_notify(&m_peerconn.peer,mess);
	return Yang_Ok;

}

void YangSrsRtcHandleImpl::init(YangStreamConfig* pconf) {
	if(pconf==NULL) return;
	m_stream=pconf;
	if(m_isInit==0){
		YangAVContext avcontext;
		memset(&avcontext,0,sizeof(YangAVContext));
		avcontext.avinfo=&m_conf->avinfo;
		avcontext.sendRequest=g_yang_sendRequest;
		avcontext.setPlayMediaConfig=g_yang_setPlayMediaConfig;

		avcontext.recvcb.receiveAudio=g_rtc_receiveAudio;
		avcontext.recvcb.receiveVideo=g_rtc_receiveVideo;

		m_peerconn.peer.avcontext=&avcontext;
		m_peerconn.peer.streamconfig=pconf;
		m_peerconn.peer.user=this;
		m_peerconn.init(&m_peerconn.peer);
        if(pconf->streamOptType==Yang_Stream_Play) m_conf->streams.setRtcMessageNotify(pconf->uid,this);

		m_isInit=1;
	}

}
int32_t YangSrsRtcHandleImpl::connectRtcServer(){
    return m_peerconn.connectServer(&m_peerconn.peer);
}


