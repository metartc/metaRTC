/*
 * YangScreenCapture.h
 *
 *  Created on: 2020年8月30日
 *      Author: yang
 */

#ifndef YANGCAPTURE_SRC_YANGSCREENCAPTUREIMPL_H_
#define YANGCAPTURE_SRC_YANGSCREENCAPTUREIMPL_H_
#include <yangcapture/YangScreenCapture.h>
#include "yangutil/buffer/YangVideoBuffer.h"
#include <yangutil/yangavinfotype.h>

class YangScreenCaptureImpl:public YangScreenCapture {
public:
	YangScreenCaptureImpl(YangContext* pcontext);
	virtual ~YangScreenCaptureImpl();
	int32_t m_width,m_height;
	int32_t init();
	void setInterval(int32_t pinterval);
	void setDrawmouse(bool isDraw);
	bool getisDrawmouse();
	YangVideoBuffer * getOutVideoBuffer();
	YangVideoBuffer * getPreVideoBuffer();
	void setVideoCaptureStart();
	void setVideoCaptureStop();
	int32_t getVideoCaptureState();
	YangVideoBuffer* getScreenBuffer();
    uint8_t *m_rgb;
	int32_t m_bufLen,isCapture;
	int32_t m_fd;
	void putBuffer(YangFrame* videoFrame);
	void startLoop();
	void stopLoop();
private:
	YangVideoBuffer *m_out_videoBuffer;
	YangVideoBuffer *m_pre_videoBuffer;
	YangContext* m_context;
	int m_loop;
	int m_state;
	int32_t m_interval;
	bool m_isDrawmouse;
};

#endif /* YANGCAPTURE_SRC_YANGSCREENCAPTUREIMPL_H_ */
