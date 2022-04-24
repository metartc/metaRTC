//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YUVPLAYER_YUVPLAYER_H
#define YUVPLAYER_YUVPLAYER_H


#include <pthread.h>
#include <unistd.h>
#include <yangutil/sys/YangYuvGl.h>
#include "YangPlayerAndroid.h"
class MetaPlayer {
public:
    MetaPlayer(YangContext* pcontext);
    ~MetaPlayer();
    void init(ANativeWindow* pwindows);
    int32_t startPlayer(std::string url);
    void stopPlayer();

    void startPlayerTask();
    YangYuvGl* m_gl;
private:
    void adjustVideoScaleResolution();//调整缩放后的视频宽高
    YangPlayerAndroid* m_player;
    bool m_isloop;
    pthread_t m_pid_player;
    int32_t m_video_width;
    int32_t m_video_height;

    int32_t scale_video_width;//视频宽需要是8(2的3次幂)的倍数，否则视频错乱，绿条等等
    int32_t scale_video_height;//视频高最好是2的倍数

    bool m_isSoftdecoder;
};


#endif //YUVPLAYER_YUVPLAYER_H
