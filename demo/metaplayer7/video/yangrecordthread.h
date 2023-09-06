//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGTHREAD1_H
#define YANGTHREAD1_H
#include <QThread>
#include <QVector>

#include <yangutil/buffer/YangVideoBuffer.h>
#include <yangplayer/YangYuvPlayWidget.h>
#include <yangplayer/YangPlayWidget.h>
#include <yangstream/YangSynBuffer.h>

class YangRecordThread : public QThread
{
public:
    YangRecordThread();
    virtual ~YangRecordThread();

    int m_isLoop;
    void initPara();


#if Yang_OS_APPLE
    YangYuvPlayWidget *m_video;
#else
    YangPlayWidget *m_video;
#endif

    YangSynBuffer* m_syn;
    bool m_isRender;

    int m_sid;
    void stopAll();
    int showType;

    void setBlackBk();

private:
    void render();
    void closeAll();

    YangFrame m_frame;
    int m_isStart;
    int m_playWidth;
    int m_playHeight;
    uint8_t* m_playBuffer;

private:
    virtual void run();
};

#endif // YANGTHREAD_H
