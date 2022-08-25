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
#include <yangandroid/YangPushAndroid.h>
#include <android/log.h>
YangPushAndroid *g_push = nullptr;
YangContext* g_context = nullptr;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void g_push_initContext(YangContext* context){

	context->avinfo.sys.mediaServer=Yang_Server_Srs;//Yang_Server_Srs/Yang_Server_Zlm
	context->avinfo.video.videoEncoderType=Yang_VED_264; //h264

	//YangI420 YangYv12 YangNv12 YangNv21//根据机型mediacodec 需要nv12
	context->avinfo.video.videoEncoderFormat=YangI420;
 	//YangV_Hw_Android mediacodec
	//Yang_Hw_Soft libx264
	context->avinfo.video.videoEncHwType=Yang_Hw_Soft;
	//Yang_Rotate0 Yang_Rotate90 Yang_Rotate180 Yang_Rotate270
	context->avinfo.video.rotate=Yang_Rotate0;


    context->avinfo.audio.enableMono=yangfalse;
    context->avinfo.audio.sample=48000;
    context->avinfo.audio.channel=2;
    context->avinfo.audio.enableAudioFec=yangfalse;
    context->avinfo.audio.audioCacheNum=8;
    context->avinfo.audio.audioCacheSize=8;
    context->avinfo.audio.audioPlayCacheNum=8;

    context->avinfo.video.rate=2048;//码率默认为2048K
    context->avinfo.video.frame=30;//默认30帧
    context->avinfo.video.bitDepth=8;//默认8位，10,12,16
    context->avinfo.video.videoCaptureFormat=YangI420;
    context->avinfo.video.videoCacheNum=10;
    context->avinfo.video.evideoCacheNum=10;
    context->avinfo.video.videoPlayCacheNum=10;

    context->avinfo.audio.audioEncoderType=Yang_AED_OPUS;
    context->avinfo.sys.rtcLocalPort=10000+yang_random()%15000;
    memset(context->avinfo.sys.localIp,0,sizeof(context->avinfo.sys.localIp));
    yang_getLocalInfo(context->avinfo.sys.localIp);
    context->avinfo.enc.enc_threads=4; //x264编码线程数
    context->avinfo.enc.createMeta=yangfalse;

    context->avinfo.rtc.enableDatachannel=yangfalse;

    context->avinfo.rtc.enableAudioBuffer=yangtrue;
    context->avinfo.audio.enableAec=yangfalse;

    yang_setLogLevel(5);
	yang_setLogFile(1);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_metartc_push_YangAndroidPush_setSurface(JNIEnv *env, jobject, jobject surface,jint width,jint height,jint fps) {
    // TODO: implement setSurface()
    yang_trace("setSurface in");
    pthread_mutex_lock(&mutex);
    if(g_context==NULL) {
		g_context=new YangContext();
		g_context->init();
		g_push_initContext(g_context);
	}
    if(g_push==NULL) g_push=new YangPushAndroid(g_context,ANativeWindow_fromSurface(env, surface));
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
	yang_delete(g_context);
    pthread_mutex_unlock(&mutex);
}
