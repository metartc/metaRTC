//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangaudiodev/win/YangWinAudioCapture.h>
#if Yang_OS_WIN
#include <uuids.h>

using namespace std;
#define SAFE_RELEASE(x) { if (x) x->Release(); x = NULL; }
YangWinAudioCapture::YangWinAudioCapture(YangContext *pcontext){ // @suppress("Class members should be properly initialized")
    //m_context = pcontext;
    m_ahandle = new YangWinAudioCaptureHandle(pcontext);

	aIndex = 0;
	m_ret = 0;
	m_size = 0;
	m_loops = 0;

	m_isInit = 0;

	m_frames = 1024;
	m_channel = 2;
	m_sample = 44100;
	m_pg = NULL;
	m_pb = NULL;
	m_pm = NULL;
	m_grabber = NULL;
	m_grabberF = NULL;
	m_event = NULL;
	m_nullRender = NULL;
	m_audioSrc = NULL;
	 m_moniker = NULL;
	m_mt = NULL;
	aIndex = 1;
    if (pcontext->avinfo.audio.enableMono) {
		m_frames = 320;
		m_channel = 1;
		m_sample = 16000;
	} else {
		m_frames = 1024;
	}

}
YangWinAudioCapture::~YangWinAudioCapture() {

	if (m_loops) {
		stop();
		while (m_isStart) {
			yang_usleep(1000);
		}
	}
	//a	delete pMoniker1,pGrabber,pGrabberF,pEvent;
	if(m_ahandle){
		SAFE_RELEASE(m_grabber);
		SAFE_RELEASE(m_grabberF);

		SAFE_RELEASE(m_event);
		SAFE_RELEASE(m_pm);
		SAFE_RELEASE(m_pg);
		SAFE_RELEASE(m_pb);
		SAFE_RELEASE(m_audioSrc);

		yang_delete(m_ahandle);
	}
}
void YangWinAudioCapture::setCatureStart() {
    m_ahandle->m_enableBuf = 1;
}
void YangWinAudioCapture::setCatureStop() {
    m_ahandle->m_enableBuf = 0;
}
void YangWinAudioCapture::setOutAudioBuffer(YangAudioBuffer *pbuffer) {
	m_ahandle->setOutAudioBuffer(pbuffer);
}
void YangWinAudioCapture::setPlayAudoBuffer(YangAudioBuffer *pbuffer) {
	m_ahandle->m_aecPlayBuffer = pbuffer;
}
void YangWinAudioCapture::setAec(YangRtcAec *paec) {
	m_ahandle->setAec(paec);
}
void YangWinAudioCapture::setPreProcess(YangPreProcess *pp) {
    m_ahandle->m_preProcess = pp;
	if (pp) {
        pp->init(pp->context,m_frames, m_sample, m_channel);
	}
}

void YangWinAudioCapture::setInAudioBuffer(vector<YangAudioPlayBuffer*> *pal) {
    m_ahandle->setInAudioBuffer(pal);
}
void YangWinAudioCapture::stopLoop() {
	m_loops = 0;
	if(m_pm!=NULL) m_pm->Stop();
}

HRESULT YangWinAudioCapture::GetUnconnectPin(IBaseFilter *pFilter,
		PIN_DIRECTION dir, IPin **ppPin) {
	*ppPin = 0;
	IEnumPins *pEnum = 0;
	IPin *pPin = 0;
	HRESULT hr = pFilter->EnumPins(&pEnum);
	if (FAILED(hr)) {
		return hr;
	}
	while (pEnum->Next(1, &pPin, NULL) == S_OK) {
		PIN_DIRECTION thisPinDir;
		pPin->QueryDirection(&thisPinDir);
		if (thisPinDir == dir) {
			IPin *pTemp = 0;
			hr = pPin->ConnectedTo(&pTemp);
			if (SUCCEEDED(hr)) {
				pTemp->Release();
			} else {
				pEnum->Release();
				*ppPin = pPin;
				return S_OK;
			}
		}
		pPin->Release();
	}
	pEnum->Release();
	return E_FAIL;
}

IPin* YangWinAudioCapture::FindPin(IBaseFilter *pFilter, PIN_DIRECTION dir) {
	IEnumPins *pEnumPins;
	IPin *pOutpin;
	PIN_DIRECTION pDir;
	pFilter->EnumPins(&pEnumPins);
	while (pEnumPins->Next(1, &pOutpin, NULL) == S_OK) {
		pOutpin->QueryDirection(&pDir);

		if (pDir == dir) {
			return pOutpin;
		}
	}
	return 0;
}
void YangWinAudioCapture::setPara() {

	int32_t nBytesPerSample = 2;
	IAMStreamConfig *config = NULL;
	HRESULT hr = m_pb->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Audio,
			m_audioSrc, IID_IAMStreamConfig, (void**) &config);
	AM_MEDIA_TYPE *pmt = { 0 };
	hr = config->GetFormat(&pmt);
	if (SUCCEEDED(hr)) {

		WAVEFORMATEX *pWF = (WAVEFORMATEX*) pmt->pbFormat;
		pmt->subtype = MEDIASUBTYPE_PCM;

		pWF->nChannels = (WORD) m_channel;	//m_sample;//2;
		pWF->nSamplesPerSec = m_sample;	//44100;
		pWF->nAvgBytesPerSec = nBytesPerSample * m_channel * m_sample;//4 * 44100;
		pWF->wBitsPerSample = nBytesPerSample * 8;
		pWF->nBlockAlign = nBytesPerSample * m_channel;	//4;
		hr = config->SetFormat(pmt);
		SAFE_RELEASE(config);

		IAMBufferNegotiation *pNeg = NULL;
		hr = m_pb->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Audio,
				m_audioSrc, IID_IAMBufferNegotiation, (void**) &pNeg);
		ALLOCATOR_PROPERTIES prop = { 0 };
		prop.cbBuffer = m_frames * nBytesPerSample * m_channel;
		prop.cBuffers = 6;
		prop.cbAlign = nBytesPerSample * m_channel;
		hr = pNeg->SuggestAllocatorProperties(&prop);
		SAFE_RELEASE(pNeg);

	} else {
		yang_error("set parar is fail!");
	}

}
int32_t YangWinAudioCapture::init() {
	if (m_isInit)
        return Yang_Ok;

	CoInitialize(NULL);

	ULONG cFetched;
	ICreateDevEnum *devEnum = NULL;
	CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC,
			IID_PPV_ARGS(&devEnum));
	IEnumMoniker *classEnum = NULL;
	devEnum->CreateClassEnumerator(CLSID_AudioInputDeviceCategory,
			&classEnum, 0);
	int32_t aco = 0;
	while (classEnum->Next(1, &m_moniker, &cFetched) == S_OK) {
		aco++;
		if (aco != aIndex)
			continue;
		m_moniker->BindToObject(0, 0, IID_IBaseFilter, (void**) &m_audioSrc);
		m_moniker->Release();
		break;
	}
	classEnum->Release();
	devEnum->Release();
	if (m_audioSrc == NULL)
        return 1;
	CoCreateInstance(CLSID_CaptureGraphBuilder2, 0, CLSCTX_INPROC_SERVER,
			IID_ICaptureGraphBuilder2, (void**) &m_pb);
	CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,
			IID_IGraphBuilder, (void**) &m_pg);

	m_pb->SetFiltergraph(m_pg);
	m_pg->QueryInterface(IID_IMediaControl, (void**) &m_pm);
	m_pg->AddFilter(m_audioSrc, L"Audio");

	CLSID CLSID_SampleGrabber = { 0xC1F400A0, 0x3F08, 0x11d3, { 0x9F, 0x0B,
			0x00, 0x60, 0x08, 0x03, 0x9E, 0x37 } };
	//sample
	HRESULT hr = CoCreateInstance(CLSID_SampleGrabber, NULL,
			CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_grabberF));
	hr = m_pg->AddFilter(m_grabberF, L"Sample Grabber");
	hr = m_grabberF->QueryInterface(IID_ISampleGrabber, (void**) &m_grabber);

	hr = m_pg->QueryInterface(IID_IMediaEventEx, (void**) &m_event);

	m_nullRender = NULL;
	CLSID CLSID_NullRenderer = { 0xC1F400A4, 0x3F08, 0x11d3, { 0x9F, 0x0B, 0x00,
			0x60, 0x08, 0x03, 0x9E, 0x37 } };
	hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC,
			IID_IBaseFilter, (void**) &m_nullRender);
	setPara();

	IPin *pAudioOut = FindPin(m_audioSrc, PINDIR_OUTPUT);
	IPin *pGrabin = FindPin(m_grabberF, PINDIR_INPUT);
	IPin *pGrabout = FindPin(m_grabberF, PINDIR_OUTPUT);
	IPin *pNullIn = FindPin(m_nullRender, PINDIR_INPUT);
	hr = m_pg->Connect(pAudioOut, pGrabin);
	hr = m_pg->Connect(pGrabout, pNullIn);


	if (m_mt == NULL)	m_mt = new AM_MEDIA_TYPE();
	hr = m_grabber->GetConnectedMediaType(m_mt);
	hr = m_grabber->SetMediaType(m_mt);
	hr = m_grabber->SetBufferSamples(TRUE);
	hr = m_grabber->SetOneShot(FALSE);
	m_grabber->SetCallback(m_ahandle, 1);
	m_isInit = 1;
    return Yang_Ok;
}

void YangWinAudioCapture::startLoop() {
	m_loops = 1;
	if (m_pm != NULL) {
		m_pm->Run();
		long eventCode;
        if(m_ahandle) m_ahandle->startRender();
		m_event->WaitForCompletion(INFINITE, &eventCode);
        if(m_ahandle) m_ahandle->stopRender();
	}

}
#endif
