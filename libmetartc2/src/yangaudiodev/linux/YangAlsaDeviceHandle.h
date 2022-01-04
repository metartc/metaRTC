
#ifndef Yang_ALSA_DEVICE_H1
#define Yang_ALSA_DEVICE_H1

#ifndef _WIN32
#include <sys/poll.h>
#include <alsa/asoundlib.h>
//#include <yangavutil/audio/YangAudioMix.h>
#include <yangavutil/audio/YangPreProcess.h>
//#include <yangavutil/audio/YangResample.h>
#include <yangaudiodev/YangAudioCapture.h>
#include <yangaudiodev/YangAudioCaptureHandle.h>
#include <yangaudiodev/YangAudioRenderData.h>
struct YangAlsaDevice {
	char *device_name;
	int32_t channels;
	int32_t period;
	snd_pcm_t *capture_handle;
	snd_pcm_t *playback_handle;
	int32_t readN, writeN;
	struct pollfd *read_fd, *write_fd;
};

class YangAlsaDeviceHandle: public YangAudioCapture {
public:
	YangAlsaDeviceHandle(YangContext *pcontext);
	~YangAlsaDeviceHandle();
	int32_t alsa_device_open(char *device_name, uint32_t  rate,
			int32_t channels, int32_t period);
public:
	YangAudioCaptureHandle *m_ahandle;
	int32_t init();
	void setInAudioBuffer(vector<YangAudioPlayBuffer*> *pal);
	void setPreProcess(YangPreProcess *pp);
	void setCatureStart();
	void setCatureStop();
    void setOutAudioBuffer(YangAudioBuffer *pbuffer);
	void setPlayAudoBuffer(YangAudioBuffer *pbuffer);
    void setAec(YangAecBase *paec);

protected:
	void run();

	void startLoop();

	void stopLoop();
	YangAudioRenderData m_audioData;
private:
    //YangResample *m_resample;
	//vector<YangAudioPlayBuffer*> *m_in_audioBuffer;
	uint8_t *m_buffer;
	int32_t m_isInit;
	int32_t m_ret;
	YangAudioMix m_mix;
	YangPreProcess *m_preProcess;
	//int32_t hasData();

	void alsa_device_close();
	int32_t alsa_device_read(short *pcm, int32_t len);
	int32_t alsa_device_write(const short *pcm, int32_t len);
	int32_t alsa_device_capture_ready(struct pollfd *pfds, uint32_t  nfds);
	int32_t alsa_device_playback_ready(struct pollfd *pfds, uint32_t  nfds);
	//void alsa_device_start();
	int32_t alsa_device_nfds();
	void alsa_device_getfds(struct pollfd *pfds, uint32_t  nfds);

private:
	YangAlsaDevice *m_dev;
	int32_t playDeviceState=1;
	int32_t catpureDeviceState = 1;
	int32_t m_size;
	int32_t m_loops;
	int32_t m_channel;
	uint32_t  m_sample;
	snd_pcm_uframes_t m_frames;
	int32_t m_audioPlayCacheNum;
};

#endif
#endif
