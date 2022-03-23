//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <jni.h>
#include <string>
#include <pthread.h>
#include <android/native_window_jni.h> // 是为了 渲染到屏幕支持的
#include <android/asset_manager_jni.h>
#include <yangutil/sys/YangLog.h>S
#include <yangandroid/MetaPlayer.h>
#include <android/log.h>
MetaPlayer *g_player = nullptr;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

extern "C"
JNIEXPORT void JNICALL
Java_com_metartc_player_YangAndroidPlay_setSurface(JNIEnv *env, jobject, jobject surface) {
    // TODO: implement setSurface()
    yang_trace("setSurface in");
    pthread_mutex_lock(&mutex);
    if(g_player==NULL) g_player=new MetaPlayer();
    g_player->init(ANativeWindow_fromSurface(env, surface));


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
    if(g_player==NULL) g_player=new MetaPlayer();
    g_player->m_gl->m_context.window_width = width;
    g_player->m_gl->m_context.window_height = height;

    pthread_mutex_unlock(&mutex);
    yang_trace("setSurfaceSize out");
}



extern "C"
JNIEXPORT int JNICALL
Java_com_metartc_player_YangAndroidPlay_startPlayer(JNIEnv *env, jobject, jstring purl) {
    // TODO: implement startPlayer()
    yang_trace("startPlayer in");
    int ret=1;
    const char *url = env->GetStringUTFChars(purl, nullptr);

    if(g_player==NULL) g_player=new MetaPlayer();

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

    pthread_mutex_unlock(&mutex);
}
