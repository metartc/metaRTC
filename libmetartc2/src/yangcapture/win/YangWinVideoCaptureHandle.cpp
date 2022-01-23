#include "YangWinVideoCaptureHandle.h"
#ifdef _WIN32

#include "time.h"
#include "stdlib.h"
#include "stdio.h"
#include <strmif.h>
//using namespace std;

YangWinVideoCaptureHandle::YangWinVideoCaptureHandle(YangVideoInfo *pcontext):YangVideoCaptureHandle(pcontext)
{

}
YangWinVideoCaptureHandle:: ~YangWinVideoCaptureHandle(void){

}
STDMETHODIMP_(ULONG) YangWinVideoCaptureHandle::AddRef() {
	return 1;
}
STDMETHODIMP_(ULONG) YangWinVideoCaptureHandle::Release() {
	return 2;
}

STDMETHODIMP YangWinVideoCaptureHandle::QueryInterface(REFIID riid,
		void **ppvObject) {
	// printf("*********************************\n");
	if (NULL == ppvObject)
		return E_POINTER;
	if (riid == __uuidof(IUnknown)) {
		*ppvObject = static_cast<IUnknown*>(this);
		return S_OK;
	}
	if (riid == IID_ISampleGrabber) {
		*ppvObject = static_cast<ISampleGrabberCB*>(this);
		return S_OK;
	}
	return E_NOTIMPL;
}

STDMETHODIMP YangWinVideoCaptureHandle::SampleCB(double Time,
		IMediaSample *pSample) {

	return E_NOTIMPL;
}

STDMETHODIMP YangWinVideoCaptureHandle::BufferCB(double Time, BYTE *pBuffer,
		long BufferLen) {

	putBuffer(Time*1000000, pBuffer, BufferLen);
	return E_NOTIMPL;
}

#endif
