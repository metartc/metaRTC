//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <jni.h>
#include <string>
#include <pthread.h>
#include <android/native_window_jni.h> // 是为了 渲染到屏幕支持的
#include <android/asset_manager_jni.h>
#include <yangutil/sys/YangLog.h>

#include <yangandroid/YangGlobalContexts.h>
#include <yangandroid/MetaPlayer.h>
#include <android/log.h>
MetaPlayer *g_player = nullptr;
ANativeWindow * g_nativeWindow = nullptr;
YangGlobalContexts *g_gl_player_context = nullptr;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

extern "C"
JNIEXPORT void JNICALL
Java_com_metartc_player_YangAndroidPlay_setSurface(JNIEnv *env, jobject, jobject surface) {
    // TODO: implement setSurface()
    yang_trace("setSurface in");
    pthread_mutex_lock(&mutex);

    if (g_nativeWindow) {
        ANativeWindow_release(g_nativeWindow);
        g_nativeWindow = nullptr;
    }

    // 创建新的窗口用于视频显示
    g_nativeWindow = ANativeWindow_fromSurface(env, surface);
    if(nullptr == g_gl_player_context) {
        g_gl_player_context = new YangGlobalContexts();
    }
    g_gl_player_context->nativeWindow = g_nativeWindow;
    pthread_mutex_unlock(&mutex);
    yang_trace("setSurface out");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_metartc_player_YangAndroidPlay_setSurfaceSize(JNIEnv *env, jobject, jint width,
                                                       jint height) {
    // TODO: implement setSurfaceSize()
    yang_trace("setSurfaceSize in");
    pthread_mutex_lock(&mutex);
    if(nullptr == g_gl_player_context) {
        g_gl_player_context = new YangGlobalContexts();
    }
    g_gl_player_context->gl_window_width = width;
    g_gl_player_context->gl_window_height = height;
    if(g_player) g_player-
    pthread_mutex_unlock(&mutex);
    yang_trace("setSurfaceSize out");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_metartc_player_YangAndroidPlay_saveAssetManager(JNIEnv *env, jobject,
                                                         jobject manager) {
    // TODO: implement saveAssetManager()
    yang_trace("saveAssetManager in");
    pthread_mutex_lock(&mutex);
    AAssetManager *mgr = AAssetManager_fromJava(env, manager);
    if(nullptr == g_gl_player_context) {
        g_gl_player_context = new YangGlobalContexts();
    }
    g_gl_player_context->assetManager = mgr;
    pthread_mutex_unlock(&mutex);
    yang_trace("saveAssetManager out");
}

extern "C"
JNIEXPORT int JNICALL
Java_com_metartc_player_YangAndroidPlay_startPlayer(JNIEnv *env, jobject, jstring purl) {
    // TODO: implement startPlayer()
    yang_trace("startPlayer in");
    int ret=1;
    const char *url = env->GetStringUTFChars(purl, nullptr);
    if(nullptr == g_gl_player_context) {
        g_gl_player_context = new YangGlobalContexts();
    }



    if(g_player==nullptr) {

        g_player = new MetaPlayer(g_gl_player_context);
    }
    if(g_player->startPlayer(url)==Yang_Ok){
        ret=0;
    }
    env->ReleaseStringUTFChars(purl,url);
    yang_trace("startPlayer out");
    return ret;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_metartc_player_YangAndroidPlay_stopPlayer(JNIEnv *env, jobject) {
    // TODO: implement stopPlayer()
    if(nullptr != g_player) {
        g_player->stopPlayer();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_metartc_player_YangAndroidPlay_releaseResources(JNIEnv *env, jobject) {
    // TODO: implement releaseResources()
    pthread_mutex_lock(&mutex);
    if(nullptr != g_player) {
        delete g_player;
        g_player = nullptr;
    }
    if(nullptr != g_gl_player_context) {
        delete g_gl_player_context;
        g_gl_player_context = nullptr;
    }
    pthread_mutex_unlock(&mutex);
}
