//
// Copyright (c) 2019-2023 yanggaofeng
//

#include <yangcapture/mac/YangVideoCaptureMac.h>
#include <yangutil/yangavinfotype.h>

#if Yang_OS_APPLE
void yang_macv_on_video(uint8_t* data,uint32_t nb,uint64_t ts,void* user){
    YangVideoCaptureMac* mac;
    if(data==NULL || user==NULL) return;
    mac=(YangVideoCaptureMac*)user;
    mac->on_video(data,nb,ts);
}

YangVideoCaptureMac::YangVideoCaptureMac(YangVideoInfo *pcontext) {
	m_para = pcontext;

	m_vhandle = new YangVideoCaptureHandle(pcontext);
	cameraIndex = pcontext->vIndex;
	m_width = m_para->width;
	m_height = m_para->height;

    m_isloop = yangfalse;
    m_waitState = yangfalse;


    YangMacVideoCallback callback;
    callback.user=this;
    callback.on_video=yang_macv_on_video;

    m_device=(YangVideoDeviceMac*)yang_calloc(sizeof(YangVideoDeviceMac),1);

    yang_create_macVideo(m_device,(uint32_t)pcontext->width,(uint32_t)pcontext->height,(uint32_t)pcontext->frame,&callback);
    yang_thread_mutex_init(&m_lock,NULL);
    yang_thread_cond_init(&m_cond_mess,NULL);

    m_vhandle->setCaptureFormat(YangNv12);
}

YangVideoCaptureMac::~YangVideoCaptureMac() {
    if (m_isStart) {
		stop();
		while (m_isStart) {
			yang_usleep(1000);
		}
	}

	yang_delete(m_vhandle);
    yang_destroy_macVideo(m_device);
    yang_free(m_device);
    yang_thread_mutex_destroy(&m_lock);
    yang_thread_cond_destroy(&m_cond_mess);
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


int32_t YangVideoCaptureMac::init() {

    if(m_device&&m_device->init(m_device->session)!=Yang_Ok){
        yang_error("mac video capture init fail!");
        return 1;
    }

	return Yang_Ok;
}

void YangVideoCaptureMac::on_video(uint8_t* data,uint32_t nb,uint64_t ts){
    if (m_vhandle)
        m_vhandle->putBuffer(ts, data,nb);
}


long YangVideoCaptureMac::m_difftime(struct timeval *p_start,
		struct timeval *p_end) {
	return (p_end->tv_sec - p_start->tv_sec) * 1000000
			+ (p_end->tv_usec - p_start->tv_usec);
}


void YangVideoCaptureMac::stopLoop() {

    if(m_device&&m_device->stop(m_device->session)!=Yang_Ok){
        yang_error("mac video capture stop fail!");
        //return;
    }
    m_isloop = yangfalse;
    if (m_waitState) {
        yang_thread_mutex_lock(&m_lock);
        yang_thread_cond_signal(&m_cond_mess);
        yang_thread_mutex_unlock(&m_lock);

    }
}



void YangVideoCaptureMac::startLoop() {

    m_isloop=yangtrue;
    if(m_device&&m_device->start(m_device->session)!=Yang_Ok){
        yang_error("mac video capture start fail!");
        //return;
    }
    yang_thread_mutex_lock(&m_lock);
    while (m_isloop) {
        m_waitState = yangtrue;

        yang_thread_cond_wait(&m_cond_mess, &m_lock);
        m_waitState = yangfalse;
    }	            		//end while


    yang_thread_mutex_unlock(&m_lock);
}
#endif
