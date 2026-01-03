//
// Copyright (c) 2019-2026 yanggaofeng
//

#include <yangencoder/YangVideoEncoderMac.h>

void yang_videoencoder_mpp_putFrame(void* session,YangFrame* frame){
	YangVideoEncoderMac* encoder=(YangVideoEncoderMac*)session;
	if(session==NULL || frame==NULL) return;
	if(encoder->m_callback)
		encoder->m_callback->onVideoData(frame);
}

YangVideoEncoderMac::YangVideoEncoderMac() {
	m_encoder=(YangCVideoEncoder*)yang_calloc(sizeof(YangCVideoEncoder),1);
	yang_create_ios_encoder(m_encoder);
	m_mppCallback.session=this;
	m_mppCallback.onVideoData=yang_videoencoder_mpp_putFrame;
	m_callback=NULL;
}

YangVideoEncoderMac::~YangVideoEncoderMac(void) {
	yang_destroy_ios_encoder(m_encoder);
	yang_free(m_encoder);

}
void YangVideoEncoderMac::sendMsgToEncoder(YangRtcEncoderMessage *msg){
	if(msg->request==Yang_Req_Sendkeyframe){
		if(m_encoder)
			m_encoder->sendMsgToEncoder(m_encoder->session,msg->request);
	}else if(msg->request==Yang_Req_HighLostPacketRate){

	}else if(msg->request==Yang_Req_LowLostPacketRate){

	}
}

int32_t YangVideoEncoderMac::init(YangVideoInfo* videoInfo,YangVideoEncInfo* encInfo){
	if (m_isInit == 1)
		return Yang_Ok;

	setVideoPara(videoInfo, encInfo);

	 if(m_encoder)
	 	m_encoder->init(m_encoder->session,videoInfo,&m_enc,&m_mppCallback);
	return Yang_Ok;
}


int32_t YangVideoEncoderMac::encode(YangFrame* pframe, YangEncoderCallback* pcallback) {

	m_callback=pcallback;
	if(m_encoder)
		m_encoder->encode(m_encoder->session,pframe);

	return Yang_Ok;
}

