//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGPLAYER_SRC_YANGAUDIOPLAYALSA_H_
#define YANGPLAYER_SRC_YANGAUDIOPLAYALSA_H_

#include <yangaudiodev/YangAudioPlay.h>

#if Yang_OS_LINUX

#include <alsa/asoundlib.h>

class YangAudioPlayLinux:public YangAudioPlay{
public:
	YangAudioPlayLinux(YangAVInfo* avinfo,YangSynBufferManager* streams);
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

#endif /* YANGPLAYER_SRC_YANGAUDIOPLAYALSA_H_ */
