//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGAPP_YangP2pCapture_H_
#define YANGAPP_YangP2pCapture_H_
#include <yangutil/yangtype.h>
#include <yangaudiodev/YangAudioCapture.h>
#include <yangutil/sys/YangThread.h>
#include <yangcapture/YangMultiVideoCapture.h>
#include <yangcapture/YangScreenCapture.h>
#include <yangutil/buffer/YangAudioBuffer.h>
#include <yangutil/buffer/YangVideoBuffer.h>
#include <yangavutil/audio/YangRtcAec.h>
#include <yangutil/buffer/YangAudioPlayBuffer.h>
class YangP2pCapture :public YangThread {
public:
	YangP2pCapture(YangContext *pcontext);
	virtual ~YangP2pCapture();
public:
        void startCamera();
        void stopCamera();

	void startVideoCaptureState();
	void stopVideoCaptureState();

	int32_t initVideo();
	int32_t initScreen();
	void startVideoCapture();
	void startScreenCapture();
	YangVideoBuffer * getOutVideoBuffer();
	YangVideoBuffer * getPreVideoBuffer();


	void stopAll();
	void change(int32_t st);



#if Yang_HaveVr
	void addVr();
	void delVr();
#endif
private:
	YangAudioCapture *m_audioCapture;
	YangMultiVideoCapture *m_videoCapture;

	YangVideoBuffer *m_out_videoBuffer;
	YangVideoBuffer *m_pre_videoBuffer;

	YangContext *m_context;
	YangAudioBuffer *m_out_audioBuffer;
	YangRtcAec *m_aec;
#if Yang_HaveVr
        YangVideoBuffer *m_out_vr_pre_videoBuffer;
#endif
public:
	void stop();
	int32_t m_isStart;
protected:
	void run();
#if Yang_HaveVr
        void startLoop();
#else
	void startLoop(){};
 #endif
	void stopLoop();
	void initVr();

	int32_t m_isConvert;
public:
	int32_t initAudio(YangPreProcess *pp);
	int32_t initAudio2(YangPreProcess *pp);
	void startAudioCapture();
	YangAudioBuffer* getOutAudioBuffer();
	void stopAudioCaptureState();
	void startAudioCaptureState();
	void setAec(YangRtcAec *paec);
	void setInAudioBuffer(vector<YangAudioPlayBuffer*> *pbuf);

};

#endif /* YANGAPP_YANGCAPTUREAPP_H_ */
