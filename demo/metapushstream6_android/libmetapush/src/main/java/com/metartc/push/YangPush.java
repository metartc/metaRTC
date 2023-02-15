package com.metartc.push;

import android.view.Surface;

public class YangPush {
    private long  m_pushPointer=0;
    public  static final int Encoder_cpu=1;
    public  static final int Encoder_gpu=0;
    public  static final int MediaServer_srs=0;
    public  static final int MediaServer_zlm=1;
    public static final int Camera_Front=0;
    public static final int Camera_Back=1;
    public YangPush(int cameraIndex,int encoder,int mediaServer){
        m_pushPointer=YangAndroidPush.getInstance().createPusher(cameraIndex,encoder,mediaServer);
    }
    public  void setSurface( Surface surface, int width, int height, int fps){
        YangAndroidPush.getInstance().setSurface(m_pushPointer,surface,width,height,fps);
    }
    public  void setSurfaceSize(int width, int height){
        YangAndroidPush.getInstance().setSurfaceSize(m_pushPointer,width,height);
    }

    public  int startPush( String url){
        return YangAndroidPush.getInstance().startPush(m_pushPointer,url);
    }
    public  void stopPush(){
        YangAndroidPush.getInstance().stopPush(m_pushPointer);
    }
    public  void releaseResources(){
        YangAndroidPush.getInstance().releaseResources(m_pushPointer);
    }


}
