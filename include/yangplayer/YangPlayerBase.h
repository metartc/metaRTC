//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef MeetingPlayoBase_H
#define MeetingPlayoBase_H
#include <yangplayer/YangPlayerDecoder.h>
#include <yangplayer/YangPlayerPlay.h>
#include <vector>

#include "yangutil/buffer/YangAudioEncoderBuffer.h"
#include "yangutil/buffer/YangAudioPlayBuffer.h"
#include "yangutil/buffer/YangVideoDecoderBuffer.h"

using namespace std;
class YangPlayerBase
{
    public:
        YangPlayerBase();
        virtual ~YangPlayerBase();
        YangPlayerDecoder *m_ydb;
        YangPlayerPlay *m_ypb;

    void startAudioDecoder(YangAudioEncoderBuffer *prr);
    void startVideoDecoder(YangVideoDecoderBuffer *prr);
    void init(YangContext* audio);
    void startAudioPlay(YangContext* paudio);
    void stopAll();
    protected:

    private:

};

#endif // ZBBASE_H
