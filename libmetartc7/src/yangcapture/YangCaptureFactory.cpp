//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangcapture/YangCaptureFactory.h>

#if Yang_OS_WIN
#include <yangaudiodev/win/YangWinAudioCapture.h>
#include <yangaudiodev/win/YangWinAudioApiDevice.h>
#include <yangaudiodev/win/YangAudioCaptureWindows.h>
#include "win/YangVideoCaptureWindows.h"
#else
#if Yang_OS_ANDROID
#include <yangaudiodev/android/YangAudioCaptureAndroid.h>
#include <yangcapture/android/YangVideoCaptureAndroid.h>
#else
#include <yangaudiodev/linux/YangAudioAecLinux.h>
#include <yangaudiodev/linux/YangAudioCaptureLinux.h>
#include <yangcapture/linux/YangVideoCaptureLinux.h>
#endif
#endif
#if Yang_OS_APPLE
#include <yangaudiodev/mac/YangAudioCaptureMac.h>
#include <yangcapture/mac/YangVideoCaptureMac.h>
#endif
YangCaptureFactory::YangCaptureFactory() {


}

YangCaptureFactory::~YangCaptureFactory() {

}

YangAudioCapture* YangCaptureFactory::createAudioCapture(YangContext *pcontext){
#if Yang_OS_WIN
	return new YangWinAudioApiDevice(pcontext,0,true);
	// return new YangWinAudioCapture(pcontext);
#else

#if Yang_OS_ANDROID
	return NULL;
#else
    #if Yang_OS_APPLE
        return NULL;
    #else
        return new YangAudioAecLinux(&pcontext->avinfo,&pcontext->synMgr);//new YangAlsaHandle(pcontext);//YangAudioCaptureImpl(pcontext);
    #endif
#endif
#endif

}
YangAudioCapture* YangCaptureFactory::createRecordAudioCapture(YangAVInfo *avinfo){
#if Yang_OS_WIN
	 return new YangAudioCaptureWindows(avinfo);
	 //return new YangWinAudioApiDevice(pcontext,1);
#else
#if Yang_OS_ANDROID
	 return new YangAudioCaptureAndroid(avinfo);
#else
    #if Yang_OS_APPLE
        return new YangAudioCaptureMac(avinfo);
    #else
         return new YangAudioCaptureLinux(avinfo);//new YangAlsaHandle(pcontext);//YangAudioCaptureImpl(pcontext);
    #endif
#endif
#endif
}



YangMultiVideoCapture* YangCaptureFactory::createVideoCapture(YangVideoInfo *pcontext){
#if Yang_OS_WIN
	return new YangVideoCaptureWindows(pcontext);

#else
#if Yang_OS_ANDROID
	return NULL;
#else
    #if Yang_OS_APPLE
        return NULL;
    #else
        return new YangVideoCaptureLinux(pcontext);
    #endif
#endif
#endif
}
YangMultiVideoCapture* YangCaptureFactory::createRecordVideoCapture(YangVideoInfo *pcontext){
#if Yang_OS_WIN
	return new YangVideoCaptureWindows(pcontext);
#else
#if Yang_OS_ANDROID
	return NULL;
#else
    #if Yang_OS_APPLE
        return new YangVideoCaptureMac(pcontext);
    #else
        return new YangVideoCaptureLinux(pcontext);
    #endif
#endif
#endif
}
YangMultiVideoCapture* YangCaptureFactory::createRecordVideoCaptureAndroid(YangVideoInfo *pcontext,void* pwindow){
#if Yang_OS_ANDROID
	return new YangVideoCaptureAndroid(pcontext,pwindow);
#else
	return NULL;
#endif
}

