//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef Yang_ALSA_DEVICE_H1
#define Yang_ALSA_DEVICE_H1
#include <yangaudiodev/YangAudioCaptureHandle.h>
#include <yangaudiodev/YangAudioRenderData.h>
#include <yangaudiodev/YangAudioCapture.h>

#include <yangavutil/audio/YangPreProcess.h>

#if Yang_OS_LINUX
#include <sys/poll.h>
#include <alsa/asoundlib.h>

struct YangAlsaDevice {
	char *device_name;
	int32_t channels;
	int32_t period;
	snd_pcm_t *capture_handle;
	snd_pcm_t *playback_handle;
	int32_t readN, writeN;
	struct pollfd *read_fd, *write_fd;
};

class YangAudioAecLinux: public YangAudioCapture {
public:
	YangAudioAecLinux(YangAVInfo *avinfo,YangSynBufferManager* streams);
	~YangAudioAecLinux();

public:
	YangAudioCaptureHandle *m_ahandle;
	int32_t init();
	void setInAudioBuffer(vector<YangAudioPlayBuffer*> *pal);
	void setPreProcess(YangPreProcess *pp);
	void setCatureStart();
	void setCatureStop();
    void setOutAudioBuffer(YangAudioBuffer *pbuffer);
	void setPlayAudoBuffer(YangAudioBuffer *pbuffer);
    void setAec(YangRtcAec *paec);

protected:
	void run();

	void startLoop();

	void stopLoop();
	YangAudioRenderData m_audioData;
private:

	uint8_t *m_buffer;
	yangbool m_isInit;
	int32_t m_ret;

	YangPreProcess *m_preProcess;

	int32_t alsa_device_open(char *device_name, uint32_t  rate,
				int32_t channels, int32_t period);
	void alsa_device_close();
	int32_t alsa_device_read(short *pcm, int32_t len);
	int32_t alsa_device_write(const short *pcm, int32_t len);
	int32_t alsa_device_capture_ready(struct pollfd *pfds, uint32_t  nfds);
	int32_t alsa_device_playback_ready(struct pollfd *pfds, uint32_t  nfds);

	int32_t alsa_device_nfds();
	void alsa_device_getfds(struct pollfd *pfds, uint32_t  nfds);

private:
	YangAVInfo* m_avinfo;
	YangAlsaDevice *m_dev;
	int32_t playDeviceState=1;
	int32_t catpureDeviceState = 1;
	int32_t m_size;
	yangbool m_loops;
	int32_t m_captureChannel;
	uint32_t  m_captureSample;
	uint32_t m_sample,m_channel;
	snd_pcm_uframes_t m_captureFrames,m_frames;
	int32_t m_audioPlayCacheNum;

};

#endif
#endif
