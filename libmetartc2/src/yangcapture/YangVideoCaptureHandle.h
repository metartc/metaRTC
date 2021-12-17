#ifndef __YangLivingVideoCaptureHandle__
#define __YangLivingVideoCaptureHandle__
#include <yangavutil/video/YangYuvConvert.h>
#include "yangutil/buffer/YangVideoBuffer.h"
#include "yangutil/sys/YangIni.h"
class YangVideoCaptureHandle
{
public:
	YangVideoCaptureHandle(YangVideoInfo *pcontext);
	virtual ~YangVideoCaptureHandle(void);
    void reset(YangVideoInfo* p_config);

    void putBuffer(int64_t startTime, uint8_t *pBuffer, int32_t BufferLen);
    void putBuffers();
	void saveFile(char* fileName,double Time, uint8_t *pBuffer, int32_t BufferLen);
	void initstamp();
	void startLoop();
	long m_start_time;
	int32_t m_isCapture;
	int32_t m_isLivingCaptrue;
	int32_t m_isFilm;
	void  setVideoBuffer(YangVideoBuffer *pbuf);
	void  setLivingVideoBuffer(YangVideoBuffer *pbuf);
	void  setFilmVideoBuffer(YangVideoBuffer *pbuf);
	void  setPreVideoBuffer(YangVideoBuffer *plist);
	void  setCaptureFormat(int32_t pformat);
private:

	YangVideoBuffer *m_out_videoBuffer,*m_pre_videoBuffer,*m_living_out_videoBuffer,*m_film_out_videoBuffer;
	YangFrame m_videoFrame;
	YangYuvConvert m_yuv;
	uint8_t *m_buf;
    int32_t m_width,m_height,m_bufLen;
	int64_t vtick;
	int64_t curstamp,basesatmp;
	int m_encoderVideoFormat;



};

#endif
