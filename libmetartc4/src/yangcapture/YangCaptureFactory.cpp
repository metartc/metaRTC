//
// Copyright (c) 2019-2022 yanggaofeng
//

#include "yangcapture/YangCaptureFactory.h"
#ifdef _WIN32
#include <yangaudiodev/win/YangWinAudioCapture.h>
#include <yangaudiodev/win/YangWinAudioApiDevice.h>
#include <yangaudiodev/win/YangAudioCaptureWindows.h>
#include "win/YangVideoCaptureWindows.h"
#else
#ifdef __ANDROID__
#include <yangaudiodev/android/YangAudioCaptureAndroid.h>
#include <yangcapture/android/YangVideoCaptureAndroid.h>
#else
#include <yangaudiodev/linux/YangAlsaDeviceHandle.h>
#include <yangaudiodev/linux/YangAlsaHandle.h>
#include <yangaudiodev/linux/YangAudioCaptureLinux.h>
#include <yangcapture/linux/YangVideoCaptureLinux.h>
#endif
#endif

#include "YangScreenCaptureImpl.h"

YangCaptureFactory::YangCaptureFactory() {


}

YangCaptureFactory::~YangCaptureFactory() {

}

YangAudioCapture* YangCaptureFactory::createAudioCapture(YangContext *pcontext){
#ifdef _WIN32
	return new YangWinAudioApiDevice(pcontext,0,true);
	// return new YangWinAudioCapture(pcontext);
#else

#ifdef __ANDROID__
	return NULL;
#else
	return new YangAlsaDeviceHandle(pcontext);//new YangAlsaHandle(pcontext);//YangAudioCaptureImpl(pcontext);
#endif
#endif

}
YangAudioCapture* YangCaptureFactory::createRecordAudioCapture(YangContext *pcontext){
#ifdef _WIN32
	 return new YangAudioCaptureWindows(pcontext);
	 //return new YangWinAudioApiDevice(pcontext,1);
#else
#ifdef __ANDROID__
	 return new YangAudioCaptureAndroid(pcontext);
#else
	 return new YangAudioCaptureLinux(pcontext);//new YangAlsaHandle(pcontext);//YangAudioCaptureImpl(pcontext);
#endif
#endif
}


YangMultiVideoCapture* YangCaptureFactory::createVideoCapture(YangVideoInfo *pcontext){
#ifdef _WIN32
	return new YangVideoCaptureWindows(pcontext);

#else
#ifdef __ANDROID__
	return NULL;
#else
	return new YangVideoCaptureLinux(pcontext);
#endif
#endif
}
YangMultiVideoCapture* YangCaptureFactory::createRecordVideoCapture(YangVideoInfo *pcontext){
#ifdef _WIN32
	return new YangVideoCaptureWindows(pcontext);
#else
#ifdef __ANDROID__
	return NULL;
#else
	return new YangVideoCaptureLinux(pcontext);
#endif
#endif
}
YangMultiVideoCapture* YangCaptureFactory::createRecordVideoCaptureAndroid(YangVideoInfo *pcontext,void* pwindow){
#ifdef __ANDROID__
	return new YangVideoCaptureAndroid(pcontext,pwindow);
#else
	return NULL;
#endif
}
YangScreenCapture *YangCaptureFactory::createScreenCapture(YangContext *pcontext){
	return new YangScreenCaptureImpl(pcontext);
}
