//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangcapture/linux/YangVideoCaptureLinux.h>
#include <yangutil/yangavinfotype.h>
#include <fcntl.h>
#if Yang_OS_LINUX

#include <linux/fb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <assert.h>
#include <iostream>



YangVideoCaptureLinux::YangVideoCaptureLinux(YangVideoInfo *pcontext) {

	m_para = pcontext;

	m_vhandle = new YangVideoCaptureHandle(pcontext);
	cameraIndex = pcontext->vIndex;
	m_width = m_para->width;
	m_height = m_para->height;
	m_vd_id = 0;

	memset(&m_buf, 0, sizeof(m_buf));
	m_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	m_buf.memory = V4L2_MEMORY_MMAP;
	m_isloop = 0;
	m_isFirstFrame = 0;
	m_buffer_count = 0;
	m_timestatmp = 0;
	m_hasYuy2 = 0, m_hasI420 = 0, m_hasNv12 = 0, m_hasYv12 = 0, m_hasP010 = 0,m_hasP016=0;
}

YangVideoCaptureLinux::~YangVideoCaptureLinux() {
	if (m_isloop) {
		stop();
		while (m_isStart) {
			yang_usleep(1000);
		}
	}
	stop_capturing();
	uninit_camer_device();
	close_camer_device();
	yang_delete(m_vhandle);
}
void YangVideoCaptureLinux::setVideoCaptureStart() {
	m_vhandle->m_isCapture = 1;
}
void YangVideoCaptureLinux::setVideoCaptureStop() {
	m_vhandle->m_isCapture = 0;
}
int32_t YangVideoCaptureLinux::getVideoCaptureState() {
	return m_vhandle->m_isCapture;
}
int32_t YangVideoCaptureLinux::getLivingVideoCaptureState() {
	return m_vhandle->m_isLivingCaptrue;
}
int32_t YangVideoCaptureLinux::getFilmVideoCaptureState() {
	return m_vhandle->m_isFilm;
}

void YangVideoCaptureLinux::setLivingVideoCaptureStart() {
	m_vhandle->m_isLivingCaptrue = 1;
}
void YangVideoCaptureLinux::setLivingVideoCaptureStop() {
	m_vhandle->m_isLivingCaptrue = 0;
}

void YangVideoCaptureLinux::setFilmVideoCaptureStart() {
	m_vhandle->m_isFilm = 1;
}
void YangVideoCaptureLinux::setFilmVideoCaptureStop() {
	m_vhandle->m_isFilm = 0;
}

void YangVideoCaptureLinux::setOutVideoBuffer(YangVideoBuffer *pbuf) {
	m_vhandle->setVideoBuffer(pbuf);
}
void YangVideoCaptureLinux::setLivingOutVideoBuffer(YangVideoBuffer *pbuf) {
	m_vhandle->setLivingVideoBuffer(pbuf);
}
void YangVideoCaptureLinux::setFilmOutVideoBuffer(YangVideoBuffer *pbuf) {
	m_vhandle->setFilmVideoBuffer(pbuf);
}
void YangVideoCaptureLinux::setPreVideoBuffer(YangVideoBuffer *pbuf) {
	m_vhandle->setPreVideoBuffer(pbuf);
}
void YangVideoCaptureLinux::initstamp() {
	m_vhandle->initstamp();
}
int32_t YangVideoCaptureLinux::enum_camera_frmival(int32_t fd, struct v4l2_frmsizeenum *framesize) {
	struct v4l2_frmivalenum frmival;
	memset(&frmival, 0, sizeof(frmival));
	frmival.pixel_format = framesize->pixel_format;
	frmival.width = framesize->discrete.width;
	frmival.height = framesize->discrete.height;
	//frmival.type = V4L2_FRMIVAL_TYPE_DISCRETE;
	frmival.index = 0;


	while (ioctl(fd, VIDIOC_ENUM_FRAMEINTERVALS, &frmival) == 0) {
		//输出分数，即帧间隔
		if(frmival.width==m_para->width) {
			yang_trace("%d.frameinterval:%u/%u\n ", frmival.index,
				frmival.discrete.numerator, frmival.discrete.denominator);
		}
		frmival.index++;
	}

	return 0;
}

void YangVideoCaptureLinux::setReselution(__u32 format,int32_t val){
	if (format == V4L2_PIX_FMT_YUYV)
		m_hasYuy2 = val;
	if (format == V4L2_PIX_FMT_YUV420)
		m_hasI420 = val;
	if (format == V4L2_PIX_FMT_NV12)
		m_hasNv12 = val;
	if (format == V4L2_PIX_FMT_YVU420)
		m_hasYv12 = val;
}
void YangVideoCaptureLinux::setReselutionPara(__u32 pformat){
	struct v4l2_fmtdesc fmt;
	int32_t vet=0;
	while ((vet = ioctl(m_vd_id, VIDIOC_ENUM_FMT, &fmt)) != -1) {
			fmt.index++;
			if(fmt.pixelformat==pformat){
				struct v4l2_frmsizeenum frmsize;
				frmsize.pixel_format = fmt.pixelformat;
				frmsize.index = 0;
				while (!ioctl(m_vd_id, VIDIOC_ENUM_FRAMESIZES, &frmsize)) {
					if (frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE) {
						m_para->width=frmsize.discrete.width;
						m_para->height=frmsize.discrete.height;
						m_width=frmsize.discrete.width;
						m_height=frmsize.discrete.height;
					}
					enum_camera_frmival(m_vd_id, &frmsize);
					frmsize.index++;
				}
			}
		}

}
int32_t YangVideoCaptureLinux::setPara() {
	struct v4l2_capability cap;
	struct v4l2_fmtdesc fmt;
	memset(&fmt, 0, sizeof(struct v4l2_fmtdesc));
	fmt.index = 0;
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	int32_t vet = 0;


	if (ioctl(m_vd_id, VIDIOC_QUERYCAP, &cap) != 0) {
		yang_error("\n VIDIOC_QUERYCAP error!");
		return ERROR_SYS_Linux_NoVideoDriver;
	}
	yang_trace("\ndriver name %s card = %s cap = %0x\n", cap.driver, cap.card,
			cap.capabilities);

	while ((vet = ioctl(m_vd_id, VIDIOC_ENUM_FMT, &fmt)) != -1) {
		fmt.index++;
		setReselution(fmt.pixelformat,1);

		yang_trace("\n{ pixelformat = ''%c%c%c%c'', description = ''%s'' }\n",
				fmt.pixelformat & 0xFF, (fmt.pixelformat >> 8) & 0xFF,
				(fmt.pixelformat >> 16) & 0xFF, (fmt.pixelformat >> 24) & 0xFF,
				fmt.description);
		struct v4l2_frmsizeenum frmsize;
		frmsize.pixel_format = fmt.pixelformat;
		frmsize.index = 0;
		while (!ioctl(m_vd_id, VIDIOC_ENUM_FRAMESIZES, &frmsize)) {
			if (frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE) {
				if(m_para->width==(int)frmsize.discrete.width&&m_para->height==(int)frmsize.discrete.height){
					yang_trace("DISCRETE: line-%d %d: {%d*%d}\n", __LINE__,
											frmsize.index, frmsize.discrete.width,frmsize.discrete.height);
					setReselution(fmt.pixelformat,2);
				}

			}

			else if (frmsize.type == V4L2_FRMSIZE_TYPE_STEPWISE) {
				yang_trace("\nSTEPWISE: line: %d %d: {%d*%d}\n", __LINE__,
						frmsize.index, frmsize.stepwise.max_width,
						frmsize.stepwise.max_height);
			}

			enum_camera_frmival(m_vd_id, &frmsize);
			frmsize.index++;
		}
	}

	return Yang_Ok;
}
std::string getVideoFormat(uint32_t  pformat ){
	std::string res="";
	if(pformat==V4L2_PIX_FMT_YUV420) res="I420";
	if(pformat==V4L2_PIX_FMT_NV12) res="Nv12";
	if(pformat==V4L2_PIX_FMT_YVU420) res="Yv12";
	if(pformat==V4L2_PIX_FMT_YUYV) res="Yuy2";
	return res;
}
int32_t YangVideoCaptureLinux::init() {
	char devStr[30];
	memset(devStr, 0, 30);
	sprintf(devStr, "/dev/video%d", cameraIndex);

	if ((m_vd_id = open(devStr, O_RDWR)) == -1) {
		yang_error("open video device Error!");
		return ERROR_SYS_Linux_VideoDeveceOpenFailure;
	}

	int32_t ret = Yang_Ok;
	ret = setPara();
	if (ret) return ret;
	uint32_t  format = V4L2_PIX_FMT_YUYV;
	if(m_hasYuy2>1||m_hasI420>1||m_hasNv12>1||m_hasYv12>1){
		if(m_hasI420>1) {
			format = V4L2_PIX_FMT_YUV420;
			m_para->videoCaptureFormat=YangI420;
		}else if(m_hasNv12>1){
			format = V4L2_PIX_FMT_NV12;
			m_para->videoCaptureFormat=YangNv12;
		}else if(m_hasYv12>1){
			format = V4L2_PIX_FMT_YVU420;
			m_para->videoCaptureFormat=YangYv12;
		}else if(m_hasYuy2>1){
			format = V4L2_PIX_FMT_YUYV;
			m_para->videoCaptureFormat=YangYuy2;
		}
		#if Yang10bit
		if(m_para->videoCaptureFormat==YangP010) format=V4L2_PIX_FMT_P010;
		if(m_para->videoCaptureFormat==YangP016) format=V4L2_PIX_FMT_P016;
		#endif

	}else if(m_hasYuy2||m_hasI420||m_hasNv12||m_hasYv12){
		if(m_hasI420) {
			format = V4L2_PIX_FMT_YUV420;
			m_para->videoCaptureFormat=YangI420;
		}else if(m_hasNv12){
			format = V4L2_PIX_FMT_NV12;
			m_para->videoCaptureFormat=YangNv12;
		}else if(m_hasYv12){
			format = V4L2_PIX_FMT_YVU420;
			m_para->videoCaptureFormat=YangYv12;
		}else if(m_hasYuy2){
			format = V4L2_PIX_FMT_YUYV;
			m_para->videoCaptureFormat=YangYuy2;
		}
		setReselutionPara(format);
	}else{
		return ERROR_SYS_Linux_NoVideoCatpureInterface;
	}

	struct v4l2_format v4_format;
	v4_format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	v4_format.fmt.pix.width = m_width;
	v4_format.fmt.pix.height = m_height;
	v4_format.fmt.pix.pixelformat = format;	//V4L2_PIX_FMT_YUYV; //V4L2_PIX_FMT_YUYV;
	v4_format.fmt.pix.field = V4L2_FIELD_INTERLACED;

	if ((ioctl(m_vd_id, VIDIOC_S_FMT, &v4_format)) != 0) {
		yang_error("\n set fmt error!");

	}
	if(m_vhandle) m_vhandle->setCaptureFormat(m_para->videoCaptureFormat);




	struct v4l2_streamparm Stream_Parm;
	memset(&Stream_Parm, 0, sizeof(struct v4l2_streamparm));
	Stream_Parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	Stream_Parm.parm.capture.timeperframe.denominator =m_para->frame;;
	Stream_Parm.parm.capture.timeperframe.numerator = 1;

	if( ioctl(m_vd_id, VIDIOC_S_PARM, &Stream_Parm)){
		yang_error("\n..........................set video frame error!");
	}

	//	setfps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	//  setfps.parm.capture.timeperframe.numerator = 10;
	//  setfps.parm.capture.timeperframe.denominator = 10;
	struct v4l2_requestbuffers tV4L2_reqbuf;
	uint32_t i = 0;
	memset(&tV4L2_reqbuf, 0, sizeof(struct v4l2_requestbuffers));
	tV4L2_reqbuf.count = REQ_BUF_NUM;
	tV4L2_reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	tV4L2_reqbuf.memory = V4L2_MEMORY_MMAP;

	if (ioctl(m_vd_id, VIDIOC_REQBUFS, &tV4L2_reqbuf)) {

	}
	m_buffer_count = tV4L2_reqbuf.count;

	if (m_user_buffer == NULL) {
		yang_error("calloc Error");
		exit(1);
	}
	for (i = 0; i < tV4L2_reqbuf.count; i++) {
		struct v4l2_buffer tV4L2buf;
		memset(&tV4L2buf, 0, sizeof(struct v4l2_buffer));
		tV4L2buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		tV4L2buf.memory = V4L2_MEMORY_MMAP;
		tV4L2buf.index = i;
		if (ioctl(m_vd_id, VIDIOC_QUERYBUF, &tV4L2buf))
			printf("search!");

		m_user_buffer[i].length = tV4L2buf.length;
		m_user_buffer[i].start = (uint8_t*) mmap( NULL, tV4L2buf.length,
		PROT_READ | PROT_WRITE, MAP_SHARED, m_vd_id, tV4L2buf.m.offset);
		if (MAP_FAILED == m_user_buffer[i].start) {
			yang_error(" error! mmap");
			exit(1);
		}
	}
	return Yang_Ok;
}

long YangVideoCaptureLinux::m_difftime(struct timeval *p_start,
		struct timeval *p_end) {
	return (p_end->tv_sec - p_start->tv_sec) * 1000000
			+ (p_end->tv_usec - p_start->tv_usec);
}

int32_t YangVideoCaptureLinux::read_buffer() {

	if (ioctl(m_vd_id, VIDIOC_DQBUF, &m_buf) != 0) {
		yang_error("VIDIOC_DQBUF");
		exit(1);
	}
	if (m_isFirstFrame) {
		m_timestatmp = m_difftime(&m_startTime, &m_buf.timestamp);
	} else {
		m_isFirstFrame = 1;
		m_startTime.tv_sec = m_buf.timestamp.tv_sec;
		m_startTime.tv_usec = m_buf.timestamp.tv_usec;
		m_timestatmp = 0;
	}

	if (m_vhandle)
		m_vhandle->putBuffer(m_timestatmp, m_user_buffer[m_buf.index].start,
				m_user_buffer[m_buf.index].length);

	if (ioctl(m_vd_id, VIDIOC_QBUF, &m_buf) != 0) {
		yang_error("VIDIOC_QBUF");
		exit(1);
	}

	return Yang_Ok;
}
void YangVideoCaptureLinux::stopLoop() {
	m_isloop = 0;
}

void YangVideoCaptureLinux::stop_capturing() {
	enum v4l2_buf_type type;

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (-1 == ioctl(m_vd_id, VIDIOC_STREAMOFF, &type)) {
		yang_error("Fail to ioctl 'VIDIOC_STREAMOFF'");
		exit(EXIT_FAILURE);
	}

}
void YangVideoCaptureLinux::uninit_camer_device() {
	int32_t i = 0;

	for (i = 0; i < m_buffer_count; i++) {
		if (-1 == munmap(m_user_buffer[i].start, m_user_buffer[i].length)) {
			exit(EXIT_FAILURE);
		}
	}
}

void YangVideoCaptureLinux::close_camer_device() {
	if (-1 == close(m_vd_id)) {
		yang_error("Fail to close fd");
		exit(EXIT_FAILURE);
	}

}

void YangVideoCaptureLinux::startLoop() {
	for (int32_t i = 0; i < m_buffer_count; i++) {
		struct v4l2_buffer tV4L2buf;
		memset(&tV4L2buf, 0, sizeof(struct v4l2_buffer));
		tV4L2buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		tV4L2buf.memory = V4L2_MEMORY_MMAP;
		tV4L2buf.index = i;
		if (ioctl(m_vd_id, VIDIOC_QBUF, &tV4L2buf)) {
			yang_error("VIDIOC_QBUF");
		}
	}

	enum v4l2_buf_type v4l2type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(m_vd_id, VIDIOC_STREAMON, &v4l2type)) {
		yang_error("VIDIOC_STREAMON");
	}

	fd_set fds;
	struct timeval tv;
	int32_t r;
	FD_ZERO(&fds);
	FD_SET(m_vd_id, &fds);
	m_isloop = 1;
	m_vhandle->m_start_time = 0;

	while (m_isloop) {
		tv.tv_sec = 2;
		tv.tv_usec = 0;
		r = select(m_vd_id + 1, &fds, NULL, NULL, &tv);
		if (-1 == r) {
			if (EINTR == errno)
				continue;
			yang_error("video capture Fail to select");
			exit(EXIT_FAILURE);
		}
		if (0 == r) {
			yang_error("video capture select Timeout\n");
			exit(EXIT_FAILURE);
		}
		read_buffer();
	}

}
#endif
