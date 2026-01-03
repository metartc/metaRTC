//
// Copyright (c) 2019-2026 yanggaofeng
//
#ifndef YANGPLAYER_SRC_YANGAUDIOPLAYMac_H_
#define YANGPLAYER_SRC_YANGAUDIOPLAYMac_H_

#include <yangaudiodev/YangAudioPlay.h>

#if Yang_OS_APPLE
#include <yangaudiodev/mac/YangAudioMac.h>


class YangAudioPlayMac:public YangAudioPlay{
public:
	YangAudioPlayMac(YangAVInfo* avinfo,YangSynBufferManager* streams);
	~YangAudioPlayMac();

    int32_t init();
    YangFrame* on_audio();

    void start();
protected:
	void startLoop();
	void stopLoop();
    void closeAudio();

private:
   //  yangbool m_loops;
    // yangbool m_isStart;
     yangbool m_isInited;

private:

	int32_t audioLen;
    int32_t m_channel;
    uint32_t  m_sample;
    uint32_t m_audiolen;

    YangAudioMac* m_macAudio;
    YangMacAudioCallback m_callback;

    YangFrame m_audioFrame;

};
#endif

#endif /* YANGPLAYER_SRC_YANGAUDIOPLAYALSA_H_ */
