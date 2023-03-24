
//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGAPP_YangPushCapture_H_
#define YANGAPP_YangPushCapture_H_
#include <yangutil/yangtype.h>
#include <yangaudiodev/YangAudioCapture.h>
#include <yangutil/sys/YangThread2.h>
#include <yangcapture/YangMultiVideoCapture.h>
#include <yangcapture/YangScreenCapture.h>
#include <yangutil/buffer/YangAudioBuffer.h>
#include <yangutil/buffer/YangVideoBuffer.h>
#include <yangavutil/audio/YangRtcAec.h>
#include <yangutil/buffer/YangAudioPlayBuffer.h>
class YangPushCapture :public YangThread {
public:
	YangPushCapture(YangContext *pcontext);
	virtual ~YangPushCapture();
public:
        void startCamera();

        void stopCamera();

        void setScreenInterval(int32_t pinterval);
        void setDrawmouse(bool isDraw);
	void startVideoCaptureState();
	void startScreenCaptureState();
	void stopVideoCaptureState();
	void stopScreenCaptureState();
	int32_t initVideo();

	void startVideoCapture();
	void startScreenCapture();
	YangVideoBuffer * getOutVideoBuffer();
	YangVideoBuffer * getPreVideoBuffer();

	YangVideoBuffer * getScreenOutVideoBuffer();
	YangVideoBuffer * getScreenPreVideoBuffer();
	void stopAll();
	void change(int32_t st);



#if Yang_Enable_Vr
	void addVr();
	void delVr();
#endif
private:
	YangAudioCapture *m_audioCapture;
	YangMultiVideoCapture *m_videoCapture;

	YangVideoBuffer *m_out_videoBuffer;
	YangVideoBuffer *m_pre_videoBuffer;

	YangVideoBuffer *m_screen_pre_videoBuffer;
	YangVideoBuffer *m_screen_out_videoBuffer;
	YangScreenCapture* m_screenCapture;

	YangContext *m_context;
	YangAudioBuffer *m_out_audioBuffer;

#if Yang_Enable_Vr
        YangVideoBuffer *m_out_vr_pre_videoBuffer;
#endif
public:
	void stop();
	int32_t m_isStart;
protected:
	void run();
#if Yang_Enable_Vr
        void startLoop();
#else
	void startLoop(){};
 #endif
	void stopLoop();
	void initVr();

	int32_t m_isConvert;
public:
	int32_t initAudio(YangPreProcess *pp);
	void startAudioCapture();
	YangAudioBuffer* getOutAudioBuffer();
	void stopAudioCaptureState();
	void startAudioCaptureState();
	void setAec(YangRtcAec *paec);
	void setInAudioBuffer(vector<YangAudioPlayBuffer*> *pbuf);
};

#endif /* YANGAPP_YANGCAPTUREAPP_H_ */
