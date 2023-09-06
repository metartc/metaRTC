//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef __YangWinVideoCaptureHandle__
#define __YangWinVideoCaptureHandle__
#include <yang_config.h>
#if Yang_OS_WIN
#include <yangutil/buffer/YangVideoBuffer.h>
#include <yangutil/yangavinfotype.h>
#include "qedit.h"
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
