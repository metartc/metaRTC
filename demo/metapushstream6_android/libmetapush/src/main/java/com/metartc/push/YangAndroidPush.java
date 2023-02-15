//
// Copyright (c) 2019-2022 yanggaofeng
//
package com.metartc.push;

import android.view.Surface;


public class YangAndroidPush {
    static {
        System.loadLibrary("metapush");
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


    //srs:0 zlm:1
    public native long createPusher(int cameraIndex,int isSoftEncoder,int mediaServer);
    public native void setSurface(long context,Surface surface,int width,int height,int fps);
    public native void setSurfaceSize(long context,int width, int height);

    public native int startPush(long context, String url);
    public native void stopPush(long context);
    public native void releaseResources(long context);
}
