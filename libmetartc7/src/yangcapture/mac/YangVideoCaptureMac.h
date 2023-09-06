//
// Copyright (c) 2019-2023 yanggaofeng
//

#ifndef YANGCAPTURE_SRC_YangVideoCaptureMac_H_
#define YANGCAPTURE_SRC_YangVideoCaptureMac_H_

#include <yangcapture/YangVideoCaptureHandle.h>
#include <yangcapture/YangMultiVideoCapture.h>

#if Yang_OS_APPLE
#include <yangcapture/mac/YangVideoDeviceMac.h>

class YangVideoCaptureMac: public YangMultiVideoCapture {
public:
	YangVideoCaptureMac(YangVideoInfo *pcontext);
	~YangVideoCaptureMac();
	YangVideoCaptureHandle *m_vhandle;
	int32_t init();
	void setVideoCaptureStart();
	void setVideoCaptureStop();
	void setOutVideoBuffer(YangVideoBuffer *pbuf);
	void setPreVideoBuffer(YangVideoBuffer *pbuf);
	int32_t getVideoCaptureState();
	void initstamp();
	void stopLoop();

	int32_t getLivingVideoCaptureState();
	int32_t getFilmVideoCaptureState();

	void setLivingOutVideoBuffer(YangVideoBuffer *pbuf);
	void setLivingVideoCaptureStart();
	void setLivingVideoCaptureStop();

	void setFilmOutVideoBuffer(YangVideoBuffer *pbuf);
	void setFilmVideoCaptureStart();
	void setFilmVideoCaptureStop();
    void on_video(uint8_t* data,uint32_t nb,uint64_t ts);
protected:
	void startLoop();
	long m_difftime(struct timeval *p_start, struct timeval *p_end);
private:
    YangVideoDeviceMac* m_device;
	int32_t m_width, m_height;
    yangbool m_isloop;
    yangbool m_waitState;
    yang_thread_mutex_t m_lock;
    yang_thread_cond_t m_cond_mess;


};
#endif
#endif /* YANGCAPTURE_SRC_YANGVIDEOCAPTUREIMPL_H_ */
