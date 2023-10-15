//
// Copyright (c) 2019-2022 yanggaofeng
//


#include <yangplayer/YangPlayFactory.h>


YangPlayFactory::YangPlayFactory() {


}

YangPlayFactory::~YangPlayFactory() {

}

YangAudioPlay* YangPlayFactory::createAudioPlay(YangAudioInfo *pcontext){
#if !Yang_OS_WIN
	return NULL;//if(pcontext->audioPlayType==1) return new YangAudioPlayAlsa(pcontext);
#endif
	return NULL;// new YangAudioPlaySdl(pcontext);
}

YangAudioPlay *YangPlayFactory::createAudioPlay(YangAudioPlayType paet,YangAudioInfo *pcontext){
#if !Yang_OS_WIN
	return NULL;//if(paet==Yang_AP_ALSA) return new YangAudioPlayAlsa(pcontext);
#endif
	return NULL;//new YangAudioPlaySdl(pcontext);
}
