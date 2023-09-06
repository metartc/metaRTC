//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGCAPTURE_SRC_YANGAUDIOCAPTUREIMPL_H_
#define YANGCAPTURE_SRC_YANGAUDIOCAPTUREIMPL_H_
#include <yangaudiodev/YangAudioCaptureHandle.h>
#include <yangaudiodev/YangAudioCapture.h>
#include <yangavutil/audio/YangPreProcess.h>
#include <vector>

#if Yang_OS_LINUX
#include <alsa/asoundlib.h>
using namespace std;

class YangAudioCaptureLinux: public YangAudioCapture {
public:
	YangAudioCaptureLinux(YangAVInfo *avinfo);
	~YangAudioCaptureLinux();
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
	int32_t alsa_device_capture_ready(struct pollfd *pfds,uint32_t  nfds);
	int32_t alsa_device_read(short *pcm, int32_t len);
private:
	YangAVInfo *m_avinfo;
	int32_t m_size;
	int32_t m_loops;
	int32_t m_channel;
	uint32_t  m_sample;
	int32_t  m_readN ;
	snd_pcm_uframes_t m_frames;
	uint8_t *m_buffer;
	snd_pcm_t *m_handle;
	yangbool onlySupportSingle;

};
#endif
#endif /* YANGCAPTURE_SRC_YANGAUDIOCAPTUREIMPL_H_ */
