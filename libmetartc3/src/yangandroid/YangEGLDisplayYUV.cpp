//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangandroid/YangEGLDisplayYUV.h>
#include <yangutil/sys/YangLog.h>
YangEGLDisplayYUV::YangEGLDisplayYUV(ANativeWindow *window, YangGlobalContexts *context) {
    this->nativeWindow = window;
    this->global_context = context;
}

YangEGLDisplayYUV::~YangEGLDisplayYUV() {

}

int YangEGLDisplayYUV::eglOpen() {
    EGLDisplay eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (eglDisplay == EGL_NO_DISPLAY ) {
        yang_trace("eglGetDisplay failure : %d", eglGetError());
        return -1;
    }
    yang_trace("eglGetDisplay ok");
    this->global_context->eglDisplay = eglDisplay;

    EGLint majorVersion;//主版本号
    EGLint minorVersion;//次版本号
    EGLBoolean success = eglInitialize(eglDisplay, &majorVersion,
                                       &minorVersion);
    if (!success) {
        yang_trace("eglInitialize failure: %d", eglGetError());
        return -1;
    }
    yang_trace("eglInitialize ok");

    EGLint numConfigs;
    EGLConfig config;
    const EGLint CONFIG_ATTRIBS[] = { EGL_RED_SIZE, 8,
                                      EGL_GREEN_SIZE, 8,
                                      EGL_BLUE_SIZE, 8,
                                      EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                                      EGL_NONE // the end
    };

    success = eglChooseConfig(eglDisplay, CONFIG_ATTRIBS, &config, 1, &numConfigs);
    if (!success) {
        yang_trace("eglChooseConfig failure: %d", eglGetError());
        return -1;
    }
    yang_trace("eglChooseConfig ok");

    EGLSurface eglSurface = eglCreateWindowSurface(eglDisplay, config,
                                                   this->nativeWindow, 0);
    if (EGL_NO_SURFACE == eglSurface) {
        yang_trace("eglCreateWindowSurface failure: %d", eglGetError());
        return -1;
    }

    yang_trace("eglCreateWindowSurface ok");
    this->global_context->eglSurface = eglSurface;

    const EGLint attribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
    EGLContext elgContext = eglCreateContext(eglDisplay, config, EGL_NO_CONTEXT,
                                             attribs);
    if (elgContext == EGL_NO_CONTEXT ) {
        yang_trace("eglCreateContext failure, error is %d", eglGetError());
        return -1;
    }
    yang_trace("eglCreateContext ok");
    this->global_context->eglContext = elgContext;
    return 0;
}

int YangEGLDisplayYUV::eglClose() {
    EGLBoolean success = eglDestroySurface(this->global_context->eglDisplay, this->global_context->eglSurface);
    if (!success) {
        yang_trace("eglDestroySurface failure.");
    }

    success = eglDestroyContext(this->global_context->eglDisplay, this->global_context->eglContext);
    if (!success) {
        yang_trace("eglDestroyContext failure.");
    }

    success = eglTerminate(this->global_context->eglDisplay);
    if (!success) {
        yang_trace("eglTerminate failure.");
    }

    this->global_context->eglSurface = NULL;
    this->global_context->eglContext = NULL;
    this->global_context->eglDisplay = NULL;
    return 0;
}
