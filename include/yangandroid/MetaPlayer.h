//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YUVPLAYER_YUVPLAYER_H
#define YUVPLAYER_YUVPLAYER_H


#include <pthread.h>
#include <unistd.h>
#include <yangandroid/YangEGLDisplayYUV.h>
#include <yangandroid/YangGlobalContexts.h>
#include <yangandroid/YangShaderRGBA.h>
#include <yangandroid/YangShaderYUV.h>
#include "YangPlayerAndroid.h"


//支持两种方式渲染YUV数据, 第一种：yuv直接传给Shader，yuv转rgb在GPU完成
//对应函数：startYUVPlayerTask()
//支持两种方式渲染YUV数据, 第二种：yuv数据转rgba，再传给Shader
//对应函数：startRGBAPlayerTask()
class MetaPlayer {
public:
    MetaPlayer(YangGlobalContexts *global_context);
    ~MetaPlayer();
    int startPlayer(std::string url);
    void stopPlayer();
    void startRGBAPlayerTask();
    void startYUVPlayerTask();
private:
    void adjustVideoScaleResolution();//调整缩放后的视频宽高
    YangPlayerAndroid* m_player=NULL;
    YangGlobalContexts *global_context = NULL;
    YangEGLDisplayYUV *eglDisplayYuv = NULL;
    YangShaderYUV * shaderYuv = NULL;
    YangShaderRGBA * shaderRgba = NULL;

   // char *data_source = NULL;
    pthread_t m_pid_player,m_pid_rtc;

    //uint8_t *frame_data;// = {NULL};
   // uint8_t *scale_frame_data;
    //uint8_t *rgba_data = NULL;

    uint64_t file_size = 0;
    int totalFrames = 0;
    int video_width = 0;
    int video_height = 0;
    bool m_isloop;
    int scale_video_width;//视频宽需要是8(2的3次幂)的倍数，否则视频错乱，绿条等等
    int scale_video_height;//视频高最好是2的倍数
};


#endif //YUVPLAYER_YUVPLAYER_H
