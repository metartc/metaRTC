package com.metartc.player;

import android.content.res.AssetManager;
import android.view.Surface;

//
// Copyright (c) 2019-2022 yanggaofeng
//
public class YangAndroidPlay {
    static {
        System.loadLibrary("metaplayer");
    }


    private static volatile YangAndroidPlay instance;

    private YangAndroidPlay() {
    }

    public static YangAndroidPlay getInstance() {
        if(null == instance) {
            synchronized (YangAndroidPlay.class) {
                if(null == instance) {
                    instance = new YangAndroidPlay();
                }
            }
        }
        return instance;
    }


    //srs:0 zlm:1
    public native long createPlayer(int isSoftDecoder,int mediaServer);
    public native void setDecoder(long context,int decoderType);
    public native void setMediaserver(long context,int mediaServer);

    public native void setSurface(long context,Surface surface);
    public native void setSurfaceSize(long context,int width, int height);

    public native int startPlayer(long context, String url);
    public native void stopPlayer(long context);
    public native void releaseResources(long context);

}
