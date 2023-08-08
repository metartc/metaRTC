//
// Copyright (c) 2019-2023 yanggaofeng
//

#include <yangcapture/mac/YangVideoCaptureMac.h>
#include <yangutil/yangavinfotype.h>

#if defined(__APPLE__)



YangVideoCaptureMac::YangVideoCaptureMac(YangVideoInfo *pcontext) {

	m_para = pcontext;

	m_vhandle = new YangVideoCaptureHandle(pcontext);
	cameraIndex = pcontext->vIndex;
	m_width = m_para->width;
	m_height = m_para->height;
	m_vd_id = 0;


	m_isloop = 0;
	m_isFirstFrame = 0;
	m_buffer_count = 0;
	m_timestatmp = 0;
	m_hasYuy2 = 0, m_hasI420 = 0, m_hasNv12 = 0, m_hasYv12 = 0, m_hasP010 = 0,m_hasP016=0;
}

YangVideoCaptureMac::~YangVideoCaptureMac() {
	if (m_isloop) {
		stop();
		while (m_isStart) {
			yang_usleep(1000);
		}
	}

	yang_delete(m_vhandle);
}
void YangVideoCaptureMac::setVideoCaptureStart() {
	m_vhandle->m_isCapture = 1;
}
void YangVideoCaptureMac::setVideoCaptureStop() {
	m_vhandle->m_isCapture = 0;
}
int32_t YangVideoCaptureMac::getVideoCaptureState() {
	return m_vhandle->m_isCapture;
}
int32_t YangVideoCaptureMac::getLivingVideoCaptureState() {
	return m_vhandle->m_isLivingCaptrue;
}
int32_t YangVideoCaptureMac::getFilmVideoCaptureState() {
	return m_vhandle->m_isFilm;
}

void YangVideoCaptureMac::setLivingVideoCaptureStart() {
	m_vhandle->m_isLivingCaptrue = 1;
}
void YangVideoCaptureMac::setLivingVideoCaptureStop() {
	m_vhandle->m_isLivingCaptrue = 0;
}

void YangVideoCaptureMac::setFilmVideoCaptureStart() {
	m_vhandle->m_isFilm = 1;
}
void YangVideoCaptureMac::setFilmVideoCaptureStop() {
	m_vhandle->m_isFilm = 0;
}

void YangVideoCaptureMac::setOutVideoBuffer(YangVideoBuffer *pbuf) {
	m_vhandle->setVideoBuffer(pbuf);
}
void YangVideoCaptureMac::setLivingOutVideoBuffer(YangVideoBuffer *pbuf) {
	m_vhandle->setLivingVideoBuffer(pbuf);
}
void YangVideoCaptureMac::setFilmOutVideoBuffer(YangVideoBuffer *pbuf) {
	m_vhandle->setFilmVideoBuffer(pbuf);
}
void YangVideoCaptureMac::setPreVideoBuffer(YangVideoBuffer *pbuf) {
	m_vhandle->setPreVideoBuffer(pbuf);
}
void YangVideoCaptureMac::initstamp() {
	m_vhandle->initstamp();
}
int32_t YangVideoCaptureMac::setPara() {


	return Yang_Ok;
}

int32_t YangVideoCaptureMac::init() {




	return Yang_Ok;
}

long YangVideoCaptureMac::m_difftime(struct timeval *p_start,
		struct timeval *p_end) {
	return (p_end->tv_sec - p_start->tv_sec) * 1000000
			+ (p_end->tv_usec - p_start->tv_usec);
}

int32_t YangVideoCaptureMac::read_buffer() {



	return Yang_Ok;
}
void YangVideoCaptureMac::stopLoop() {
	m_isloop = 0;
}

void YangVideoCaptureMac::stop_capturing() {


}
void YangVideoCaptureMac::uninit_camer_device() {



}

void YangVideoCaptureMac::close_camer_device() {


}

void YangVideoCaptureMac::startLoop() {



}
#endif
