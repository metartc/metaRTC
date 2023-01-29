//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YUVPLAYER_YUVPLAYER_H
#define YUVPLAYER_YUVPLAYER_H



#include <yangutil/sys/YangThread.h>
#include <yangutil/sys/YangYuvGl.h>
#include "YangPlayerAndroid.h"
#include <unistd.h>
#include <string>
class MetaPlayer {
public:
    MetaPlayer(YangContext* pcontext);
    ~MetaPlayer();
    void init(ANativeWindow* pwindows);
    int32_t startPlayer(std::string url);
    void stopPlayer();

    void startPlayerTask();
    YangContext* m_context;
    YangYuvGl* m_gl;
private:
    void adjustVideoScaleResolution();//调整缩放后的视频宽高
    YangPlayerAndroid* m_player;
    bool m_isloop;
    yang_thread_t m_pid_player;
    int32_t m_width;
    int32_t m_height;

    int32_t m_render_width;
    int32_t m_render_height;

    bool m_isSoftdecoder;
};


#endif //YUVPLAYER_YUVPLAYER_H
