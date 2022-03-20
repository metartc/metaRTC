//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGPLAYER_SRC_YANGAUDIOPLAYAndroid_H_
#define YANGPLAYER_SRC_YANGAUDIOPLAYAndroid_H_

#ifdef __ANDROID__

#include "YangAudioAndroid.h"


#include <yangaudiodev/YangAudioPlay.h>

class YangAudioPlayAndroid:public YangAudioPlay{
public:
	YangAudioPlayAndroid(YangContext *pcontext);
	~YangAudioPlayAndroid();

    int init();



protected:
	void startLoop();
	void stopLoop();

private:
	 int32_t m_loops;

private:

    int32_t m_contextt;
    YangAudioAndroid* m_audioAndroid;


	void closeAudio();
	int32_t ret;

};
#endif

#endif /* YANGPLAYER_SRC_YANGAUDIOPLAYALSA_H_ */
