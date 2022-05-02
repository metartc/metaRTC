//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGCAPTURE_WIN_API_YANGAUDIOAPICAPTURE_H_
#define SRC_YANGCAPTURE_WIN_API_YANGAUDIOAPICAPTURE_H_
#include <yangaudiodev/YangCaptureCallback.h>
#include <yangutil/sys/YangThread.h>
#include <yangutil/yangavinfotype.h>

class YangAudioApiCapture:public YangThread{
public:
	YangAudioApiCapture(){m_isStart=0;};
	virtual ~YangAudioApiCapture(){};
	virtual void setCaptureCallback(YangCaptureCallback* cb)=0;
    virtual int initCapture()=0;
    virtual int startCpature()=0;
    virtual int stopCapture()=0;
    virtual void captureThread()=0;
    virtual int getAudioOutLength()=0;
    int m_isStart;
};



#endif /* SRC_YANGCAPTURE_WIN_API_YANGAUDIOAPICAPTURE_H_ */
