//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGCAPTURE_SRC_YANGVIDEOCAPTUREIMPL_H_
#define YANGCAPTURE_SRC_YANGVIDEOCAPTUREIMPL_H_

#include <yangcapture/YangVideoCaptureHandle.h>
#include <yangcapture/YangMultiVideoCapture.h>
#if Yang_OS_LINUX
#include <linux/videodev2.h>

#define REQ_BUF_NUM 4
typedef struct buffer_type_r buffer_type;
struct buffer_type_r {
	uint8_t *start;
	int32_t length;
};

class YangVideoCaptureLinux: public YangMultiVideoCapture {
public:
	YangVideoCaptureLinux(YangVideoInfo *pcontext);
	~YangVideoCaptureLinux();
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
protected:
	void startLoop();
	long m_difftime(struct timeval *p_start, struct timeval *p_end);
private:
	int32_t setPara();
	void setReselution(__u32 format, int32_t val);
	void setReselutionPara(__u32 pformat);
	void process_image(char *p_addr, int32_t p_length);
	int32_t read_buffer();
	void stop_capturing();
	void uninit_camer_device();
	void close_camer_device();
	int32_t enum_camera_frmival(int32_t fd, struct v4l2_frmsizeenum *framesize);

	int32_t m_hasYuy2, m_hasI420, m_hasNv12, m_hasYv12, m_hasP010, m_hasP016;
	int32_t m_width, m_height;
	int32_t m_vd_id;
	struct v4l2_buffer m_buf;
	buffer_type m_user_buffer[REQ_BUF_NUM];
	int32_t m_buffer_count;
	int32_t m_isloop;
	int32_t m_isFirstFrame;
	struct timeval m_startTime;
	long m_timestatmp;

};
#endif
#endif /* YANGCAPTURE_SRC_YANGVIDEOCAPTUREIMPL_H_ */
