//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGPLAYER_SRC_YangAudioPlayAndroid_H_
#define YANGPLAYER_SRC_YangAudioPlayAndroid_H_

#ifndef _WIN32
#ifdef __ANDROID__

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

    void closeAudio();

};
#endif
#endif
#endif /* YANGPLAYER_SRC_YANGAUDIOPLAYALSA_H_ */
