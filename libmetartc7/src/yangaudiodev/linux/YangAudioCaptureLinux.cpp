//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangutil/yangtype.h>
#if Yang_OS_LINUX
#include <yangaudiodev/linux/YangAudioCaptureLinux.h>
#include <yangavutil/audio/YangAudioUtil.h>


YangAudioCaptureLinux::YangAudioCaptureLinux(YangAVInfo *avinfo) //:YangAudioCapture(pcontext)
		{

	m_avinfo = avinfo;
	m_ahandle = new YangAudioCaptureHandle(avinfo);
	aIndex = 0;
	m_size = 0;
	m_loops = 0;
	m_handle = NULL;
	m_buffer = NULL;

	m_channel = avinfo->audio.channel;
	m_sample = avinfo->audio.sample;
	if (avinfo->audio.audioEncoderType == Yang_AED_AAC) {
		m_frames = 1024;
	} else {
		m_frames = m_sample / 50;
	}

	onlySupportSingle = yangfalse;
	m_readN=0;

}

YangAudioCaptureLinux::~YangAudioCaptureLinux() {
	if (m_loops) {
		stop();
		while (m_isStart) {
			yang_usleep(500);
		}
	}
	if (m_handle) {

		snd_pcm_close(m_handle);
		m_handle = NULL;
	}

	yang_delete(m_buffer);
	yang_delete(m_ahandle);
}
void YangAudioCaptureLinux::setCatureStart() {
	m_ahandle->m_enableBuf = 1;
}
void YangAudioCaptureLinux::setCatureStop() {
	m_ahandle->m_enableBuf = 0;
}
void YangAudioCaptureLinux::setOutAudioBuffer(YangAudioBuffer *pbuffer) {
	m_ahandle->setOutAudioBuffer(pbuffer);
}
void YangAudioCaptureLinux::setPlayAudoBuffer(YangAudioBuffer *pbuffer) {
	m_ahandle->m_aecPlayBuffer = pbuffer;
}
void YangAudioCaptureLinux::setAec(YangRtcAec *paec) {
	m_ahandle->m_aec = paec;
}
void YangAudioCaptureLinux::setInAudioBuffer(
		vector<YangAudioPlayBuffer*> *pal) {

}
void YangAudioCaptureLinux::setPreProcess(YangPreProcess *pp) {

}

int32_t YangAudioCaptureLinux::init() {
	int32_t dir = 0;
	snd_pcm_hw_params_t *hw_params;
	int32_t err = 0;
	char device_name[64] = { 0 };
	if (m_avinfo->audio.aIndex > -1)
		sprintf(device_name, "hw:%d,%d", m_avinfo->audio.aIndex,
				m_avinfo->audio.aSubIndex);

	if ((err = snd_pcm_open(&m_handle,
			m_avinfo->audio.aIndex == -1 ? "default" : device_name,
			SND_PCM_STREAM_CAPTURE, 0)) < 0) {

		yang_error("unable to open pcm device: %s\n", snd_strerror(err));
		_exit(1);
	}
	if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0) {

		yang_error("cannot allocate hardware parameter structure (%s)\n",
				snd_strerror(err));
		_exit(1);
	}

	if ((err = snd_pcm_hw_params_any(m_handle, hw_params)) < 0) {

		yang_error("cannot initialize hardware parameter structure (%s)\n",
				snd_strerror(err));
		_exit(1);
	}

	if ((err = snd_pcm_hw_params_set_access(m_handle, hw_params,
			SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {

		yang_error("cannot set access type (%s)\n", snd_strerror(err));
		_exit(1);
	}

	if ((err = snd_pcm_hw_params_set_format(m_handle, hw_params,
			SND_PCM_FORMAT_S16_LE)) < 0) {

		yang_error("cannot set sample format (%s)\n", snd_strerror(err));
		_exit(1);
	}

	if ((err = snd_pcm_hw_params_set_rate_near(m_handle, hw_params, &m_sample,
			0)) < 0) {

		yang_error("cannot set sample rate (%s)\n", snd_strerror(err));
		_exit(1);
	}

	if ((err = snd_pcm_hw_params_set_channels(m_handle, hw_params, m_channel))
			< 0) {
		yang_error("cannot set double channel  (%s)\n", snd_strerror(err));
		err = snd_pcm_hw_params_set_channels(m_handle, hw_params, 1);
		if (err < 0) {
			yang_error("cannot set single channel  (%s)\n", snd_strerror(err));
			_exit(1);
		}
		onlySupportSingle = yangtrue;
		//_exit(1);
	}

	if ((err = snd_pcm_hw_params_set_period_size_near(m_handle, hw_params,
			&m_frames, &dir)) < 0) {

		yang_error("cannot set period size (%s)\n", snd_strerror(err));
		_exit(1);
	}

	if ((err = snd_pcm_hw_params(m_handle, hw_params)) < 0) {

		yang_error("cannot set parameters (%s)\n", snd_strerror(err));
		_exit(1);
	}

	snd_pcm_hw_params_free(hw_params);
	m_size = m_frames * 2 * m_channel; // 2 bytes/sample, 2 channels
	m_buffer = (uint8_t*) malloc(m_size);
	return Yang_Ok;
}
int32_t YangAudioCaptureLinux::alsa_device_capture_ready(struct pollfd *pfds,
		uint32_t  nfds) {
	unsigned short revents = 0;
	int32_t ret=0;
	if ((ret = snd_pcm_poll_descriptors_revents(m_handle, pfds,
			m_readN, &revents)) < 0) {

		yang_error("error in alsa_device_capture_ready: %s",snd_strerror(ret));
		return pfds[0].revents & POLLIN;
	}

	return revents & POLLIN;
}

int32_t YangAudioCaptureLinux::alsa_device_read(short *pcm, int32_t len) {
	int32_t ret=0;
	if ((ret = snd_pcm_readi(m_handle, pcm, len)) != len) {
		if (ret < 0) {
			if (ret == -EPIPE) {

				yang_error("An overrun has occured, reseting capture");
			} else {

				yang_error("read from audio interface failed (%s)",
						snd_strerror(ret));

			}
			if ((ret = snd_pcm_prepare(m_handle)) < 0) {

				yang_error("cannot prepare audio interface for use (%s)",
						snd_strerror(ret));
			}
			if ((ret = snd_pcm_start(m_handle)) < 0) {

				yang_error("cannot prepare audio interface for use (%s)",
						snd_strerror(ret));
			}

		} else {

			yang_error(
					"Couldn't read as many samples as I wanted (%d instead of %d)",
					ret, len);
		}
		return 1;
	}
	return Yang_Ok;
}

void YangAudioCaptureLinux::startLoop() {

	m_loops = 1;
	unsigned long status = 0;
	int32_t err=0;
	uint8_t *tmp = NULL;
	if (onlySupportSingle) {
		tmp = new uint8_t[m_frames * 2 * 2];
	}
	if ((status = snd_pcm_prepare(m_handle)) < 0) {

		yang_error("cannot prepare audio interface for use (%s)\n",
				snd_strerror(status));
		_exit(1);
	}
	int32_t audiolen = m_frames * m_channel * 2;
	m_readN = snd_pcm_poll_descriptors_count(m_handle);
	pollfd* read_fd = (pollfd*) malloc(m_readN * sizeof(*read_fd));
		if ((err=snd_pcm_poll_descriptors(m_handle, read_fd,
				m_readN) )!= m_readN) {

			yang_error("cannot obtain capture file descriptors (%s)",
					snd_strerror(err));
			_exit(1);
		}

	int32_t nfds=m_readN;

	struct pollfd *pfds = (pollfd*) yang_malloc(sizeof(struct pollfd) * nfds);
	pfds[0]=read_fd[0];
	while (m_loops) {
		poll(pfds, nfds, -1);
		if (alsa_device_capture_ready(pfds, nfds)) {
			alsa_device_read((short*) m_buffer, m_frames);

			if (onlySupportSingle) {
				MonoToStereo((int16_t*) m_buffer, (int16_t*) tmp, m_frames);
				m_ahandle->putBuffer2(tmp, audiolen);
			} else {
				m_ahandle->putBuffer2(m_buffer, audiolen);
			}
		}

	}

	snd_pcm_close(m_handle);
	yang_deleteA(tmp);
	yang_free(m_buffer);
	yang_free(pfds);
	m_handle = NULL;

}

void YangAudioCaptureLinux::stopLoop() {
	m_loops = 0;
}
#endif
