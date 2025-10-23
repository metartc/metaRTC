//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGCAPTURE_YangAudioCaptureMac_H_
#define YANGCAPTURE_YangAudioCaptureMac_H_
#include <yangaudiodev/YangAudioCaptureHandle.h>

#include <yangaudiodev/YangAudioCapture.h>
#include <yangavutil/audio/YangPreProcess.h>
#include <yangutil/buffer/YangAudioPlayBuffer.h>
#include <vector>

#if Yang_OS_APPLE
#include <yangaudiodev/mac/YangAudioMac.h>
using namespace std;

class YangAudioCaptureMac: public YangAudioCapture {
public:
	YangAudioCaptureMac(YangAVInfo *avinfo);
	~YangAudioCaptureMac();
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
    void on_audio(uint8_t* data,uint32_t nb);
    void setPlayAudioParam(int32_t  puid,YangAudioParam* audioParam);
    void setCaptureVolume(int32_t vol);
    void setPlayVolume(int32_t vol);
protected:
	void startLoop();
	void stopLoop();

private:
	YangAVInfo *m_avinfo;

	int32_t m_loops;
	int32_t m_channel;
	uint32_t  m_sample;

    yangbool m_isInited;

    YangAudioMac* m_macAudio;
    YangMacAudioCallback m_callback;
    YangFrame m_audioFrame;

};
#endif
#endif /* YANGCAPTURE_SRC_YANGAUDIOCAPTUREIMPL_H_ */
