//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGCAPTURE_SRC_YANGSCREENSHARE_H_
#define YANGCAPTURE_SRC_YANGSCREENSHARE_H_
#include "yangcapture/YangScreenCapture.h"
class YangScreenShare :public YangScreenCapture{
public:
	YangScreenShare();
	virtual ~YangScreenShare();


public:
	void setScreenHandle(YangScreenCaptureHandleI *handle);
	void setInterval(int32_t pinterval);
	int32_t init();
	void setVideoCaptureStart();
	void setVideoCaptureStop();
	void setOutVideoBuffer(YangVideoBuffer *pbuf);
	int32_t getVideoCaptureState();
	void initstamp();
	void stopLoop();
private:
	int32_t m_isloop;
	int32_t m_isCapture;
	int32_t m_interval;
	YangScreenCaptureHandleI *m_capture;
	YangVideoBuffer *m_out_videoBuffer;


protected:
	void startLoop();
};




#endif /* YANGCAPTURE_SRC_YANGSCREENSHARE_H_ */
