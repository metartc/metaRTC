//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef INCLUDE_YANGCAPTURE_YANGSCREENCAPTURE_H_
#define INCLUDE_YANGCAPTURE_YANGSCREENCAPTURE_H_

#include "yangutil/buffer/YangVideoBuffer.h"
#include <yangutil/sys/YangThread2.h>
#include "YangScreenCaptureHandleI.h"


class YangScreenCapture :public YangThread{
public:
	YangScreenCapture();
	virtual ~YangScreenCapture();

	int32_t m_isStart;

public:
	virtual void setDrawmouse(bool isDraw)=0;
	virtual bool getisDrawmouse()=0;

	virtual void setInterval(int32_t pinterval)=0;
	virtual int32_t init()=0;
	virtual void setVideoCaptureStart()=0;
	virtual void setVideoCaptureStop()=0;
	virtual YangVideoBuffer* getOutVideoBuffer()=0;
	virtual YangVideoBuffer* getPreVideoBuffer()=0;
	virtual int32_t getVideoCaptureState()=0;
	void stop();
protected:
    void run();
    virtual void startLoop()=0;
    virtual  void stopLoop()=0;



};

#endif /* INCLUDE_YANGCAPTURE_YANGSCREENCAPTURE_H_ */
