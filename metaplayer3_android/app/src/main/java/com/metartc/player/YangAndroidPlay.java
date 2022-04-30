package com.metartc.player;

import android.content.res.AssetManager;
import android.view.Surface;

//
// Copyright (c) 2019-2022 yanggaofeng
//
public class YangAndroidPlay {
    static {
        System.loadLibrary("player");
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

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native void setSurface(Surface surface);
    public native void setSurfaceSize(int width, int height);
   // public native void saveAssetManager(AssetManager manager);
    public native int startPlayer( String url);
    public native void stopPlayer();
    public native void releaseResources();
}
