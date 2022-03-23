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
    MetaPlayer();
    ~MetaPlayer();
    void init(ANativeWindow* pwindows);
    int startPlayer(std::string url);
    void stopPlayer();

    void startPlayerTask();
    YangYuvGl* m_gl=NULL;
private:
    void adjustVideoScaleResolution();//调整缩放后的视频宽高
    YangPlayerAndroid* m_player;

    pthread_t m_pid_player,m_pid_rtc;

    uint64_t file_size = 0;
    int totalFrames = 0;
    int video_width = 0;
    int video_height = 0;
    bool m_isloop;
    int scale_video_width;//视频宽需要是8(2的3次幂)的倍数，否则视频错乱，绿条等等
    int scale_video_height;//视频高最好是2的倍数
};


#endif //YUVPLAYER_YUVPLAYER_H
