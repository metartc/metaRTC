//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef __YangLivingVideoCaptureHandle__
#define __YangLivingVideoCaptureHandle__
#include <yangavutil/video/YangYuvConvert.h>
#include <yangutil/buffer/YangVideoBuffer.h>
#include <yangutil/yangavinfotype.h>


class YangVideoCaptureHandle
{
public:
	YangVideoCaptureHandle(YangVideoInfo *pcontext);
	virtual ~YangVideoCaptureHandle(void);
    void reset(YangVideoInfo* p_config);

    void putBuffer(int64_t startTime, uint8_t *pBuffer, int32_t BufferLen);
    void putBufferAndroid(int64_t startTime, uint8_t *pBuffer, int32_t BufferLen);
    void putBufferAndroid2(int64_t startTime, uint8_t *data_y, uint8_t* data_u,uint8_t* data_v);
    void putBufferNv21(int64_t startTime, uint8_t *data_y, uint8_t* data_u);
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
	uint8_t *m_buf,*m_androidBuf;
    int32_t m_width,m_height,m_bufLen,m_ylen,m_ulen;
	int64_t vtick;
	int64_t curstamp,basesatmp;
	int m_encoderVideoFormat;
	int m_captureVideoFormat;
	int32_t m_rotate;
	bool m_isHardwareEncoder;



};

#endif
