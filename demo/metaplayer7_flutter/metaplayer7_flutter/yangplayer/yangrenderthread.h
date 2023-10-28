//
// Copyright (c) 2019-2023 yanggaofeng
//
#ifndef YANGTHREAD1_H
#define YANGTHREAD1_H


#include "yangutil/YangThread2.h"
#include "yang_player_app.h"
class YangRenderThread : public YangThread
{
public:
    YangRenderThread();
    virtual ~YangRenderThread();
    YangPlayerApp *m_player;
    int m_isLoop;
   // int m_width,m_height;
    void initPara();


    bool m_isRender;

    int m_sid;
    void stopAll();
  
   int m_isStart;
    int m_playWidth;
    int m_playHeight;

private:
    void render();
    void closeAll();

  
 


public:
    void stop();
protected:
     void run();
};

#endif // YANGTHREAD_H
