//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGPLAYER_SRC_YANGAUDIOPLAYMac_H_
#define YANGPLAYER_SRC_YANGAUDIOPLAYMac_H_

#include <yangaudiodev/YangAudioPlay.h>

#if defined(__APPLE__)



class YangAudioPlayMac:public YangAudioPlay{
public:
	YangAudioPlayMac(YangAVInfo* avinfo,YangSynBufferManager* streams);
	~YangAudioPlayMac();

    int init();



protected:
	void startLoop();
	void stopLoop();

private:
	 int32_t m_loops;

private:

    int32_t m_contextt;

    int32_t m_frames;


	void closeAudio();
	int32_t ret;

};
#endif

#endif /* YANGPLAYER_SRC_YANGAUDIOPLAYALSA_H_ */
