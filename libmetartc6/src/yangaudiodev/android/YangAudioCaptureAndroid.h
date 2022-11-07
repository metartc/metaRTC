//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGCAPTURE_SRC_YANGAUDIOCAPTUREIMPL_H_
#define YANGCAPTURE_SRC_YANGAUDIOCAPTUREIMPL_H_
#include <yangaudiodev/YangAudioCaptureHandle.h>
#include <yangaudiodev/YangAudioCapture.h>
#include "yangavutil/audio/YangPreProcess.h"
#ifdef __ANDROID__
#include <yangaudiodev/android/YangAudioAndroid.h>
#include <vector>
using namespace std;


class YangAudioCaptureAndroid: public YangAudioCapture {
public:
	YangAudioCaptureAndroid(YangContext *pcontext);
	~YangAudioCaptureAndroid();
public:
	YangAudioCaptureHandle *m_ahandle;
	int32_t init();
	void setPreProcess(YangPreProcess *pp);
	void setCatureStart();
	void setCatureStop();
	void setOutAudioBuffer(YangAudioBuffer *pbuffer);
	void setPlayAudoBuffer(YangAudioBuffer *pbuffer);
	void setInAudioBuffer(vector<YangAudioPlayBuffer*> *pal);
	void setAec(YangRtcAec *paec);


protected:
	void startLoop();
	void stopLoop();
	void closeAudio();
 	YangAudioAndroid* m_audioAndroid;
private:
	int32_t m_size;
	int32_t m_loops;
	int32_t m_channel;
	uint32_t  m_sample;
};
#endif
#endif /* YANGCAPTURE_SRC_YANGAUDIOCAPTUREIMPL_H_ */
