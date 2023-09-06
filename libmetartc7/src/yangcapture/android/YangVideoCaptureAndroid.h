//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGCAPTURE_YangVideoCaptureAndroid_H_
#define YANGCAPTURE_YangVideoCaptureAndroid_H_

#include <yangcapture/YangVideoCaptureHandle.h>
#include <yangcapture/YangMultiVideoCapture.h>

#if Yang_OS_ANDROID
#include <yangcapture/android/YangCameraAndroid.h>

class YangVideoCaptureAndroid: public YangMultiVideoCapture {
public:
	YangVideoCaptureAndroid(YangVideoInfo *pcontext,void* pwindows);
	~YangVideoCaptureAndroid();
	YangVideoCaptureHandle *m_vhandle;
	YangCameraAndroid* m_camera;
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
protected:
	void startLoop();
	int32_t setPara();
	YangVideoBuffer* m_pre_videoBuffer;

private:
	int32_t m_width, m_height;
	int32_t m_isloop;
};
#endif
#endif /* YANGCAPTURE_SRC_YANGVIDEOCAPTUREIMPL_H_ */
