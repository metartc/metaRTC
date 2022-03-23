//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <jni.h>
#include <string>
#include <pthread.h>
#include <android/native_window_jni.h> // 是为了 渲染到屏幕支持的
#include <android/asset_manager_jni.h>
#include <yangutil/sys/YangLog.h>
#include <yangandroid/YangPushAndroid.h>
#include <android/log.h>
YangPushAndroid *g_push = nullptr;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

extern "C"
JNIEXPORT void JNICALL
Java_com_metartc_push_YangAndroidPush_setSurface(JNIEnv *env, jobject, jobject surface,jint width,jint height,jint fps) {
    // TODO: implement setSurface()
    yang_trace("setSurface in");
    pthread_mutex_lock(&mutex);
    if(g_push==NULL) g_push=new YangPushAndroid(ANativeWindow_fromSurface(env, surface));
    g_push->m_context->avinfo.video.width=width;
    g_push->m_context->avinfo.video.height=height;
    g_push->m_context->avinfo.video.outWidth=width;
    g_push->m_context->avinfo.video.outHeight=height;
    g_push->m_context->avinfo.video.frame=fps;
    g_push->startCamera();
    pthread_mutex_unlock(&mutex);
    yang_trace("setSurface out");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_metartc_push_YangAndroidPush_setSurfaceSize(JNIEnv *env, jobject, jint width,
                                                     jint height) {
    // TODO: implement setSurfaceSize()
    yang_trace("setSurfaceSize in");
    pthread_mutex_lock(&mutex);
    //g_push->m_gl->m_context.window_width = width;
    //g_push->m_gl->m_context.window_height = height;
    //if(g_push) g_push-
    pthread_mutex_unlock(&mutex);
    yang_trace("setSurfaceSize out");
}



extern "C"
JNIEXPORT int JNICALL
Java_com_metartc_push_YangAndroidPush_startPush(JNIEnv *env, jobject, jstring purl) {
    // TODO: implement startpush()
    yang_trace("startPush in");
    int ret=1;
    const char *url = env->GetStringUTFChars(purl, nullptr);
    if(g_push->startPush(url)==Yang_Ok){
        ret=0;
    }
    env->ReleaseStringUTFChars(purl,url);
    yang_trace("startPush out");
    return ret;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_metartc_push_YangAndroidPush_stopPush(JNIEnv *env, jobject) {
    // TODO: implement stopPush()
    if(nullptr != g_push) {
        g_push->stopPush();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_metartc_push_YangAndroidPush_releaseResources(JNIEnv *env, jobject) {
    // TODO: implement releaseResources()
    pthread_mutex_lock(&mutex);
    if(nullptr != g_push) {
        delete g_push;
        g_push = nullptr;
    }

    pthread_mutex_unlock(&mutex);
}
