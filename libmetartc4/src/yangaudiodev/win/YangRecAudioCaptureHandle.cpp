//
// Copyright (c) 2019-2022 yanggaofeng
//
#include "yangutil/yang_unistd.h"
#ifdef _WIN32
#include <strmif.h>
#include "YangRecAudioCaptureHandle.h"

YangRecAudioCaptureHandle::YangRecAudioCaptureHandle(YangContext *pcontext):YangAudioCaptureHandle(pcontext)
{
	readStart=0;

    if (pcontext->avinfo.audio.usingMono) {
		m_len=320*2;
	} else {
		m_len = 1024*4;
	}
    if(pcontext->avinfo.audio.sample==48000) m_len=960*6;
	m_buf=new uint8_t[m_len];

	m_aec=NULL;

}
YangRecAudioCaptureHandle::~YangRecAudioCaptureHandle(void)
{

	if(m_buf) delete[] m_buf;
	m_buf=NULL;
}


   STDMETHODIMP_(ULONG) YangRecAudioCaptureHandle::AddRef() { return 1; }
   STDMETHODIMP_(ULONG) YangRecAudioCaptureHandle::Release() { return 2; }

    STDMETHODIMP YangRecAudioCaptureHandle::QueryInterface(REFIID riid, void **ppvObject)
    {

		if (NULL == ppvObject) return E_POINTER;
        if (riid == __uuidof(IUnknown))
        {
            *ppvObject = static_cast<IUnknown*>(this);
             return S_OK;
        }
        if (riid == IID_ISampleGrabber)
        {
            *ppvObject = static_cast<ISampleGrabberCB*>(this);
             return S_OK;
        }
        return E_NOTIMPL;
    }

    STDMETHODIMP YangRecAudioCaptureHandle::SampleCB(double Time, IMediaSample *pSample)
    {

        return E_NOTIMPL;
    }

    STDMETHODIMP YangRecAudioCaptureHandle::BufferCB(double Time, BYTE *pBuffer, long BufferLen)
    {

    	memset(m_buf,0,m_len);  
        putBuffer2(pBuffer,BufferLen);


	return E_NOTIMPL;
    }


#endif
