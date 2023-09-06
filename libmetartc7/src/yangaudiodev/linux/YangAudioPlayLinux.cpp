//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangaudiodev/linux/YangAudioPlayLinux.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/yangautofree.h>
#if Yang_OS_LINUX

#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>

YangAudioPlayLinux::YangAudioPlayLinux(YangAVInfo* avinfo,YangSynBufferManager* streams):YangAudioPlay(avinfo,streams){
	m_handle = NULL;
	m_loops = 0;
	ret = 0;
	m_frames = 0;
	m_isStart = 0;
	m_contextt = 0;
}

YangAudioPlayLinux::~YangAudioPlayLinux() {

	closeAudio();
}

int YangAudioPlayLinux::init() {
	if (m_contextt == 1)
		return Yang_Ok;

	m_frames = m_avinfo->audio.sample / 50;

	uint32_t val = 0;
	int32_t dir = 0;
	snd_pcm_hw_params_t *hw_params;
	snd_pcm_sw_params_t *sw_params;
	int32_t err = 0;
	if ((err = snd_pcm_open(&m_handle, "default", SND_PCM_STREAM_PLAYBACK, 0))
			< 0) {
		fprintf(stderr, "unable to open pcm device: %s\n", snd_strerror(err));
		exit(1);
	}
	if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0) {
		fprintf(stderr, "cannot allocate hardware parameter structure (%s)\n",
				snd_strerror(err));
		exit(1);
	}

	if ((err = snd_pcm_hw_params_any(m_handle, hw_params)) < 0) {
		fprintf(stderr, "cannot initialize hardware parameter structure (%s)\n",
				snd_strerror(err));
		exit(1);
	}

	if ((err = snd_pcm_hw_params_set_access(m_handle, hw_params,
			SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		fprintf(stderr, "cannot set access type (%s)\n", snd_strerror(err));
		exit(1);
	}

	if ((err = snd_pcm_hw_params_set_format(m_handle, hw_params,
			SND_PCM_FORMAT_S16_LE)) < 0) {
		fprintf(stderr, "cannot set sample format (%s)\n", snd_strerror(err));
		exit(1);
	}

	val = m_sample;
	if ((err = snd_pcm_hw_params_set_rate_near(m_handle, hw_params, &val, 0))
			< 0) {
		fprintf(stderr, "cannot set sample rate (%s)\n", snd_strerror(err));
		exit(1);
	}

	if ((err = snd_pcm_hw_params_set_channels(m_handle, hw_params, m_channel))
			< 0) {
		fprintf(stderr, "cannot set channel count (%s)\n", snd_strerror(err));
		exit(1);
	}


	if ((err = snd_pcm_hw_params_set_period_size_near(m_handle, hw_params,
			&m_frames, &dir)) < 0) {
		fprintf(stderr, "cannot set period size (%s)\n", snd_strerror(err));
		exit(1);
	}
	snd_pcm_uframes_t buffer_size = m_frames * 2;
	dir = 0;
	if ((err = snd_pcm_hw_params_set_buffer_size_near(m_handle,
			hw_params, &buffer_size)) < 0) {

		yang_error("cannot set buffer time (%s)", snd_strerror(err));
		_exit(1);
	}
	if ((err = snd_pcm_hw_params(m_handle, hw_params)) < 0) {
		fprintf(stderr, "cannot set parameters (%s)\n", snd_strerror(err));
		exit(1);
	}

	snd_pcm_hw_params_free(hw_params);
 // 2 bytes/sample, 2 channels
	if ((err = snd_pcm_sw_params_malloc(&sw_params)) < 0) {

			yang_error("cannot allocate software parameters structure (%s)",
					snd_strerror(err));
			_exit(1);
		}
		if ((err = snd_pcm_sw_params_current(m_handle, sw_params))
				< 0) {

			yang_error("cannot initialize software parameters structure (%s)",
					snd_strerror(err));
			_exit(1);
		}
		if ((err = snd_pcm_sw_params_set_avail_min(m_handle,
				sw_params, m_frames)) < 0) {

			yang_error("cannot set minimum available count (%s)",
					snd_strerror(err));
			_exit(1);
		}
		if ((err = snd_pcm_sw_params_set_start_threshold(m_handle,
				sw_params, m_frames)) < 0) {

			yang_error("cannot set start mode (%s)", snd_strerror(err));
			_exit(1);
		}
		if ((err = snd_pcm_sw_params(m_handle, sw_params)) < 0) {

			yang_error("cannot set software parameters (%s)", snd_strerror(err));
			_exit(1);
		}
	m_audioData.initRender(m_sample,m_channel);
	m_contextt = 1;
	return Yang_Ok;

}

void YangAudioPlayLinux::closeAudio() {

	if (m_handle) {
		snd_pcm_close(m_handle);
		m_handle = NULL;
	}

}

void YangAudioPlayLinux::stopLoop() {
	m_loops = 0;
}

void YangAudioPlayLinux::startLoop() {

	m_loops = 1;
	unsigned long status = 0;
	uint8_t *pcm = new uint8_t[4096*2];
	memset(pcm,0,4096*2);
	YangAutoFreeA(uint8_t,pcm);
	uint8_t *tmp = NULL;
	if ((status = snd_pcm_prepare(m_handle)) < 0) {
		fprintf(stderr, "cannot prepare audio interface for use (%s)\n",
				snd_strerror(status));
		exit(1);
	}
	YangFrame frame;
	memset(&frame,0,sizeof(YangFrame));
	int err = 0;
	struct pollfd *ufds;
	int count = snd_pcm_poll_descriptors_count(m_handle);
	ufds = (struct pollfd *)malloc(sizeof(struct pollfd) * count);
	if ((err = snd_pcm_poll_descriptors(m_handle, ufds, count)) < 0) {
		yang_error("Unable to obtain poll descriptors for playback: %s\n",
				snd_strerror(err));
		return;
	}
	int32_t audiolen = m_frames * m_channel * 2;
	while (m_loops == 1) {
		unsigned short revents = 0;
		status=0;
		err = snd_pcm_poll_descriptors_revents(m_handle, ufds, count, &revents);
		if (err < 0) {
			yang_error("error in alsa_device_playback_ready: %s",snd_strerror(err));
			return;
		}
		if (revents & POLLOUT) {
			frame.nb=0;
			frame.payload=NULL;
			tmp =m_audioData.getRenderAudioData(audiolen);

			if (tmp){
				status = snd_pcm_writei(m_handle, tmp, m_frames);
			}else{
				status = snd_pcm_writei(m_handle, pcm, m_frames);
			}

			if (status !=m_frames) {
				if (status == -EPIPE) {
					// EPIPE means overrun
					yang_warn("underrun occurred");
					yang_usleep(1000);
					snd_pcm_prepare(m_handle);
					continue;
					//snd_pcm_prepare(m_handle);
				}
				status = snd_pcm_recover(m_handle, status, 0);
				if (status < 0) {
					yang_error("ALSA write failed (unrecoverable): %s",	snd_strerror(status));
				}

			}
		}//endif

	}

	snd_pcm_close(m_handle);
	tmp = NULL;

	if(ufds) free(ufds);
	m_handle = NULL;

}

#endif
