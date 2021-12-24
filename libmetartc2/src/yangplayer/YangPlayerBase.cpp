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
		//m_ydb->m_audio.sample=audio->sample;
		//m_ydb->m_audio.channel=audio->channel;
		//m_ydb->m_audio.usingMono=audio->usingMono;
		//m_ydb->m_audio.audioDecoderType=audio->audioDecoderType;
		m_ydb->initAudioDecoder();
		m_ydb->initVideoDecoder();
	}

	if(m_ypb==NULL) {
		m_ypb=new YangPlayerPlay();
		m_ypb->initAudioPlay(pcontext);
		//m_ypb->initVideoPlay(m_ydb->m_videoDec);
		//m_ydb->m_videoDec->m_yvp=m_ypb->
		m_ypb->setInAudioList(m_ydb->getOutAudioBuffer());
		//m_ypb->setInVideoList(m_ydb->getOutVideoBuffer());
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
	//m_ydb->startDecoder();
	if(m_ypb){
		m_ypb->initAudioPlay(paudio);
		m_ypb->startAudioPlay();
	}
}



