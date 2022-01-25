#ifndef __YangWinVideoCaptureHandle__
#define __YangWinVideoCaptureHandle__
#ifdef _WIN32
#include "qedit.h"
//#include <dshow.h>
#include "yangutil/buffer/YangVideoBuffer.h"
#include "yangutil/sys/YangIni.h"
#include "../YangVideoCaptureHandle.h"
class YangWinVideoCaptureHandle: public ISampleGrabberCB,
		public YangVideoCaptureHandle {
public:
    YangWinVideoCaptureHandle(YangVideoInfo *pcontext);
	virtual ~YangWinVideoCaptureHandle(void);

	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
	STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject);
	STDMETHODIMP SampleCB(double Time, IMediaSample *pSample);
	STDMETHODIMP BufferCB(double Time, BYTE *pBuffer, long BufferLen);
private:

};
#endif
#endif
