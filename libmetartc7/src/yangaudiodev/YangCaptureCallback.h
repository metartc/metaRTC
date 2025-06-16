//
// Copyright (c) 2019-2025 yanggaofeng
//
#ifndef YANGCAPTURE_WIN_API_YANGCAPTURECALLBACK_H_
#define YANGCAPTURE_WIN_API_YANGCAPTURECALLBACK_H_
#include <yangutil/yangavinfo.h>
class YangCaptureCallback{
public:
	YangCaptureCallback(){};
	virtual ~YangCaptureCallback(){};
	virtual void caputureAudioData(YangFrame* audioFrame)=0;
};

#endif /* YANGCAPTURE_WIN_API_YANGCAPTURECALLBACK_H_ */
