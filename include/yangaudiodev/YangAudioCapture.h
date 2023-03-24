//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGAUDIOCAPTURE_H
#define YANGAUDIOCAPTURE_H

#include <yangavutil/audio/YangRtcAec.h>
#include <yangavutil/audio/YangPreProcess.h>
#include <yangutil/sys/YangThread2.h>
#include <yangutil/yangavinfotype.h>
#include "yangutil/buffer/YangAudioPlayBuffer.h"
#include "yangutil/buffer/YangAudioBuffer.h"
#include <vector>
#include "yangutil/sys/YangLog.h"
using namespace std;

class YangAudioCapture:public YangThread
{
    public:
        YangAudioCapture();
        virtual ~YangAudioCapture();
    public:

        int32_t aIndex;
        yangbool m_isStart;
        virtual int32_t init()=0;
        virtual void setCatureStart()=0;
        virtual void setCatureStop()=0;
        virtual void setOutAudioBuffer(YangAudioBuffer *pbuffer)=0;
        virtual void setPlayAudoBuffer(YangAudioBuffer *pbuffer)=0;
        virtual void setAec(YangRtcAec *paec)=0;
        virtual void setInAudioBuffer(vector<YangAudioPlayBuffer*> *pbuffer)=0;
        virtual void setPreProcess(YangPreProcess *pp)=0;

        void stop();
    protected:
        void run();
        YangContext *m_context;
        virtual void startLoop()=0;
        virtual  void stopLoop()=0;

};

#endif // YANGAUDIOCAPTURE_H
