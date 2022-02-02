/*
 * YangLivingVideoCaptureWinImpl.h
 *
 *  Created on: 2020年8月30日
 *      Author: yang
 */

#ifndef YANGCAPTURE_SRC_YangLivingVideoCaptureWinImpl_H_
#define YANGCAPTURE_SRC_YangLivingVideoCaptureWinImpl_H_

#ifdef _WIN32

#include <yangcapture/YangMultiVideoCapture.h>
#include <windows.h>
#include "amstream.h"
#include <control.h>
#include "qedit.h"

#include <dmodshow.h>
#include <Dmoreg.h>
#include <initguid.h>
#include "YangWinVideoCaptureHandle.h"

struct YangVideoCaptureType{
	YangYuvType yuvType;
	int32_t state;
	int32_t width;
	int32_t height;
};
class YangWinVideoCapture: public YangMultiVideoCapture {
public:
    YangWinVideoCapture(YangVideoInfo *pcontext);
	virtual ~YangWinVideoCapture();
	YangWinVideoCaptureHandle *m_vhandle;
	int32_t init();
	void setVideoCaptureStart();
	void setVideoCaptureStop();
	int32_t getVideoCaptureState();
	int32_t getLivingVideoCaptureState();
	int32_t getFilmVideoCaptureState();

	void setLivingOutVideoBuffer(YangVideoBuffer *pbuf);
	void setLivingVideoCaptureStart();
	void setLivingVideoCaptureStop();

	void setFilmOutVideoBuffer(YangVideoBuffer *pbuf);
	void setFilmVideoCaptureStart();
	void setFilmVideoCaptureStop();

	void setOutVideoBuffer(YangVideoBuffer *pbuf);
	void setPreVideoBuffer(YangVideoBuffer *pbuf);

	void initstamp();
	void stopLoop();

protected:
	void startLoop();
	//long m_difftime(struct timeval *p_start, struct timeval *p_end);

	void setRevolutionPara();
	void setRevolution();
	IPin* FindPin(IBaseFilter *pFilter, PIN_DIRECTION dir);
	IPin* GetOutPin(IBaseFilter *pFilter, PIN_DIRECTION pin_dir, int32_t nPin);
	IPin* GetInPin(IBaseFilter *pFilter, PIN_DIRECTION pin_dir, int32_t nPin);

	IMediaControl *m_pm;
	IGraphBuilder *m_pg;
	ICaptureGraphBuilder2 *m_pb;
	IBaseFilter *m_videoSrc, *m_grabberF;
	ISampleGrabber *m_grabber;
	IBaseFilter *m_nullRender;
	//IMoniker  *m_moniker;
	IMediaEventEx *m_event;
private:
	void setWH(YangVideoCaptureType *pct);
	REFGUID getUID();

	YangVideoCaptureType m_yuy2, m_i420 , m_nv12, m_yv12, m_p010,m_p016;
	int32_t m_width, m_height;
	int32_t m_vd_id;
	int32_t cameraIndex;
	int32_t hasVideo;
	int32_t m_preframe, m_isOpAddMinus;
	ULONG m_t_time;
	int32_t m_buffer_count;
	int32_t m_isloop;
	int32_t m_isFirstFrame;
	//struct timeval m_startTime;
	long m_timestatmp;

};
#endif
#endif /* YANGCAPTURE_SRC_YangLivingVideoCaptureWinImpl_H_ */
