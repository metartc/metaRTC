//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangaudiodev/win/YangWinAudioApiCapture.h>
#ifdef _WIN32
#include <yangutil/sys/YangLog.h>
#include <assert.h>
#include <ksmedia.h>
#include <avrt.h>

#define Yang_Release(x) if(x){x->Release();x=NULL;}

const float MAX_MICROPHONE_VOLUME = 255.0f;
const float MIN_MICROPHONE_VOLUME = 0.0f;
YangWinAudioApiCapture::YangWinAudioApiCapture(YangContext *pcontext) {
	m_inputDeviceIndex = 0;
	m_isStart = 0;
	m_loops = 0;
	m_bufferLength = 0;
	m_blockSize = 480;
	m_frameSize=4;
	m_captureCollection = NULL;
	m_deviceIn = NULL;
	m_clientIn = NULL;
	m_captureClient = NULL;
	m_captureVolume = NULL;
	m_samplesReadyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_audioData.initOut(pcontext->avinfo.audio.sample, pcontext->avinfo.audio.channel);

	m_channlList[0] = 2;  // stereo is prio 1
	m_channlList[1] = 1;  // mono is prio 2
	m_channlList[2] = 4;  // quad is prio 3

	m_blank = new uint8_t[960 * 8];
	memset(m_blank, 0, 960 * 8);
}
YangWinAudioApiCapture::~YangWinAudioApiCapture() {
	yang_stop(this);
	yang_stop_thread(this);
	if (NULL != m_samplesReadyEvent) {
		CloseHandle(m_samplesReadyEvent);
		m_samplesReadyEvent = NULL;
	}
	Yang_Release(m_captureCollection);
	Yang_Release(m_deviceIn);
	Yang_Release(m_clientIn);
	Yang_Release(m_captureClient);
	Yang_Release(m_captureVolume);
	yang_deleteA(m_blank);
}

void YangWinAudioApiCapture::setCaptureCallback(YangCaptureCallback *cb) {
	m_audioData.m_cb = cb;
}

int YangWinAudioApiCapture::setMicrophoneVolume(int volume) {

	if (volume < static_cast<int>(MIN_MICROPHONE_VOLUME)
			|| volume > static_cast<int>(MAX_MICROPHONE_VOLUME)) {
		return 1;
	}

	HRESULT hr = S_OK;
	// scale input volume to valid range (0.0 to 1.0)
	const float fLevel = static_cast<float>(volume) / MAX_MICROPHONE_VOLUME;

	//   m_lock.lock();
	m_captureVolume->SetMasterVolumeLevelScalar(fLevel, NULL);
//    m_lock.unlock();
	if (FAILED(hr))
		return 1;

	return 0;

}
int YangWinAudioApiCapture::getMicrophoneVolume(int &volume) {

	HRESULT hr = S_OK;
	float fLevel(0.0f);
	volume = 0;
	//  m_lock.lock();
	hr = m_captureVolume->GetMasterVolumeLevelScalar(&fLevel);
	//  m_lock.unlock();
	if (FAILED(hr))
		return 1;

	// scale input volume range [0.0,1.0] to valid output range
	volume = static_cast<int>(fLevel * MAX_MICROPHONE_VOLUME);

	return 0;

}

int YangWinAudioApiCapture::setMicrophoneMute(bool enable) {

	if (m_deviceIn == NULL) {
		return 1;
	}

	HRESULT hr = S_OK;
	IAudioEndpointVolume *pVolume = NULL;

	// Set the microphone system mute state.
	hr = m_deviceIn->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL,
			reinterpret_cast<void**>(&pVolume));
	if (FAILED(hr))
		return 1;

	const BOOL mute(enable);
	hr = pVolume->SetMute(mute, NULL);
	if (FAILED(hr))
		return 1;

	Yang_Release(pVolume);
	return 0;

}

int YangWinAudioApiCapture::initMicrophone() {
	//eCommunications eConsole
	int ret = getListDevice(m_enum, eCapture, m_inputDeviceIndex, &m_deviceIn);
	if (ret != 0 || (m_deviceIn == NULL)) {
		Yang_Release(m_deviceIn);
        return yang_error_wrap(ERROR_SYS_AudioCapture, "get capture devicein fail...");
	}

	Yang_Release(m_captureVolume);
	ret = m_deviceIn->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL,
			reinterpret_cast<void**>(&m_captureVolume));
	if (ret != 0 || m_captureVolume == NULL) {
		Yang_Release(m_captureVolume);
		return 1;
	}

	return 0;

}
int YangWinAudioApiCapture::initCapture() {
	getDefaultDeviceIndex(m_enum, eCapture, eConsole, &m_inputDeviceIndex);

	if (initMicrophone()) {
        return yang_error_wrap(ERROR_SYS_AudioCapture, "init microphone fail");
	}
	if (m_deviceIn == NULL) {
		yang_error("initCapture failed");
		return 1;
	}

	HRESULT hr = S_OK;
	WAVEFORMATEX *pWfxIn = NULL;
	WAVEFORMATEXTENSIBLE Wfx = WAVEFORMATEXTENSIBLE();
	WAVEFORMATEX *pWfxClosestMatch = NULL;

	// Create COM object with IAudioClient interface.
	Yang_Release(m_clientIn);
	hr = m_deviceIn->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL,
			(void**) &m_clientIn);

	if (FAILED(hr))
        return yang_error_wrap(ERROR_SYS_AudioCapture, "create capture audioclient fail");

	// processing (mixing) of shared-mode streams.
	hr = m_clientIn->GetMixFormat(&pWfxIn);
	if (SUCCEEDED(hr)) {

	}

	// Set wave format
	Wfx.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
	Wfx.Format.wBitsPerSample = 16;
	Wfx.Format.cbSize = 22;
	Wfx.dwChannelMask = 0;
	Wfx.Samples.wValidBitsPerSample = Wfx.Format.wBitsPerSample;
	Wfx.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;

	const int freqs[6] = { 48000, 44100, 16000, 96000, 32000, 8000 };
	hr = S_FALSE;

	// Iterate over frequencies and channels, in order of priority
	for (unsigned int freq = 0; freq < sizeof(freqs) / sizeof(freqs[0]);
			freq++) {
		for (unsigned int chan = 0;
				chan < sizeof(m_channlList) / sizeof(m_channlList[0]); chan++) {
			Wfx.Format.nChannels = m_channlList[chan];
			Wfx.Format.nSamplesPerSec = freqs[freq];
			Wfx.Format.nBlockAlign = Wfx.Format.nChannels
					* Wfx.Format.wBitsPerSample / 8;
			Wfx.Format.nAvgBytesPerSec = Wfx.Format.nSamplesPerSec
					* Wfx.Format.nBlockAlign;
			// If the method succeeds and the audio endpoint device supports the
			// specified stream format, it returns S_OK. If the method succeeds and
			// provides a closest match to the specified format, it returns S_FALSE.
			hr = m_clientIn->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED,
					(WAVEFORMATEX*) &Wfx, &pWfxClosestMatch);
			if (hr == S_OK) {
				break;
			} else {
				if (pWfxClosestMatch) {
					CoTaskMemFree(pWfxClosestMatch);
					pWfxClosestMatch = NULL;
				} else {
					yang_error(" is not supported. No closest match.");
				}
			}
		}
		if (hr == S_OK)
			break;
	}

	if (hr == S_OK) {
		m_frameSize= Wfx.Format.nBlockAlign;
		m_blockSize = Wfx.Format.nSamplesPerSec / 100;

		m_audioData.initIn(Wfx.Format.nSamplesPerSec, Wfx.Format.nChannels);
		yang_trace("\ncapture: sample==%d,channle==%d,nBlockAlign==%d,blockSize==%d\n",Wfx.Format.nSamplesPerSec,Wfx.Format.nChannels,m_frameSize,m_blockSize);
	}


	// Create a capturing stream.
	hr = m_clientIn->Initialize(AUDCLNT_SHAREMODE_SHARED, // share Audio Engine with other applications
			AUDCLNT_STREAMFLAGS_EVENTCALLBACK | // processing of the audio buffer by
					// the client will be event driven
					AUDCLNT_STREAMFLAGS_NOPERSIST,// volume and mute settings for an
			// audio session will not persist
			// across system restarts
			0,// required for event-driven shared mode
			0,                    // periodicity
			(WAVEFORMATEX*) &Wfx,  // selected wave format
			NULL);

	if (FAILED(hr))
        return yang_error_wrap(ERROR_SYS_AudioCapture," capture audioclient Initialize fail...");


	hr = m_clientIn->GetBufferSize(&m_bufferLength);
//	yang_trace("\ncapture bufferLength==%d..............", m_bufferLength);
	if (FAILED(hr))
        return yang_error_wrap(ERROR_SYS_AudioCapture, " capture audioclient GetBufferSize fail");
	hr = m_clientIn->SetEventHandle(m_samplesReadyEvent);
	if (FAILED(hr))
        return yang_error_wrap(ERROR_SYS_AudioCapture, " capture audioclient SetEventHandle fail");
	// Get an IAudioCaptureClient interface.
	Yang_Release(m_captureClient);
	hr = m_clientIn->GetService(__uuidof(IAudioCaptureClient),
			(void**) &m_captureClient);
	if (FAILED(hr))
        return yang_error_wrap(ERROR_SYS_AudioCapture, " captureclient Initialize fail");

	CoTaskMemFree(pWfxIn);
	CoTaskMemFree(pWfxClosestMatch);

	return 0;

}
int YangWinAudioApiCapture::startCpature() {

	if (m_clientIn) {
		HRESULT hr = m_clientIn->Start();
		if (FAILED(hr))
            return yang_error_wrap(ERROR_SYS_AudioCapture, "capure clientin start fail");
		return Yang_Ok;
	}
    return yang_error_wrap(ERROR_SYS_AudioCapture, "capure clientin is null");

}
int YangWinAudioApiCapture::stopCapture() {
	if (m_clientIn) {
		m_clientIn->Stop();
	}
	return Yang_Ok;
}
int YangWinAudioApiCapture::captureFrame(YangFrame *audioFrame) {
	audioFrame->nb = 0;
	UINT32 packetLength=0;
	BYTE *pData = 0;
	UINT32 framesAvailable = 0;
	DWORD flags = 0;
	UINT64 recTime = 0;
	UINT64 recPos = 0;
	HRESULT hr =m_captureClient->GetNextPacketSize(&packetLength);
	while(packetLength!=0){
		 hr = m_captureClient->GetBuffer(&pData, // packet which is ready to be read by used
				&framesAvailable,  // #frames in the captured packet (can be zero)
				&flags,            // support flags (check)
				&recPos,    // device position of first audio frame in data packet
				&recTime);  // value of performance counter at the time of recording
		if (SUCCEEDED(hr)) {
			if (AUDCLNT_S_BUFFER_EMPTY == hr) {
				// Buffer was empty => start waiting for a new capture notification
				// event
				return 1;
			}

			if (flags & AUDCLNT_BUFFERFLAGS_SILENT) {
				pData = NULL;
			}

				if (pData) {
					audioFrame->payload = pData;
					audioFrame->nb = framesAvailable * m_frameSize;

				} else {
					audioFrame->payload = m_blank;
					audioFrame->nb = framesAvailable * m_frameSize;
				}
				m_audioData.caputure(audioFrame);

			hr = m_captureClient->ReleaseBuffer(framesAvailable);
		}
		m_captureClient->GetNextPacketSize(&packetLength);
	}

	return 0;
}
int YangWinAudioApiCapture::getAudioOutLength(){
	return m_audioData.getOutLength();
}
void YangWinAudioApiCapture::run() {
	m_isStart = 1;
	startLoop();
	m_isStart = 0;
}
void YangWinAudioApiCapture::stop() {
	stopLoop();
}
void YangWinAudioApiCapture::stopLoop() {
	m_loops = 0;
	stopCapture();
}

void YangWinAudioApiCapture::startLoop() {
    DWORD taskIndex(0);
    HANDLE hMmTask = AvSetMmThreadCharacteristicsA("Pro Audio", &taskIndex);
  //  ERROR_INVALID_TASK_INDEX
    if (hMmTask) {
      if (FALSE == AvSetMmThreadPriority(hMmTask, AVRT_PRIORITY_CRITICAL)) {
        yang_warn( "failed to boost wincapture-thread using MMCSS");
      }
      yang_trace("wincapture thread is now registered with MMCSS (taskIndex=%d)",
           taskIndex );
    } else {
      yang_error( "failed to enable MMCSS on wincapture thread (err=%lu", GetLastError() );

    }
	m_loops = 1;
	HRESULT hr = m_clientIn->GetBufferSize(&m_bufferLength);
    if (FAILED(hr))     yang_error( "capure clientin getbufferSize fail.");
		yang_trace("\ncapture bufferLength==%d.", m_bufferLength);
	if (startCpature()) {
		yang_error("start capture fail");
		return;
	}

	HANDLE waitArray[1] = { m_samplesReadyEvent };
	YangFrame audioFrame;
	memset(&audioFrame,0,sizeof(YangFrame));
	while (m_loops == 1) {
		DWORD waitResult = WaitForMultipleObjects(1, waitArray, FALSE, 500);
		if (waitResult == (WAIT_OBJECT_0 + 0)) {
			audioFrame.payload = NULL;
			audioFrame.nb = 0;
			captureFrame(&audioFrame);
		}

	}

}
void YangWinAudioApiCapture::captureThread() {
	run();
}

#endif
