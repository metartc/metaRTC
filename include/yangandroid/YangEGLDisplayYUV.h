//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YUVPLAYER_EGLDISPLAYYUV_H
#define YUVPLAYER_EGLDISPLAYYUV_H


#include <EGL/egl.h>
#include <yangandroid/YangGlobalContexts.h>

class YangEGLDisplayYUV {
public:
    YangEGLDisplayYUV(ANativeWindow * window, YangGlobalContexts *context);
    ~YangEGLDisplayYUV();
    int eglOpen();
    int eglClose();
    ANativeWindow * nativeWindow;
    YangGlobalContexts *global_context;
};


#endif //YUVPLAYER_EGLDISPLAYYUV_H
