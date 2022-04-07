//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGTHREAD_H
#define YANGTHREAD_H
#include <QThread>
#include <QVector>

#include "YangPlayWidget.h"
//#include "../yangutil/yangmeetingtype.h"
#include "yangrecliving/YangLivingType.h"
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
    YangPlayWidget *m_local_video,*m_remote_video;
    YangVideoBuffer *m_videoBuffer;
    int32_t m_sid;
    void stopAll();
    int32_t showType;
    YangSynBuffer* m_syn;
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

    uint8_t* m_playBuffer;
    int m_playWidth;
    int m_playHeight;

private:
    virtual void run();
};

#endif // YANGTHREAD_H
