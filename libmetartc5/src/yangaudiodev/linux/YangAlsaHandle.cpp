//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangaudiodev/linux/YangAlsaHandle.h>
#ifndef _WIN32


#include "stdlib.h"
#include <stdio.h>
#include "memory.h"
#include "malloc.h"
YangAlsaHandle::YangAlsaHandle(YangContext *pcontext) //:YangAudioCapture(pcontext)
		{

	m_context = pcontext;
    m_ahandle = new YangAudioCaptureHandle(pcontext);
	m_writeInit = 0, m_writeRet = 0;
	m_readInit = 0;
	aIndex = 0;
	m_size = 0;
	m_loops = 0;
	m_readHandle = NULL;
	m_writeHandle = NULL;
	m_in_audioBuffer = NULL;
	m_buffer = NULL;
	m_frames = 1024;
	m_channel = 2;
	m_sample = 44100;
}

YangAlsaHandle::~YangAlsaHandle() {


	yang_delete(m_buffer);
	yang_delete(m_ahandle);
}
void YangAlsaHandle::setCatureStart(){
	m_ahandle->isBuf=1;
}
void YangAlsaHandle::setCatureStop(){
	m_ahandle->isBuf=0;
}
void YangAlsaHandle::setOutAudioBuffer(YangAudioBuffer *pbuffer){
	m_ahandle->setOutAudioBuffer(pbuffer);
}
void YangAlsaHandle::setPlayAudoBuffer(YangAudioBuffer *pbuffer){
	m_ahandle->m_aecPlayBuffer=pbuffer;
}
void YangAlsaHandle::setAec(YangRtcAec *paec){
	m_ahandle->m_aec=paec;
}
void YangAlsaHandle::setPreProcess(YangPreProcess *pp) {

}

int32_t YangAlsaHandle::init() {
	if (m_context->avinfo.audio.usingMono) {
		m_frames = 320;
		m_channel = 1;
		m_sample = 16000;
	} else {
		m_frames = 1024;
	}
	initRead();
	initWrite();
	return Yang_Ok;
}

void YangAlsaHandle::setInAudioBuffer(vector<YangAudioPlayBuffer*> *pal) {
	m_in_audioBuffer = pal;
}
void YangAlsaHandle::initWrite() {
	if (m_writeInit == 1)
		return;

	uint32_t  val = 0;
	int32_t dir = 0;
	snd_pcm_hw_params_t *hw_params;
	int32_t err = 0;
	if ((err = snd_pcm_open(&m_writeHandle, "default", SND_PCM_STREAM_PLAYBACK,
			0)) < 0) {

			yang_error( "unable to open pcm device: %s\n",
					snd_strerror(err));
		_exit(1);
	}
	if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0) {
			yang_error("cannot allocate hardware parameter structure (%s)\n",
					snd_strerror(err));
		_exit(1);
	}

	if ((err = snd_pcm_hw_params_any(m_writeHandle, hw_params)) < 0) {

			yang_error(
					"cannot initialize hardware parameter structure (%s)\n",
					snd_strerror(err));
		_exit(1);
	}

	if ((err = snd_pcm_hw_params_set_access(m_writeHandle, hw_params,
			SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {

			yang_error( "cannot set access type (%s)\n",
					snd_strerror(err));
		_exit(1);
	}

	if ((err = snd_pcm_hw_params_set_format(m_writeHandle, hw_params,
			SND_PCM_FORMAT_S16_LE)) < 0) {

			yang_error( "cannot set sample format (%s)\n",
					snd_strerror(err));
		_exit(1);
	}

	val = m_sample;
	if ((err = snd_pcm_hw_params_set_rate_near(m_writeHandle, hw_params, &val,
			0)) < 0) {

			yang_error( "cannot set sample rate (%s)\n",
					snd_strerror(err));
		_exit(1);
	}

	if ((err = snd_pcm_hw_params_set_channels(m_writeHandle, hw_params,
			m_channel)) < 0) {

			yang_error( "cannot set channel count (%s)\n",
					snd_strerror(err));
		_exit(1);
	}

	if ((err = snd_pcm_hw_params_set_period_size_near(m_writeHandle, hw_params,
			&m_frames, &dir)) < 0) {

			yang_error( "cannot set period size (%s)\n",
					snd_strerror(err));
		_exit(1);
	}

	if ((err = snd_pcm_hw_params(m_writeHandle, hw_params)) < 0) {

			yang_error( "cannot set write parameters (%s)\n",
					snd_strerror(err));
		_exit(1);
	}

	snd_pcm_hw_params_free(hw_params);
	hw_params=NULL;
	m_size = m_frames * 2 * m_channel; // 2 bytes/sample, 2 channels

	m_writeInit = 1;

}
void YangAlsaHandle::initRead() {
	if (m_readInit == 1)
		return;
	int32_t dir = 0;
	snd_pcm_hw_params_t *hw_params;
	int32_t err = 0;
	if ((err = snd_pcm_open(&m_readHandle, "default", SND_PCM_STREAM_CAPTURE, 0))
			< 0) {

			yang_error( "unable to open pcm device: %s\n",
					snd_strerror(err));
		_exit(1);
	}
	if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0) {

			yang_error(
					"cannot allocate hardware parameter structure (%s)\n",
					snd_strerror(err));
		_exit(1);
	}

	if ((err = snd_pcm_hw_params_any(m_readHandle, hw_params)) < 0) {

			yang_error(
					"cannot initialize hardware parameter structure (%s)\n",
					snd_strerror(err));
		_exit(1);
	}

	if ((err = snd_pcm_hw_params_set_access(m_readHandle, hw_params,
			SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {

			yang_error( "cannot set access type (%s)\n",
					snd_strerror(err));
		_exit(1);
	}

	if ((err = snd_pcm_hw_params_set_format(m_readHandle, hw_params,
			SND_PCM_FORMAT_S16_LE)) < 0) {

			yang_error( "cannot set sample format (%s)\n",
					snd_strerror(err));
		_exit(1);
	}


	if ((err = snd_pcm_hw_params_set_rate_near(m_readHandle, hw_params,
			&m_sample, 0)) < 0) {

			yang_error( "cannot set sample rate (%s)\n",
					snd_strerror(err));
		_exit(1);
	}

	if ((err = snd_pcm_hw_params_set_channels(m_readHandle, hw_params,
			m_channel)) < 0) {

			yang_error( "cannot set channel count (%s)\n",
					snd_strerror(err));
		_exit(1);
	}


	if ((err = snd_pcm_hw_params_set_period_size_near(m_readHandle, hw_params,
			&m_frames, &dir)) < 0) {

			yang_error( "cannot set period size (%s)\n",
					snd_strerror(err));
		_exit(1);
	}

	if ((err = snd_pcm_hw_params(m_readHandle, hw_params)) < 0) {

			yang_error( "cannot set read parameters (%s)\n",
					snd_strerror(err));
		_exit(1);
	}


	snd_pcm_hw_params_free(hw_params);
	hw_params=NULL;
	m_size = m_frames * 2 * m_channel; // 2 bytes/sample, 2 channels
	m_buffer = (uint8_t*) malloc(m_size);
	m_readInit = 1;
}

void YangAlsaHandle::startLoop() {

	m_loops = 1;
	int32_t status = 0;

	int32_t audiolen = m_frames * m_channel * 2;

	if ((status = snd_pcm_prepare(m_readHandle)) < 0) {

			yang_error(
					"cannot prepare audio interface for use (%s)\n",
					snd_strerror(status));
		_exit(1);
	}
	uint8_t *pcm = new uint8_t[audiolen];
	uint8_t *tmp = NULL;
	int32_t readLen = 0;

	YangFrame frame;
	memset(&frame,0,sizeof(YangFrame));
	while (m_loops == 1) {
		readLen = 0;
		if (hasData() > 0) {

			memset(pcm, 0, audiolen);
			for (size_t i = 0; i < m_in_audioBuffer->size(); i++) {
				tmp = m_in_audioBuffer->at(i)->getAudios(&frame);
				if (tmp)
					yang_mixaudio_mix5(pcm, tmp, audiolen, 128);
			}
			status = snd_pcm_writei(m_writeHandle, pcm, m_frames);
			 m_ahandle->putEchoPlay((short*) pcm,audiolen);
			readLen = audiolen;
			if (status < 0) {
				if (status == -EPIPE) {
					// EPIPE means overrun

					yang_usleep(1000);
					snd_pcm_prepare(m_writeHandle);
					continue;
					//snd_pcm_prepare(m_handle);
				}
				status = snd_pcm_recover(m_writeHandle, status, 0);
				if (status < 0) {

						yang_error(
								"ALSA write failed (unrecoverable): %s\n",
								snd_strerror(status));
				}

			}

		}
		if ((status = snd_pcm_readi(m_readHandle, m_buffer, m_frames))
				!= m_frames) {

				yang_error(
						"read from audio interface failed (%s)\n",
						snd_strerror(status));
		}
		if (status == -EAGAIN) {
			snd_pcm_wait(m_readHandle, 2 * m_channel);
			status = 0;
		} else if (status < 0) {
			status = snd_pcm_recover(m_readHandle, status, 0);
			if (status < 0) {

					yang_error(	"ALSA read failed (unrecoverable): %s\n",
							snd_strerror(status));
			}
			continue;
		}
		if (readLen > 0)
			m_ahandle->putEchoBuffer(m_buffer,audiolen);
		else
			m_ahandle->putBuffer2(m_buffer,audiolen);


	}

	printf("\n********************AudioCapture stop.......\n");
	snd_pcm_drain(m_readHandle);
	snd_pcm_close(m_readHandle);
	snd_pcm_drain(m_writeHandle);
	snd_pcm_close(m_writeHandle);
	free(m_buffer);
	m_readHandle = NULL;
	m_writeHandle = NULL;

	m_buffer = NULL;
}

void YangAlsaHandle::stopLoop() {
	m_loops = 0;
}

void YangAlsaHandle::run() {
	startLoop();
}

int32_t YangAlsaHandle::hasData() {
	if(!m_in_audioBuffer) return 0;
	for (int32_t i = 0; i < (int) m_in_audioBuffer->size(); i++) {
		if (m_in_audioBuffer->at(i)->size() > 0)
			return 1;
	}
	return 0;
}
#endif
