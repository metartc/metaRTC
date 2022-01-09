#include <yangutil/yang_unistd.h>
#include <yangpush/YangPushCapture.h>
#include <yangavutil/video/YangYuvConvert.h>
#include <yangavutil/video/YangYuvUtil.h>
//#include <yangavutil/video/YangMatImage.h>
//#include <yangavutil/vr/YangMatImageCv.h>
#include <yangcapture/YangCaptureFactory.h>


YangPushCapture::YangPushCapture(YangContext *pcontext) {
	m_context=pcontext;

	m_out_audioBuffer = NULL;
	m_screenCapture=NULL;
	m_videoCapture=NULL;
	m_audioCapture=NULL;


	m_out_videoBuffer=NULL;
	m_screen_pre_videoBuffer=NULL;
	m_screen_out_videoBuffer=NULL;
	m_pre_videoBuffer=new YangVideoBuffer(pcontext->video.bitDepth==8?1:2);
	m_pre_videoBuffer->isPreview=1;
	m_pre_videoBuffer->m_frames=pcontext->video.frame;
	m_isConvert=0;
	m_isStart=0;
#if Yang_HaveVr
       m_out_vr_pre_videoBuffer=NULL;
#endif
}

YangPushCapture::~YangPushCapture() {
	m_context=NULL;
	stopAll();
	yang_stop_thread(this);
	yang_stop_thread(m_audioCapture);
	yang_stop_thread(m_videoCapture);
	yang_stop_thread(m_screenCapture);


	yang_delete(m_audioCapture);
	yang_delete(m_videoCapture);
	yang_delete(m_screenCapture);

	yang_delete(m_out_audioBuffer);
	yang_delete(m_pre_videoBuffer);
	yang_delete(m_out_videoBuffer);
#if Yang_HaveVr
       yang_delete(m_out_vr_pre_videoBuffer);
#endif
	m_screen_pre_videoBuffer=NULL;
	m_screen_out_videoBuffer=NULL;
	//yang_delete(m_screen_pre_videoBuffer);

}


void YangPushCapture::startAudioCaptureState() {
    if(m_audioCapture) m_audioCapture->setCatureStart();
}
void YangPushCapture::stopAudioCaptureState() {
    if(m_audioCapture) m_audioCapture->setCatureStop();
}
void YangPushCapture::setAec(YangAecBase *paec) {
	if (m_audioCapture) {
		m_audioCapture->setAec(paec);
	}
}
void YangPushCapture::setInAudioBuffer(vector<YangAudioPlayBuffer*> *pbuf){
	if(m_audioCapture!=NULL) m_audioCapture->setInAudioBuffer(pbuf);
}
void YangPushCapture::startAudioCapture() {
	if (m_audioCapture && !m_audioCapture->m_isStart)
		m_audioCapture->start();
}
YangAudioBuffer* YangPushCapture::getOutAudioBuffer() {
	return m_out_audioBuffer;
}
int32_t YangPushCapture::initAudio(YangPreProcess *pp) {
	if (m_out_audioBuffer == NULL) {
		if (m_context->audio.usingMono)
			m_out_audioBuffer = new YangAudioBuffer(m_context->audio.audioCacheNum);
		else
			m_out_audioBuffer = new YangAudioBuffer(m_context->audio.audioCacheNum);
	}
	if (m_audioCapture == NULL) {
		YangCaptureFactory m_capture;
		m_audioCapture = m_capture.createRecordAudioCapture(m_context); //new YangAudioCapture(m_context);
		int32_t ret=m_audioCapture->init();
		if(ret){
			if(ret==ERROR_SYS_NoAudioDevice||ret==ERROR_SYS_NoAudioCaptureDevice) {
				yang_error("ERROR_SYS_NoAudioDevice");
					return ret;
			}

		}
		m_audioCapture->setPreProcess(pp);
		m_audioCapture->setOutAudioBuffer(m_out_audioBuffer);

		m_audioCapture->setCatureStop();
	}
	stopAudioCaptureState();
	return Yang_Ok;
}




void YangPushCapture::stopAll(){
	stop();
	yang_stop(m_audioCapture);
	yang_stop(m_videoCapture);
	yang_stop(m_screenCapture);

}


void YangPushCapture::startVideoCaptureState() {
	m_videoCapture->initstamp();
	m_videoCapture->setVideoCaptureStart();
}

void YangPushCapture::startScreenCaptureState() {

	m_screenCapture->setVideoCaptureStart();
}

void YangPushCapture::stopVideoCaptureState() {
	if(m_videoCapture) m_videoCapture->setVideoCaptureStop();

}
void YangPushCapture::stopScreenCaptureState(){
	if(m_screenCapture) m_screenCapture->setVideoCaptureStop();
}
void YangPushCapture::change(int32_t st) {

}


int32_t YangPushCapture::initVideo(){
	if(m_out_videoBuffer==NULL) m_out_videoBuffer = new YangVideoBuffer(m_context->video.bitDepth==8?1:2);
	int32_t err=Yang_Ok;
	if (m_videoCapture == NULL) {
		YangCaptureFactory cf;

		m_videoCapture = cf.createRecordVideoCapture(&m_context->video);//new YangVideoCapture(m_context);
		if((err=m_videoCapture->init())!=Yang_Ok){
			return yang_error_wrap(err,"video capture init fail!");
		}

		m_out_videoBuffer->init(m_context->video.width,m_context->video.height,m_context->video.videoEncoderFormat);
		m_pre_videoBuffer->init(m_context->video.width,m_context->video.height,m_context->video.videoEncoderFormat);
		m_videoCapture->setOutVideoBuffer(m_out_videoBuffer);
		m_videoCapture->setPreVideoBuffer(m_pre_videoBuffer);
		//m_videoCapture->setVideoCaptureStart();
	}
	stopVideoCaptureState();
	return err;

}

int32_t YangPushCapture::initScreen(){
	int32_t err=Yang_Ok;
	if (m_screenCapture == NULL) {
		YangCaptureFactory cf;

		m_screenCapture = cf.createScreenCapture(m_context);//new YangVideoCapture(m_context);

		if((err=m_screenCapture->init())!=Yang_Ok){
			return yang_error_wrap(err,"screen capture fail....");
		}

		m_screen_out_videoBuffer=m_screenCapture->getOutVideoBuffer();
		m_screen_pre_videoBuffer=m_screenCapture->getPreVideoBuffer();

		//m_screenCapture->setVideoCaptureStart();
	}
	stopVideoCaptureState();
	return err;
}

void YangPushCapture::startVideoCapture(){
		if(m_videoCapture&&!m_videoCapture->m_isStart) m_videoCapture->start();
}
void YangPushCapture::startScreenCapture(){
	if(m_screenCapture&&!m_screenCapture->m_isStart) m_screenCapture->start();
}
YangVideoBuffer * YangPushCapture::getOutVideoBuffer(){

	return m_out_videoBuffer;
}

YangVideoBuffer * YangPushCapture::getPreVideoBuffer(){

	return m_pre_videoBuffer;
}
void YangPushCapture::run() {
	m_isStart = 1;
	startLoop();
	m_isStart = 0;
}
void YangPushCapture::stop() {
	stopLoop();
}
void YangPushCapture::stopLoop() {
	m_isConvert = 0;
}
#if Yang_HaveVr
#include <yangavutil/vr/YangMatImageCv.h>
void YangPushCapture::startLoop() {

	m_isConvert = 1;
	int32_t inWidth = m_context->video.width;
	int32_t inHeight = m_context->video.height;
	int64_t  prestamp = 0;
	long az = inWidth * inHeight * 2;
    uint8_t *srcData=new uint8_t[az];// { 0 };
    //if (is12bits)
    az = inWidth * inHeight * 3 / 2;

        //mf.getYangMatImage();

    uint8_t *matDst=new uint8_t[inWidth * inHeight * 2];
    uint8_t *matSrcRgb=new uint8_t [inWidth * inHeight * 3];
    uint8_t *matSrcBgr=new uint8_t [inWidth * inHeight * 3];
    YangYuvConvert yuv;


    YangMatImageCv *mat = new YangMatImageCv();
    mat->initImg(m_context->bgFilename, m_context->video.width, m_context->video.height, 3);
    yang_trace("bgfilename===%s",m_context->bgFilename);
    YangFrame videoFrame;
    memset(&videoFrame,0,sizeof(YangFrame));
	while (m_isConvert == 1) {

		if (m_out_vr_pre_videoBuffer->size() == 0) {
			yang_usleep(1000);
			continue;
		}
		videoFrame.payload=srcData;
		videoFrame.nb=az;
		m_out_vr_pre_videoBuffer->getVideo(&videoFrame);
        yuv.I420torgb24(srcData, matSrcRgb, inWidth, inHeight);
		yang_rgbtobgr(matSrcRgb, matSrcBgr, inWidth, inHeight);
         mat->matImage(matSrcBgr, matDst);

		if (videoFrame.timestamp - prestamp <= 0) {
			prestamp = videoFrame.timestamp;
			continue;
		}
		prestamp = videoFrame.timestamp;
		videoFrame.payload=matDst;
		videoFrame.nb=az;
		if (m_videoCapture->getVideoCaptureState())
			m_out_videoBuffer->putVideo(&videoFrame);
        m_pre_videoBuffer->putVideo(&videoFrame);

    }

	yang_delete(mat);
    yang_deleteA(srcData);
    yang_deleteA(matDst);
    yang_deleteA(matSrcRgb);
    yang_deleteA(matSrcBgr);


}
void YangPushCapture::addVr(){
	if (m_out_vr_pre_videoBuffer == NULL)
		m_out_vr_pre_videoBuffer = new YangVideoBuffer(m_context->video.width,
				m_context->video.height, 12, m_context->video.bitDepth == 8 ? 1 : 2);

	m_out_vr_pre_videoBuffer->resetIndex();

	m_pre_videoBuffer->resetIndex();
	m_out_videoBuffer->resetIndex();
	m_videoCapture->setPreVideoBuffer(m_out_vr_pre_videoBuffer);
	m_videoCapture->setOutVideoBuffer(NULL);//(m_out_vr_pre_videoBuffer);
	start();
}
void YangPushCapture::delVr(){
	stop();
	while(m_isStart){
		yang_usleep(1000);
	}
	m_pre_videoBuffer->resetIndex();
		m_out_videoBuffer->resetIndex();
	m_videoCapture->setOutVideoBuffer(m_out_videoBuffer);
		m_videoCapture->setPreVideoBuffer(m_pre_videoBuffer);
}
#endif
YangVideoBuffer* YangPushCapture::getScreenOutVideoBuffer() {
	return m_screen_out_videoBuffer;
}

YangVideoBuffer* YangPushCapture::getScreenPreVideoBuffer() {
	return m_screen_pre_videoBuffer;
}

void YangPushCapture::startCamera() {
	initVideo();
	startVideoCapture();
}

void YangPushCapture::startScreen() {
	initScreen();
	startScreenCapture();
}

void YangPushCapture::stopCamera() {
	yang_stop(m_videoCapture);
	yang_stop_thread(m_videoCapture);
	yang_delete(m_videoCapture);
}

void YangPushCapture::stopScreen() {
	yang_stop(m_screenCapture);
	yang_stop_thread(m_screenCapture);
	yang_delete(m_screenCapture);
}

void YangPushCapture::setScreenInterval(int32_t pinterval) {
	if(m_screenCapture) m_screenCapture->setInterval(pinterval);
}

void YangPushCapture::setDrawmouse(bool isDraw) {
	if(m_screenCapture) m_screenCapture->setDrawmouse(isDraw);
}
