//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef Yang_YangVideoSrc_H1
#define Yang_YangVideoSrc_H1
#ifdef __WIN321__
#include <windows.h>
#include "amstream.h"
#include <control.h>
#include "qedit.h"

#include <dmodshow.h>
#include <Dmoreg.h>
#include <initguid.h>

class CSourceStream;

class YangVideoSrc: public IBaseFilter {
public:

	YangVideoSrc();
	virtual ~YangVideoSrc();
//ibasefilter
	HRESULT STDMETHODCALLTYPE EnumPins(IEnumPins **ppEnum);

	HRESULT STDMETHODCALLTYPE FindPin(LPCWSTR Id, IPin **ppPin);

	HRESULT STDMETHODCALLTYPE QueryFilterInfo(FILTER_INFO *pInfo);

	HRESULT STDMETHODCALLTYPE JoinFilterGraph(IFilterGraph *pGraph,
			LPCWSTR pName);

	HRESULT STDMETHODCALLTYPE QueryVendorInfo(LPWSTR *pVendorInfo);
//
	STDMETHODIMP GetClassID(CLSID *pClsID);



protected:
	CLSID m_clsid;
	IFilterGraph *m_pGraph;
	 IMediaEventSink *m_pSink;
	 LONG            m_PinVersion;
	int32_t m_iPins;  // The number of pins on this filter. Updated by CSourceStream
	CSourceStream *m_stream;
	char *m_pName;
};

//
// CSourceStream
//
// Use this class to manage a stream of data that comes from a
// pin.
// Uses a worker thread to put data on the pin.
class CSourceStream: public IPin,public IUnknown {
public:

	CSourceStream();
	virtual ~CSourceStream(void); // virtual destructor ensures derived class destructors are called too.

protected:

	YangVideoSrc *m_pFilter;	// The parent of this stream

	// *
	// * Data Source
	// *
	// * The following three functions: FillBuffer, OnThreadCreate/Destroy, are
	// * called from within the ThreadProc. They are used in the creation of
	// * the media samples this pin will provide
	// *

	// Override this to provide the worker thread a means
	// of processing a buffer
	virtual HRESULT FillBuffer(IMediaSample *pSamp) PURE;

	// Called as the thread is created/destroyed - use to perform
	// jobs such as start/stop streaming mode
	// If OnThreadCreate returns an error the thread will exit.
	virtual HRESULT OnThreadCreate(void) {
		return NOERROR;
	}
	;
	virtual HRESULT OnThreadDestroy(void) {
		return NOERROR;
	}
	;
	virtual HRESULT OnThreadStartPlay(void) {
		return NOERROR;
	}
	;

	// *
	// * Worker Thread
	// *

	HRESULT Active(void);    // Starts up the worker thread
	HRESULT Inactive(void);  // Exits the worker thread.


	 IMemAllocator *m_pAllocator;
	    IMemInputPin *m_pInputPin;
	    IPin            *m_Connected;               // Pin we have connected to
	    PIN_DIRECTION   m_dir;
	    bool            m_bRunTimeError;            // Run time error generated
	    bool            m_bCanReconnectWhenActive;  // OK to reconnect when active
	    bool            m_bTryMyTypesFirst;         // When connecting enumerate

public:
	// thread commands
	enum Command {
		CMD_INIT, CMD_PAUSE, CMD_RUN, CMD_STOP, CMD_EXIT
	};
	HRESULT Init(void) {
		return CallWorker(CMD_INIT);
	}
	HRESULT Exit(void) {
		return CallWorker(CMD_EXIT);
	}
	HRESULT Run(void) {
		return CallWorker(CMD_RUN);
	}
	HRESULT Pause(void) {
		return CallWorker(CMD_PAUSE);
	}
	HRESULT Stop(void) {
		return CallWorker(CMD_STOP);
	}



	 HRESULT STDMETHODCALLTYPE Connect(
	        IPin *pReceivePin,
	        const AM_MEDIA_TYPE *pmt) ;

	     HRESULT STDMETHODCALLTYPE ReceiveConnection(
	        IPin *pConnector,
	        const AM_MEDIA_TYPE *pmt) ;

	     HRESULT STDMETHODCALLTYPE Disconnect(
	        ) ;

	     HRESULT STDMETHODCALLTYPE ConnectedTo(
	        IPin **pPin) ;

	     HRESULT STDMETHODCALLTYPE ConnectionMediaType(
	        AM_MEDIA_TYPE *pmt) ;

	     HRESULT STDMETHODCALLTYPE Querypcontextnfo(
	        PIN_INFO *pInfo);

	     HRESULT STDMETHODCALLTYPE QueryDirection(
	        PIN_DIRECTION *pPinDir) ;

	     HRESULT STDMETHODCALLTYPE QueryId(
	        LPWSTR *Id) ;

	     HRESULT STDMETHODCALLTYPE QueryAccept(
	        const AM_MEDIA_TYPE *pmt) ;

	     HRESULT STDMETHODCALLTYPE EnumMediaTypes(
	        IEnumMediaTypes **ppEnum) ;

	     HRESULT STDMETHODCALLTYPE QueryInternalConnections(
	        IPin **apPin,
	        ULONG *nPin) ;

	     HRESULT STDMETHODCALLTYPE EndOfStream(
	        ) ;

	     HRESULT STDMETHODCALLTYPE BeginFlush(
	        );

	     HRESULT STDMETHODCALLTYPE EndFlush(
	        ) ;

	     HRESULT STDMETHODCALLTYPE NewSegment(
	        REFERENCE_TIME tStart,
	        REFERENCE_TIME tStop,
	        double dRate);

	      HRESULT InitAllocator(IMemAllocator **ppAlloc);
	     HRESULT CheckConnect(IPin *pPin);
	     HRESULT BreakConnect();

	     HRESULT CompleteConnect(IPin *pReceivePin);
	 	HRESULT DeliverEndOfStream(void);


protected:
	Command GetRequest(void) {
		return (Command) CAMThread::GetRequest();
	}
	BOOL CheckRequest(Command *pCom) {
		return CAMThread::CheckRequest((DWORD*) pCom);
	}

	// override these if you want to add thread commands
	virtual DWORD ThreadProc(void);  		// the thread function

	virtual HRESULT DoBufferProcessingLoop(void); // the loop executed whilst running

	// *
	// * AM_MEDIA_TYPE support
	// *

	// If you support more than one media type then override these 2 functions
	virtual HRESULT CheckMediaType(const CMediaType *pMediaType);
	virtual HRESULT GetMediaType(int32_t iPosition, CMediaType *pMediaType); // List pos. 0-n

	// If you support only one type then override this fn.
	// This will only be called by the default implementations
	// of CheckMediaType and GetMediaType(int, CMediaType*)
	// You must override this fn. or the above 2!
	virtual HRESULT GetMediaType(CMediaType *pMediaType) {
		return E_UNEXPECTED;
	}

	STDMETHODIMP QueryId(LPWSTR *Id);
};
#endif
#endif
