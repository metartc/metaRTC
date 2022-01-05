/*
 * YangCaptureImpl.h
 *
 *  Created on: 2020年8月30日
 *      Author: yang
 */

#ifndef INCLUDE_YANGCAPTURE_YANGCAPTUREFACTORY_H_
#define INCLUDE_YANGCAPTURE_YANGCAPTUREFACTORY_H_
#include <yangcapture/YangMultiVideoCapture.h>
#include <yangaudiodev/YangAudioCapture.h>
#include "YangScreenCapture.h"
//#include "../include/YangLivingVideoCapture.h"

class YangCaptureFactory {
public:
	YangCaptureFactory();
	virtual ~YangCaptureFactory();
	YangAudioCapture *createAudioCapture(YangContext *pcontext);
	YangAudioCapture *createRecordAudioCapture(YangContext *pcontext);
        //YangMultiVideoCapture *createVideoCapture(YangVideoInfo *pcontext);
	YangMultiVideoCapture *createVideoCapture(YangVideoInfo *pcontext);
        YangMultiVideoCapture *createRecordVideoCapture(YangVideoInfo *pcontext);
	YangScreenCapture *createScreenCapture(YangContext *pcontext);
};

#endif /* INCLUDE_YANGCAPTURE_YANGCAPTUREFACTORY_H_ */
