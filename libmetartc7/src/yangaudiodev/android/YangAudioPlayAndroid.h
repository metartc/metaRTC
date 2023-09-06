//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGPLAYER_SRC_YANGAUDIOPLAYAndroid_H_
#define YANGPLAYER_SRC_YANGAUDIOPLAYAndroid_H_
#include <yangaudiodev/YangAudioPlay.h>
#if Yang_OS_ANDROID

#include "YangAudioAndroid.h"

class YangAudioPlayAndroid:public YangAudioPlay{
public:
	YangAudioPlayAndroid(YangAVInfo* avinfo,YangSynBufferManager* streams);
	~YangAudioPlayAndroid();
    int init();

protected:
	void startLoop();
	void stopLoop();

private:
	void closeAudio();

private:
	int32_t m_loops;
    int32_t m_isInit;
    YangAudioAndroid* m_audioAndroid;

};
#endif

#endif /* YANGPLAYER_SRC_YANGAUDIOPLAYALSA_H_ */
