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

void g_player_initContext(YangContext* context,yangbool isSoftDecoder,int mediaServer){
    // strcpy(context->avinfo.rtc.iceServerIP,"127.0.0.1");//stun server ip
    // context->avinfo.rtc.iceStunPort=3478;
    // context->avinfo.rtc.hasIceServer=0;
    context->streams.m_playBuffer=new YangSynBuffer();
    context->avinfo.rtc.enableDatachannel=yangfalse;
    context->avinfo.sys.mediaServer=mediaServer;//Yang_Server_Srs/Yang_Server_Zlm

    //YangV_Hw_Android mediacodec
    //Yang_Hw_Soft yangh264decoder
    context->avinfo.video.videoDecHwType=isSoftDecoder?Yang_Hw_Soft:YangV_Hw_Android;
    if(context->avinfo.video.videoDecHwType==YangV_Hw_Android){
        context->avinfo.video.videoDecoderFormat=YangNv12;
    }

    context->avinfo.sys.rtcLocalPort=10000+yang_random()%15000;
    memset(context->avinfo.sys.localIp,0,sizeof(context->avinfo.sys.localIp));
    yang_getLocalInfo(context->avinfo.sys.familyType,context->avinfo.sys.localIp);

    context->avinfo.rtc.enableAudioBuffer=yangtrue;
    context->avinfo.audio.enableAec=yangfalse;

    yang_setLogLevel(5);
    yang_setLogFile(1);
}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_metartc_player_YangAndroidPlay_createPlayer(JNIEnv *env, jobject,jint isSoftDecoder,jint mediaServer){
    YangContext* context=new YangContext();
    context->init();
    g_player_initContext(context,isSoftDecoder,mediaServer);

    MetaPlayer* player=new MetaPlayer(context);
    return (long)player;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_metartc_player_YangAndroidPlay_setDecoder(JNIEnv *env, jobject, jlong context,jint decoder_type) {
    // TODO: implement setDecoder()
    if(context==0) return;
    MetaPlayer* player= (MetaPlayer*)context;
    player->m_context->avinfo.video.videoDecHwType=(decoder_type?Yang_Hw_Soft:YangV_Hw_Android);
    if(player->m_context->avinfo.video.videoDecHwType==YangV_Hw_Android){
        player->m_context->avinfo.video.videoDecoderFormat=YangNv12;
    }else{
        player->m_context->avinfo.video.videoDecoderFormat=YangI420;
    }
   // if(player->m_gl) player->m_gl->setColor(player->m_context->avinfo.video.videoDecoderFormat);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_metartc_player_YangAndroidPlay_setMediaserver(JNIEnv *env, jobject,jlong context, jint media_server) {
    // TODO: implement setMediaserver()
    if(context==0) return;
    MetaPlayer* player= (MetaPlayer*)context;
    player->m_context->avinfo.sys.mediaServer=media_server;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_metartc_player_YangAndroidPlay_setSurface(JNIEnv *env, jobject, jlong context,jobject surface) {
    // TODO: implement setSurface()
    yang_trace("setSurface in");

    MetaPlayer* player= (MetaPlayer*)context;
    if(player) player->init(ANativeWindow_fromSurface(env, surface));

    yang_trace("setSurface out");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_metartc_player_YangAndroidPlay_setSurfaceSize(JNIEnv *env, jobject, jlong context,jint width,
                                                       jint height) {
    // TODO: implement setSurfaceSize()
    yang_trace("setSurfaceSize in");
    MetaPlayer* player= (MetaPlayer*)context;
    if(player==NULL) return;
    player->m_gl->m_context.window_width = width;
    player->m_gl->m_context.window_height = height;


    yang_trace("setSurfaceSize out");
}



extern "C"
JNIEXPORT int JNICALL
Java_com_metartc_player_YangAndroidPlay_startPlayer(JNIEnv *env, jobject, jlong context,jstring purl) {
    // TODO: implement startPlayer()
    yang_trace("startPlayer in");
    int ret=1;
    const char *url = env->GetStringUTFChars(purl, nullptr);
    MetaPlayer* player= (MetaPlayer*)context;
    if(player&&player->startPlayer(url)==Yang_Ok){
        ret=0;
    }
    env->ReleaseStringUTFChars(purl,url);
    yang_trace("startPlayer out");
    return ret;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_metartc_player_YangAndroidPlay_stopPlayer(JNIEnv *env, jobject,jlong context) {
    // TODO: implement stopPlayer()
    MetaPlayer* player= (MetaPlayer*)context;
    if(nullptr != player) {
        player->stopPlayer();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_metartc_player_YangAndroidPlay_releaseResources(JNIEnv *env, jobject,jlong context) {
    // TODO: implement releaseResources()

    MetaPlayer* player= (MetaPlayer*)context;
    if(nullptr != player) {
        delete player;
        player = nullptr;
    }


}

