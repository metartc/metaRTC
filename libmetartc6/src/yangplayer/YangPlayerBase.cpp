//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangplayer/YangPlayerBase.h>

YangPlayerBase::YangPlayerBase()
{

    m_ydb=NULL;
    m_ypb=NULL;

}

YangPlayerBase::~YangPlayerBase()
{

	yang_delete(m_ydb);
	yang_delete(m_ypb);
}

void YangPlayerBase::stopAll(){
	if(m_ydb) 		m_ydb->stopAll();
	if(m_ypb) 		m_ypb->stopAll();
}


void YangPlayerBase::init(YangContext* pcontext){
	//YangAudioInfo* audio=&pcontext->audio;
	if(m_ydb==NULL) 		{
		m_ydb=new YangPlayerDecoder(pcontext);

		m_ydb->initAudioDecoder();
		m_ydb->initVideoDecoder();
	}

	if(m_ypb==NULL) {
		m_ypb=new YangPlayerPlay();
		m_ypb->initAudioPlay(pcontext);

		m_ypb->setInAudioList(m_ydb->getOutAudioBuffer());
;
	}

}

void YangPlayerBase::startAudioDecoder(YangAudioEncoderBuffer *prr){
	m_ydb->setInAudioBuffer(prr);
	m_ydb->startAudioDecoder();
}
void YangPlayerBase::startVideoDecoder(YangVideoDecoderBuffer *prr){
	m_ydb->setInVideoBuffer(prr);
	m_ydb->startVideoDecoder();
}

void YangPlayerBase::startAudioPlay(YangContext* paudio) {

	if(m_ypb){
		m_ypb->initAudioPlay(paudio);
		m_ypb->startAudioPlay();
	}
}



