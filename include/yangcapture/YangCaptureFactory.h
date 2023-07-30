//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef INCLUDE_YANGCAPTURE_YANGCAPTUREFACTORY_H_
#define INCLUDE_YANGCAPTURE_YANGCAPTUREFACTORY_H_
#include <yangcapture/YangMultiVideoCapture.h>
#include <yangaudiodev/YangAudioCapture.h>
#include "YangScreenCapture.h"


class YangCaptureFactory {
public:
	YangCaptureFactory();
	virtual ~YangCaptureFactory();
	YangAudioCapture *createAudioCapture(YangContext *pcontext);
	YangAudioCapture *createRecordAudioCapture(YangAVInfo *avinfo);
	YangMultiVideoCapture *createVideoCapture(YangVideoInfo *pcontext);
    YangMultiVideoCapture *createRecordVideoCapture(YangVideoInfo *pcontext);
    YangMultiVideoCapture *createRecordVideoCaptureAndroid(YangVideoInfo *pcontext,void* pwindow);


};

#endif /* INCLUDE_YANGCAPTURE_YANGCAPTUREFACTORY_H_ */
