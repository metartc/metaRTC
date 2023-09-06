//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGTHREAD_H
#define YANGTHREAD_H
#include <QThread>
#include <QVector>

#include "YangPlayWidget.h"
#include "YangYuvPlayWidget.h"
#include "yangplayer/YangWinPlayFactroy.h"
#include "yangutil/buffer/YangVideoBuffer.h"
#include "video/yangrecordvideowin.h"

class YangRecordThread : public QThread
{
public:
    YangRecordThread();
    virtual ~YangRecordThread();
     int32_t m_isLoop;
    void initPara(YangContext *par);

#if Yang_OS_APPLE
    YangYuvPlayWidget *m_video;
#else
    YangPlayWidget *m_video;
#endif
    YangVideoBuffer *m_videoBuffer;
    int32_t m_sid;
    void stopAll();
    int32_t showType;

private:
    void render();
    void closeAll();

    //void initPlaySdl(YangRecordThreadWin *pwin);
    int32_t m_isStart;
    YangColor m_bgColor;
    YangColor m_textColor;
    int32_t m_videoPlayNum;
    YangContext *m_para;
    YangFrame m_frame;

private:
    virtual void run();
};

#endif // YANGTHREAD_H
