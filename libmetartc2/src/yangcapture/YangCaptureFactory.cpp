/*
 * YangCaptureImpl.cpp
 *
 *  Created on: 2019年8月30日
 *      Author: yang
 */

#include "yangcapture/YangCaptureFactory.h"
#ifndef _WIN32
#include <yangaudiodev/linux/YangAlsaDeviceHandle.h>
#include <yangaudiodev/linux/YangAlsaHandle.h>
#include <yangaudiodev/linux/YangAudioCaptureImpl.h>
#include "YangVideoCaptureImpl.h"

#else
#include <yangaudiodev/win/YangWinAudioCapture.h>
#include <yangaudiodev/win/YangWinAudioApiDevice.h>
#include <yangaudiodev/win/YangWinRecordAudioCapture.h>
#include "win/YangWinVideoCapture.h"
#endif
#include "YangScreenCaptureImpl.h"

YangCaptureFactory::YangCaptureFactory() {


}

YangCaptureFactory::~YangCaptureFactory() {

}

YangAudioCapture *YangCaptureFactory::createAudioCapture(YangContext *pcontext){
#ifndef _WIN32
	return new YangAlsaDeviceHandle(pcontext);//new YangAlsaHandle(pcontext);//YangAudioCaptureImpl(pcontext);
#else
   // return new YangWinAudioCapture(pcontext);
   return new YangWinAudioApiDevice(pcontext,0,true);
#endif

}
YangAudioCapture *YangCaptureFactory::createRecordAudioCapture(YangContext *pcontext){
#ifndef _WIN32
	return new YangAudioCaptureImpl(pcontext);//new YangAlsaHandle(pcontext);//YangAudioCaptureImpl(pcontext);
#else
    return new YangWinRecordAudioCapture(pcontext);
    //return new YangWinAudioApiDevice(pcontext,1);
#endif
}


YangMultiVideoCapture *YangCaptureFactory::createVideoCapture(YangVideoInfo *pcontext){
#ifndef _WIN32
	return new YangVideoCaptureImpl(pcontext);
#else
    return new YangWinVideoCapture(pcontext);
#endif
}
YangMultiVideoCapture *YangCaptureFactory::createRecordVideoCapture(YangVideoInfo *pcontext){
#ifndef _WIN32
	return new YangVideoCaptureImpl(pcontext);
#else
	return new YangWinVideoCapture(pcontext);
#endif
}

YangScreenCapture *YangCaptureFactory::createScreenCapture(YangContext *pcontext){
	return new YangScreenCaptureImpl(pcontext);
}
