//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangaudiodev/linux/YangAudioAecLinux.h>
#if Yang_OS_LINUX

YangAudioAecLinux::YangAudioAecLinux(YangAVInfo *avinfo,YangSynBufferManager* streams) {

	m_audioData.setContext(streams);
    m_ahandle = new YangAudioCaptureHandle(avinfo);
    m_avinfo=avinfo;
	m_audioPlayCacheNum = m_avinfo->audio.audioPlayCacheNum;
	aIndex = 0;
	m_ret = 0;
	m_size = 0;
	m_loops = yangfalse;

	m_buffer = NULL;
	m_isInit = yangfalse;
	m_dev = NULL;

	m_captureFrames = 320;
	m_captureChannel = 1;
	m_captureSample = 16000;

	m_sample= m_avinfo->audio.sample;
	m_channel= m_avinfo->audio.channel;
	m_frames=m_sample/50;

	m_preProcess = NULL;

	m_audioData.initPlay(m_sample,m_channel);
	m_audioData.initRender(m_captureSample,m_captureChannel);


}
YangAudioAecLinux::~YangAudioAecLinux() {

	if (m_isStart) {
		stop();
		while (m_isStart) {
			yang_usleep(1000);
		}
	}
	alsa_device_close();
	m_preProcess = NULL;
	yang_free(m_buffer);
	yang_delete(m_ahandle);
}

void YangAudioAecLinux::setCatureStart() {
	m_ahandle->m_enableBuf = yangtrue;
}

void YangAudioAecLinux::setCatureStop() {
	m_ahandle->m_enableBuf = yangfalse;
}

void YangAudioAecLinux::setOutAudioBuffer(YangAudioBuffer *pbuffer) {
	m_ahandle->setOutAudioBuffer(pbuffer);
}

void YangAudioAecLinux::setPlayAudoBuffer(YangAudioBuffer *pbuffer) {
	m_ahandle->m_aecPlayBuffer = pbuffer;
}

void YangAudioAecLinux::setAec(YangRtcAec *paec) {
	m_ahandle->m_aec = paec;
}

void YangAudioAecLinux::setPreProcess(YangPreProcess *pp) {
	m_preProcess = pp;
	m_audioData.m_preProcess=pp;
}

int32_t YangAudioAecLinux::alsa_device_open(char *device_name,
		uint32_t  rate, int32_t channels, int32_t period) {
	int32_t dir;
	int32_t err;
	snd_pcm_hw_params_t *hw_params;
	snd_pcm_sw_params_t *sw_params;
	snd_pcm_uframes_t period_size = period;
	snd_pcm_uframes_t buffer_size = 2 * period;
	static snd_output_t *jcd_out;
	m_dev = (YangAlsaDevice*) malloc(
			(unsigned long) sizeof(YangAlsaDevice));
	if (!m_dev)
		return ERROR_SYS_NoAudioDevice;
	m_dev->device_name = (char*) malloc(1 + strlen(device_name));
	if (!m_dev->device_name) {
		free(m_dev);
		return ERROR_SYS_NoAudioDevice;
	}
	strcpy(m_dev->device_name, device_name);
	m_dev->channels = channels;
	m_dev->period = period;
	err = snd_output_stdio_attach(&jcd_out, stdout, 0);

	if ((err = snd_pcm_open(&m_dev->capture_handle, m_dev->device_name,
			SND_PCM_STREAM_CAPTURE, 0)) < 0) {

		yang_error("cannot open audio device %s (%s)", m_dev->device_name,
				snd_strerror(err));
		catpureDeviceState = 0;
		//_exit(1);
	}
	if (catpureDeviceState) {
		if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0) {

			yang_error("cannot allocate hardware parameter structure (%s)",
					snd_strerror(err));
			_exit(1);
		}

		if ((err = snd_pcm_hw_params_any(m_dev->capture_handle, hw_params))
				< 0) {

			yang_error("cannot initialize hardware parameter structure (%s)",
					snd_strerror(err));
			_exit(1);
		}

		if ((err = snd_pcm_hw_params_set_access(m_dev->capture_handle,
				hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {

			yang_error("cannot set access type (%s)", snd_strerror(err));
			_exit(1);
		}

		if ((err = snd_pcm_hw_params_set_format(m_dev->capture_handle,
				hw_params, SND_PCM_FORMAT_S16_LE)) < 0) {

			yang_error("cannot set sample format (%s)", snd_strerror(err));
			_exit(1);
		}

		if ((err = snd_pcm_hw_params_set_rate_near(m_dev->capture_handle,
				hw_params, &rate, 0)) < 0) {

			yang_error("cannot set sample rate (%s)", snd_strerror(err));
			_exit(1);
		}

		if ((err = snd_pcm_hw_params_set_channels(m_dev->capture_handle,
				hw_params, channels)) < 0) {

			yang_error("cannot set channel count (%s)", snd_strerror(err));
			_exit(1);
		}

		period_size = period;
		dir = 0;
		if ((err = snd_pcm_hw_params_set_period_size_near(m_dev->capture_handle,
				hw_params, &period_size, &dir)) < 0) {

			yang_error("cannot set period size (%s)", snd_strerror(err));
			_exit(1);
		}


		buffer_size = period_size * 2;
		dir = 0;
		if ((err = snd_pcm_hw_params_set_buffer_size_near(m_dev->capture_handle,
				hw_params, &buffer_size)) < 0) {

			yang_error("cannot set buffer time (%s)", snd_strerror(err));
			_exit(1);
		}

		if ((err = snd_pcm_hw_params(m_dev->capture_handle, hw_params)) < 0) {

			yang_error("cannot set capture parameters (%s)", snd_strerror(err));
			_exit(1);
		}

		snd_pcm_hw_params_free(hw_params);

		if ((err = snd_pcm_sw_params_malloc(&sw_params)) < 0) {

			yang_error("cannot allocate software parameters structure (%s)",
					snd_strerror(err));
			_exit(1);
		}
		if ((err = snd_pcm_sw_params_current(m_dev->capture_handle, sw_params))
				< 0) {

			yang_error("cannot initialize software parameters structure (%s)",
					snd_strerror(err));
			_exit(1);
		}
		if ((err = snd_pcm_sw_params_set_avail_min(m_dev->capture_handle,
				sw_params, period)) < 0) {

			yang_error("cannot set minimum available count (%s)",
					snd_strerror(err));
			_exit(1);
		}
		if ((err = snd_pcm_sw_params(m_dev->capture_handle, sw_params)) < 0) {

			yang_error("cannot set software parameters (%s)",
					snd_strerror(err));
			_exit(1);
		}
	}



	if ((err = snd_pcm_open(&m_dev->playback_handle, m_dev->device_name,
			SND_PCM_STREAM_PLAYBACK, 0)) < 0) {

		yang_error("cannot open audio device %s (%s)", m_dev->device_name,	snd_strerror(err));
		playDeviceState=0;
	}
	if(playDeviceState){
	if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0) {

		yang_error("cannot allocate hardware parameter structure (%s)",
				snd_strerror(err));
		_exit(1);
	}

	if ((err = snd_pcm_hw_params_any(m_dev->playback_handle, hw_params)) < 0) {

		yang_error("cannot initialize hardware parameter structure (%s)",
				snd_strerror(err));
		_exit(1);
	}

	if ((err = snd_pcm_hw_params_set_access(m_dev->playback_handle, hw_params,
			SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {

		yang_error("cannot set access type (%s)", snd_strerror(err));
		_exit(1);
	}

	if ((err = snd_pcm_hw_params_set_format(m_dev->playback_handle, hw_params,
			SND_PCM_FORMAT_S16_LE)) < 0) {

		yang_error("cannot set sample format (%s)", snd_strerror(err));
		_exit(1);
	}

	if ((err = snd_pcm_hw_params_set_rate_near(m_dev->playback_handle,
			hw_params, &rate, 0)) < 0) {

		yang_error("cannot set sample rate (%s)", snd_strerror(err));
		_exit(1);
	}
	/*	yang_error( "rate = %d", rate);*/

	if ((err = snd_pcm_hw_params_set_channels(m_dev->playback_handle, hw_params,
			channels)) < 0) {

		yang_error("cannot set channel count (%s)", snd_strerror(err));
		_exit(1);
	}

	period_size = period;
	dir = 0;
	if ((err = snd_pcm_hw_params_set_period_size_near(m_dev->playback_handle,
			hw_params, &period_size, &dir)) < 0) {

		yang_error("cannot set period size (%s)", snd_strerror(err));
		_exit(1);
	}

	buffer_size = period_size * 2;
	dir = 0;
	if ((err = snd_pcm_hw_params_set_buffer_size_near(m_dev->playback_handle,
			hw_params, &buffer_size)) < 0) {

		yang_error("cannot set buffer time (%s)", snd_strerror(err));
		_exit(1);
	}

	if ((err = snd_pcm_hw_params(m_dev->playback_handle, hw_params)) < 0) {

		yang_error("cannot set playback parameters (%s)", snd_strerror(err));
		_exit(1);
	}


	snd_pcm_hw_params_free(hw_params);

	if ((err = snd_pcm_sw_params_malloc(&sw_params)) < 0) {

		yang_error("cannot allocate software parameters structure (%s)",
				snd_strerror(err));
		_exit(1);
	}
	if ((err = snd_pcm_sw_params_current(m_dev->playback_handle, sw_params))
			< 0) {

		yang_error("cannot initialize software parameters structure (%s)",
				snd_strerror(err));
		_exit(1);
	}
	if ((err = snd_pcm_sw_params_set_avail_min(m_dev->playback_handle,
			sw_params, period)) < 0) {

		yang_error("cannot set minimum available count (%s)",
				snd_strerror(err));
		_exit(1);
	}
	if ((err = snd_pcm_sw_params_set_start_threshold(m_dev->playback_handle,
			sw_params, period)) < 0) {

		yang_error("cannot set start mode (%s)", snd_strerror(err));
		_exit(1);
	}
	if ((err = snd_pcm_sw_params(m_dev->playback_handle, sw_params)) < 0) {

		yang_error("cannot set software parameters (%s)", snd_strerror(err));
		_exit(1);
	}

	snd_pcm_link(m_dev->capture_handle, m_dev->playback_handle);
	if ((err = snd_pcm_prepare(m_dev->capture_handle)) < 0) {

		yang_error("cannot prepare audio interface for use (%s)",
				snd_strerror(err));
		_exit(1);
	}
	if ((err = snd_pcm_prepare(m_dev->playback_handle)) < 0) {

		yang_error("cannot prepare audio interface for use (%s)",
				snd_strerror(err));
		_exit(1);
	}
	}

	if(catpureDeviceState){
		m_dev->readN = snd_pcm_poll_descriptors_count(m_dev->capture_handle);
		m_dev->read_fd = (pollfd*) malloc(m_dev->readN * sizeof(*m_dev->read_fd));
		if (snd_pcm_poll_descriptors(m_dev->capture_handle, m_dev->read_fd,
				m_dev->readN) != m_dev->readN) {

			yang_error("cannot obtain capture file descriptors (%s)",
					snd_strerror(err));
			_exit(1);
		}
	}
	if(playDeviceState){
		m_dev->writeN = snd_pcm_poll_descriptors_count(m_dev->playback_handle);
		m_dev->write_fd = (pollfd*) malloc(m_dev->writeN * sizeof(*m_dev->read_fd));
		if (snd_pcm_poll_descriptors(m_dev->playback_handle, m_dev->write_fd,
				m_dev->writeN) != m_dev->writeN) {

			yang_error("cannot obtain playback file descriptors (%s)",
					snd_strerror(err));
			_exit(1);
		}
	}


	if(!catpureDeviceState&&!playDeviceState){
		return ERROR_SYS_NoAudioDevice;
	}else if(!catpureDeviceState){
		return ERROR_SYS_NoAudioCaptureDevice;
	}else if(!playDeviceState){
		return ERROR_SYS_NoAudioPlayDevice;
	}


	return Yang_Ok;
}

void YangAudioAecLinux::alsa_device_close() {
	if (m_dev) {
		snd_pcm_close(m_dev->capture_handle);
		snd_pcm_close(m_dev->playback_handle);
		free(m_dev->device_name);
		free(m_dev);
		m_dev = NULL;
	}
}

int32_t YangAudioAecLinux::alsa_device_read(short *pcm, int32_t len) {

	if ((m_ret = snd_pcm_readi(m_dev->capture_handle, pcm, len)) != len) {
		if (m_ret < 0) {
			if (m_ret == -EPIPE) {

				yang_error("An overrun has occured, reseting capture");
			} else {

				yang_error("read from audio interface failed (%s)",
						snd_strerror(m_ret));

			}
			if ((m_ret = snd_pcm_prepare(m_dev->capture_handle)) < 0) {

				yang_error("cannot prepare audio interface for use (%s)",
						snd_strerror(m_ret));
			}
			if ((m_ret = snd_pcm_start(m_dev->capture_handle)) < 0) {

				yang_error("cannot prepare audio interface for use (%s)",
						snd_strerror(m_ret));
			}

		} else {

			yang_error(
					"Couldn't read as many samples as I wanted (%d instead of %d)",
					m_ret, len);
		}
		return 1;
	}
	return Yang_Ok;
}

int32_t YangAudioAecLinux::alsa_device_write(const short *pcm, int32_t len) {

	if ((m_ret = snd_pcm_writei(m_dev->playback_handle, pcm, len)) != len) {
		if (m_ret < 0) {
			if (m_ret == -EPIPE) {
				//	yang_usleep(1000);
				yang_error("An underrun has occured, reseting playback, len=%d",len);
			} else {
				yang_error("write to audio interface failed (%s)",
						snd_strerror(m_ret));
			}
			if ((m_ret = snd_pcm_prepare(m_dev->playback_handle)) < 0) {
				yang_error("cannot prepare audio interface for use (%s)",
						snd_strerror(m_ret));
			}
		} else {
			yang_error(
					"Couldn't write as many samples as I wanted (%d instead of %d)",
					m_ret, len);
		}

		return 1;
	}
	return Yang_Ok;
}

int32_t YangAudioAecLinux::alsa_device_capture_ready(struct pollfd *pfds,
		uint32_t  nfds) {
	unsigned short revents = 0;

	if ((m_ret = snd_pcm_poll_descriptors_revents(m_dev->capture_handle, pfds,
			m_dev->readN, &revents)) < 0) {

		yang_error("error in alsa_device_capture_ready: %s",
				snd_strerror(m_ret));
		return pfds[0].revents & POLLIN;
	}

	return revents & POLLIN;
}

int32_t YangAudioAecLinux::alsa_device_playback_ready(struct pollfd *pfds,
		uint32_t  nfds) {
	unsigned short revents = 0;

	if ((m_ret = snd_pcm_poll_descriptors_revents(m_dev->playback_handle,
			pfds + m_dev->readN, m_dev->writeN, &revents)) < 0) {
		yang_error("error in alsa_device_playback_ready: %s",
				snd_strerror(m_ret));
		return pfds[1].revents & POLLOUT;
	}

	return revents & POLLOUT;
}

int32_t YangAudioAecLinux::alsa_device_nfds() {
	return m_dev->writeN + m_dev->readN;
}

void YangAudioAecLinux::alsa_device_getfds(struct pollfd *pfds,
		uint32_t  nfds) {
	int32_t i;

	for (i = 0; i < m_dev->readN; i++)
		pfds[i] = m_dev->read_fd[i];
	for (i = 0; i < m_dev->writeN; i++)
		pfds[i + m_dev->readN] = m_dev->write_fd[i];
}
void YangAudioAecLinux::setInAudioBuffer(vector<YangAudioPlayBuffer*> *pal) {

}
void YangAudioAecLinux::stopLoop() {
	m_loops = yangfalse;
}

void YangAudioAecLinux::run() {
	m_isStart=yangtrue;
	startLoop();
	m_isStart=yangfalse;
}



int32_t YangAudioAecLinux::init() {
	if (m_isInit)
		return Yang_Ok;


	m_captureFrames=m_captureSample*m_captureChannel/50;
	if (m_preProcess) {
		m_preProcess->init(m_preProcess->context,m_captureFrames, m_captureSample, m_captureChannel);
	}

	int32_t ret = alsa_device_open((char*)"default", m_captureSample, m_captureChannel, m_captureFrames);

	m_size = m_captureFrames * 2 * m_captureChannel; // 2 bytes/sample, 2 channels
	m_buffer = (uint8_t*) malloc(m_size);
	m_isInit = yangtrue;
	return ret;
}

void YangAudioAecLinux::startLoop() {
	m_loops = yangtrue;
	int32_t nfds = alsa_device_nfds();

	pollfd *pfds = (pollfd*) malloc(sizeof(*pfds) * nfds);
	alsa_device_getfds(pfds, nfds);
	int32_t audiolen = m_captureFrames * m_captureChannel * 2;

	uint8_t pcm_write[audiolen];
	yang_memset(pcm_write,0,audiolen);

	uint8_t *tmp = NULL;

	yangbool readStart = yangfalse;
	YangFrame frame;
	yang_memset(&frame,0,sizeof(YangFrame));
	while (m_loops) {
		poll(pfds, nfds, -1);
		if (playDeviceState&&alsa_device_playback_ready(pfds, nfds)) {

			tmp=m_audioData.getRenderAudioData(audiolen);
			if(tmp){

				if (!readStart)			readStart = yangtrue;
			}else{
				tmp=pcm_write;
			}

			alsa_device_write( (short*)tmp, m_captureFrames);
			if (readStart)		m_ahandle->putEchoPlay((short*)tmp,audiolen);
		}
		if (catpureDeviceState&&alsa_device_capture_ready(pfds, nfds)) {
			alsa_device_read((short*) m_buffer, m_captureFrames);
			if (readStart)
				m_ahandle->putEchoBuffer(m_buffer,audiolen);
			else{
				m_ahandle->putEchoBuffer2(m_buffer,audiolen);
			}

		}
	}
	yang_free(pfds);
}
#endif
