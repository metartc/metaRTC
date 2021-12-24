
#ifndef YANGCAPTURE_WIN_API_YANGCAPTURECALLBACK_H_
#define YANGCAPTURE_WIN_API_YANGCAPTURECALLBACK_H_
#include <yangutil/yangavtype.h>
class YangCaptureCallback{
public:
	YangCaptureCallback(){};
	virtual ~YangCaptureCallback(){};
	virtual void caputureAudioData(YangFrame* audioFrame)=0;
};




#endif /* YANGCAPTURE_WIN_API_YANGCAPTURECALLBACK_H_ */
