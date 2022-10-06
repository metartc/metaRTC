//
// Copyright (c) 2019-2022 yanggaofeng
//
package com.metartc.push;

import android.content.res.AssetManager;
import android.view.Surface;


public class YangAndroidPush {
    static {
        System.loadLibrary("push");
    }


    private static volatile YangAndroidPush instance;

    private YangAndroidPush() {
    }

    public static YangAndroidPush getInstance() {
        if(null == instance) {
            synchronized (YangAndroidPush.class) {
                if(null == instance) {
                    instance = new YangAndroidPush();
                }
            }
        }
        return instance;
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native void setSurface(Surface surface,int width,int height,int fps);
    public native void setSurfaceSize(int width, int height);

    public native int startPush( String url);
    public native void stopPush();
    public native void releaseResources();
}
