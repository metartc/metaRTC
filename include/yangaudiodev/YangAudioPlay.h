
//
// Copyright (c) 2019-2023 yanggaofeng
//
#ifndef YangAudioPlay_H
#define YangAudioPlay_H

#include <yangaudiodev/YangAudioRenderData.h>
#include <yangavutil/audio/YangRtcAec.h>
#include <yangutil/yangavinfotype.h>
#include "yangutil/buffer/YangAudioPlayBuffer.h"

#include <yangutil/sys/YangThread2.h>
#define YangAudioBufferMaxNum 10

#include<algorithm>
#include <vector>

using namespace std;

class YangAudioPlay:public YangThread
{
    public:
        YangAudioPlay(YangAVInfo* avinfo,YangSynBufferManager* streams);
        virtual ~YangAudioPlay();
    public:

        int32_t aIndex;
        virtual int init()=0;
        void setAudioBuffers(vector<YangAudioPlayBuffer*> *paudioList);
        void setAudioBuffer(YangAudioPlayBuffer *paudioList);
        void setAecBase(YangRtcAec* pace);
        int32_t m_aecInit=0;
        int32_t m_isStart;
        void stop();
        YangAudioRenderData m_audioData;
    protected:
        virtual void startLoop()=0;
        virtual void stopLoop()=0;

        void run();
        YangAVInfo *m_avinfo;
        YangRtcAec *m_ace;
        int	m_frames;
        int	m_channel;
        int	m_sample;

    };

#endif // YANGAUDIOCAPTURE_H
