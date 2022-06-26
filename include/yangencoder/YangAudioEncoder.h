//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef ___YangAudioEncoderPipe__
#define ___YangAudioEncoderPipe__
#include <yangutil/yangavinfotype.h>
#include "yangutil/buffer/YangAudioBuffer.h"
#include "yangutil/buffer/YangAudioEncoderBuffer.h"
#include "yangutil/sys/YangIni.h"
#include "yangutil/sys/YangThread.h"
#include "YangEncoder.h"

class YangAudioEncoder
{
public:
	YangAudioEncoder();
	virtual ~YangAudioEncoder(void);
    virtual void init(YangAudioInfo *pap)=0;
    virtual int32_t encoder(YangFrame* pframe,YangEncoderCallback* pcallback)=0;
    void stop();
    int32_t m_uid;

protected:
    void setAudioPara(YangAudioInfo *audioInfo);
	int32_t m_isInit;
	YangAudioInfo m_audioInfo;

};

#endif
