package com.metartc.player;

import android.view.Surface;

public class YangPlayer {
    private long  m_playPointer=0;
    public  static final int Decoder_cpu=1;
    public  static final int Decoder_gpu=0;
    public  static final int MediaServer_srs=0;
    public  static final int MediaServer_zlm=1;
    //
    public YangPlayer(int decoder,int mediaServer){
        m_playPointer=YangAndroidPlay.getInstance().createPlayer(decoder,mediaServer);
    }
    public void setDecoder(int decoderType){
        YangAndroidPlay.getInstance().setDecoder(m_playPointer,decoderType);
    }
    public void setMediaserver(int mediaServer){
        YangAndroidPlay.getInstance().setMediaserver(m_playPointer,mediaServer);
    }
    public  void setSurface(Surface surface){
        YangAndroidPlay.getInstance().setSurface(m_playPointer,surface);
    }
    public  void setSurfaceSize(int width, int height){
        YangAndroidPlay.getInstance().setSurfaceSize(m_playPointer,width,height);
    }

    public  int startPlayer(String url){
        return YangAndroidPlay.getInstance().startPlayer(m_playPointer,url);
    }
    public  void stopPlayer(){
        YangAndroidPlay.getInstance().stopPlayer(m_playPointer);
    }
    public  void releaseResources(){
        YangAndroidPlay.getInstance().releaseResources(m_playPointer);
    }

}
