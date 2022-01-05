#ifndef YangAudioPlay_H
#define YangAudioPlay_H
//#include "yangutil/UtilPlay.h"
#include <yangaudiodev/YangAudioRenderData.h>
#include <yangavutil/audio/YangAecBase.h>
#include <yangutil/yangavinfotype.h>
#include "yangutil/buffer/YangAudioPlayBuffer.h"
#include "yangutil/sys/YangIni.h"
#include "yangutil/sys/YangThread.h"
#define YangAudioBufferMaxNum 10

#include<algorithm>
#include <vector>

using namespace std;
//#define SIZE_AUDIO_FRAME 4096



class YangAudioPlay:public YangThread
{
    public:
        YangAudioPlay(YangContext* pcontext);
        virtual ~YangAudioPlay();
    public:

        int32_t aIndex;
        virtual int init()=0;
        void setAudioBuffers(vector<YangAudioPlayBuffer*> *paudioList);
        void setAudioBuffer(YangAudioPlayBuffer *paudioList);
        void setAecBase(YangAecBase* pace);
        int32_t m_aecInit=0;
        int32_t m_isStart;
        void stop();
        YangAudioRenderData m_audioData;
    protected:
        virtual void startLoop()=0;
        virtual void stopLoop()=0;

        void run();
        YangContext *m_context;
        YangAecBase *m_ace;
        YangAudioMix mix;
        int	m_frames;
        int	m_channel;
        int	m_sample;




    private:




    };

#endif // YANGAUDIOCAPTURE_H
