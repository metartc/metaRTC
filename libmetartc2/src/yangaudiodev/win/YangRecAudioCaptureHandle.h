#ifndef ___YangRecAudioCaptureHandlee__
#define ___YangRecAudioCaptureHandlee__
#ifdef _WIN32
#include "qedit.h"

#include "yangavutil/audio/YangMakeWave.h"
#include "yangutil/buffer/YangAudioBuffer.h"
#include "../YangAudioCaptureHandle.h"


class YangRecAudioCaptureHandle: public ISampleGrabberCB,public YangAudioCaptureHandle
{
public:
    YangRecAudioCaptureHandle(YangContext *pcontext);
	virtual ~YangRecAudioCaptureHandle(void);
	int32_t   readStart;

	STDMETHODIMP_(ULONG) AddRef();
	    STDMETHODIMP_(ULONG) Release();
	    STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject);
		STDMETHODIMP SampleCB(double Time, IMediaSample *pSample);
	    STDMETHODIMP BufferCB(double Time, BYTE *pBuffer, long BufferLen);
private:
	    uint8_t *m_buf;
	    int32_t m_len;

};
#endif
#endif
