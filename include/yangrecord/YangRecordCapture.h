//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGAPP_YangReczbCapture_H_
#define YANGAPP_YangReczbCapture_H_
#include <yangavutil/audio/YangRtcAec.h>
#include <yangavutil/audio/YangPreProcess.h>
#include <yangaudiodev/YangAudioCapture.h>
#include <yangcapture/YangCaptureFactory.h>
#include <yangcapture/YangMultiVideoCapture.h>
#include <vector>
#include "yangutil/buffer/YangAudioBuffer.h"
#include "yangutil/buffer/YangVideoBuffer.h"
#include "yangutil/sys/YangIni.h"
#include "yangutil/buffer/YangAudioPlayBuffer.h"
#include "yangutil/sys/YangThread.h"
using namespace std;

class YangRecordCapture:public YangThread {
public:
	YangRecordCapture(YangContext* pcontext);
	virtual ~YangRecordCapture();
public:
	void startAudioCaptureState();
	void startVideoCaptureState();
	void stopAudioCaptureState();
	void stopVideoCaptureState();
	void startPauseCaptureState();
	void stopPauseCaptureState();
	void initAudio(YangPreProcess *pp);
	void initVideo();
	void startAudioCapture();
	void startVideoCapture();
	void stopAudioCapture();
	void stopVideoCapture();
	void setAec(YangRtcAec *paec);
	void setInAudioBuffer(vector<YangAudioPlayBuffer*> *pbuf);

	void startVr(char* pbg);
	void stopVr();

	void startScreen();
	void stopScreen();

	YangAudioBuffer * getOutAudioBuffer();
	YangVideoBuffer * getOutVideoBuffer();
	YangVideoBuffer * getPreVideoBuffer();
	YangAudioCapture *m_audioCapture;
        //YangMultiVideoCapture *m_videoCapture;
        YangVideoCapture *m_videoCapture;
	YangScreenCapture *m_screenCapture;

	int32_t m_isStart;
	void stop();
protected:
	void run();
	void startVrLoop();
	void startScreenLoop();
	void stopLoop();
	int32_t m_isConvert;
	int32_t m_isScreen;
	string m_bgFileName;
private:

	YangContext* m_context;
	YangCaptureFactory m_capture;
	YangAudioBuffer *m_out_audioBuffer;
	YangVideoBuffer *m_out_videoBuffer;
	YangVideoBuffer *m_pre_videoBuffer;

	YangVideoBuffer *m_vr_videoBuffer;
	YangVideoBuffer *m_screen_videoBuffer;


};

#endif /* YANGAPP_YANGCAPTUREAPP_H_ */
