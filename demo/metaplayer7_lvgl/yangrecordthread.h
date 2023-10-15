//
// Copyright (c) 2019-2023 yanggaofeng
//
#ifndef YANGTHREAD1_H
#define YANGTHREAD1_H


#include <yangutil/buffer/YangVideoBuffer.h>
#include <yangutil/sys/YangThread2.h>

#include <yangstream/YangSynBuffer.h>

class YangRecordThread : public YangThread
{
public:
    YangRecordThread();
    virtual ~YangRecordThread();

    int m_isLoop;
    void initPara();

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

public:
    void stop();
protected:
     void run();
};

#endif // YANGTHREAD_H
