//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangencoder/YangAudioEncoder.h>

YangAudioEncoder::YangAudioEncoder() {
	m_isInit=0;
	m_uid=-1;
	memset(&m_audioInfo,0,sizeof(YangAudioInfo));

}
YangAudioEncoder::~YangAudioEncoder(void) {

}

void YangAudioEncoder::setAudioPara(YangAudioInfo *pap){
	memcpy(&m_audioInfo,pap,sizeof(YangAudioInfo));
}


