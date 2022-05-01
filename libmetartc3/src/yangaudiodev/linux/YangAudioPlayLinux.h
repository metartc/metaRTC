//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGPLAYER_SRC_YANGAUDIOPLAYALSA_H_
#define YANGPLAYER_SRC_YANGAUDIOPLAYALSA_H_

#ifndef _WIN32
#ifndef __ANDROID__
#include <alsa/asoundlib.h>


#include <yangaudiodev/YangAudioPlay.h>

class YangAudioPlayLinux:public YangAudioPlay{
public:
	YangAudioPlayLinux(YangContext *pcontext);
	~YangAudioPlayLinux();

    int init();



protected:
	void startLoop();
	void stopLoop();

private:
	 int32_t m_loops;

private:

    int32_t m_contextt;
	snd_pcm_t *m_handle;
	snd_pcm_uframes_t m_frames;


	void closeAudio();
	int32_t ret;

};
#endif
#endif
#endif /* YANGPLAYER_SRC_YANGAUDIOPLAYALSA_H_ */
