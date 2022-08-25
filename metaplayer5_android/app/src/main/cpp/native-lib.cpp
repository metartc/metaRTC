//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <jni.h>
#include <string>
#include <pthread.h>
#include <android/native_window_jni.h> // 是为了 渲染到屏幕支持的
#include <android/asset_manager_jni.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangSocket.h>
#include <yangutil/sys/YangMath.h>
#include <yangandroid/MetaPlayer.h>
#include <android/log.h>
MetaPlayer *g_player = nullptr;
YangContext* g_context = nullptr;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void g_player_initContext(YangContext* context){
   // strcpy(context->avinfo.rtc.iceServerIP,"127.0.0.1");//stun server ip
   // context->avinfo.rtc.iceStunPort=3478;
   // context->avinfo.rtc.hasIceServer=0;
	context->streams.m_playBuffer=new YangSynBuffer();
	context->avinfo.rtc.enableDatachannel=yangfalse;
	context->avinfo.sys.mediaServer=Yang_Server_Srs;//Yang_Server_Srs/Yang_Server_Zlm

	//YangV_Hw_Android mediacodec
	//Yang_Hw_Soft yangh264decoder
	context->avinfo.video.videoDecHwType=Yang_Hw_Soft;

    context->avinfo.sys.rtcLocalPort=10000+yang_random()%15000;
    memset(context->avinfo.sys.localIp,0,sizeof(context->avinfo.sys.localIp));
    yang_getLocalInfo(context->avinfo.sys.localIp);

    context->avinfo.rtc.enableAudioBuffer=yangtrue;
    context->avinfo.audio.enableAec=yangfalse;

	yang_setLogLevel(5);
	yang_setLogFile(1);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_metartc_player_YangAndroidPlay_setSurface(JNIEnv *env, jobject, jobject surface) {
    // TODO: implement setSurface()
    yang_trace("setSurface in");
    pthread_mutex_lock(&mutex);
    if(g_context==NULL) {
        g_context=new YangContext();
        g_context->init();
        g_player_initContext(g_context);
    }
    if(g_player==NULL) g_player=new MetaPlayer(g_context);
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
 yang_delete(g_context);
    pthread_mutex_unlock(&mutex);
}
