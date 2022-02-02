//
//
#ifdef __WIN321__
#include "YangVideoSrc.h"
#include <Windows.h>
#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#define CheckPointer(p,ret) {if((p)==0) return (ret);}
    #define ValidateReadWritePtr(p,cb) \
        {ValidateReadPtr(p,cb) ValidateWritePtr(p,cb)}
//DEFINE_GUID(MEDIASUBTYPE_I420, 0x30323449, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_YangVideoSrcFilter, 0xa5532f97, 0x46a2, 0x48ad, 0xab, 0x23,
		0xfd, 0x96, 0xb7, 0xbd, 0x86, 0xa3);
YangVideoSrc::YangVideoSrc()
{
	m_clsid=CLSID_YangVideoSrcFilter;
	m_pName=new char[64];
	m_iPins=1;
	m_stream=new CSourceStream();
}
YangVideoSrc::~YangVideoSrc(){
	delete[] m_pName;
	m_pName=0;
}

HRESULT STDMETHODCALLTYPE YangVideoSrc::EnumPins(IEnumPins **ppEnum){
	CheckPointer(ppEnum,E_POINTER);
	//    ValidateReadWritePtr(ppEnum,sizeof(IEnumPins *));

	    /* Create a new ref counted enumerator */

	    *ppEnum = new CEnumPins(this, NULL);

	    return *ppEnum == NULL ? E_OUTOFMEMORY : NOERROR;
}
#define VFW_E_NOT_FOUND                  ((HRESULT)0x80040216L)
	HRESULT STDMETHODCALLTYPE YangVideoSrc::FindPin(LPCWSTR Id, IPin **ppPin){
		 CheckPointer(ppPin,E_POINTER);
	    *ppPin = m_stream;
		    m_stream->AddRef();
		    return S_OK;
		    //return VFW_E_NOT_FOUND;
	}

	HRESULT STDMETHODCALLTYPE YangVideoSrc::QueryFilterInfo(FILTER_INFO *pInfo){
		CheckPointer(pInfo,E_POINTER);
		 //   ValidateReadWritePtr(pInfo,sizeof(FILTER_INFO));

		    if (m_pName) {
		        memcpy(pInfo->achName, m_pName,strlen(m_pName));
		    } else {
		        pInfo->achName[0] = L'\0';
		    }
		    pInfo->pGraph = m_pGraph;
		    if (m_pGraph)
		        m_pGraph->AddRef();
		    return NOERROR;
	}

	HRESULT STDMETHODCALLTYPE YangVideoSrc::JoinFilterGraph(IFilterGraph *pGraph,
			LPCWSTR pName){

	    m_pGraph = pGraph;
	    if (m_pGraph) {
	        HRESULT hr = m_pGraph->QueryInterface(IID_IMediaEventSink,
	                        (void**) &m_pSink);
	        m_pSink->Release();        // we do NOT keep a reference on it.
	    } else {
	        // if graph pointer is null, then we should
	        // also release the IMediaEventSink on the same object - we don't
	        // refcount it, so just set it to null
	        m_pSink = NULL;
	    }


	    if (m_pName) {
	        delete[] m_pName;
	        m_pName = NULL;
	    }

	    if (pName) {
	        DWORD nameLen = lstrlenW(pName)+1;
	        m_pName = new WCHAR[nameLen];
	        if (m_pName) {
	            CopyMemory(m_pName, pName, nameLen*sizeof(WCHAR));
	        }
	    }


	    return NOERROR;

	}
	STDMETHODIMP
	YangVideoSrc::GetClassID(CLSID *pClsID)
	{
	    CheckPointer(pClsID,E_POINTER);
	  //  ValidateReadWritePtr(pClsID,sizeof(CLSID));
	    *pClsID = m_clsid;
	    return NOERROR;
	}

	HRESULT STDMETHODCALLTYPE YangVideoSrc::QueryVendorInfo(LPWSTR *pVendorInfo){
		 UNREFERENCED_PARAMETER(pVendorInfo);
		    return E_NOTIMPL;
	}

	/*   This is called after a media type has been proposed

	     Try to complete the connection by agreeing the allocator
	*/
	HRESULT
	CSourceStream::CompleteConnect(IPin *pReceivePin)
	{
	    UNREFERENCED_PARAMETER(pReceivePin);
	    return DecideAllocator(m_pInputPin, &m_pAllocator);
	}

	HRESULT
	CSourceStream::CheckConnect(IPin * pPin)
	{
	    /* Check that pin directions DONT match */

	    PIN_DIRECTION pd;
	    pPin->QueryDirection(&pd);

	 //   ASSERT((pd == PINDIR_OUTPUT) || (pd == PINDIR_INPUT));
	 //   ASSERT((m_dir == PINDIR_OUTPUT) || (m_dir == PINDIR_INPUT));

	    // we should allow for non-input and non-output connections?
	    if (pd == m_dir) {
	        return VFW_E_INVALID_DIRECTION;
	    }
	    return NOERROR;
	}
	/* This method is called when the output pin is about to try and connect to
	   an input pin. It is at this point32_t that you should try and grab any extra
	   interfaces that you need, in this case IMemInputPin. Because this is
	   only called if we are not currently connected we do NOT need to call
	   BreakConnect. This also makes it easier to derive classes from us as
	   BreakConnect is only called when we actually have to break a connection
	   (or a partly made connection) and not when we are checking a connection */

	/* Overriden from CBasePin */

	HRESULT
	CSourceStream::CheckConnect(IPin * pPin)
	{
	    HRESULT hr = CBasePin::CheckConnect(pPin);
	    if (FAILED(hr)) {
	    return hr;
	    }

	    // get an input pin and an allocator interface
	    hr = pPin->QueryInterface(IID_IMemInputPin, (void **) &m_pInputPin);
	    if (FAILED(hr)) {
	        return hr;
	    }
	    return NOERROR;
	}


	/* Overriden from CBasePin */

	HRESULT
	CSourceStream::BreakConnect()
	{
	    /* Release any allocator we hold */

	    if (m_pAllocator) {
	        // Always decommit the allocator because a downstream filter may or
	        // may not decommit the connection's allocator.  A memory leak could
	        // occur if the allocator is not decommited when a connection is broken.
	        HRESULT hr = m_pAllocator->Decommit();
	        if( FAILED( hr ) ) {
	            return hr;
	        }

	        m_pAllocator->Release();
	        m_pAllocator = NULL;
	    }

	    /* Release any input pin interface we hold */

	    if (m_pInputPin) {
	        m_pInputPin->Release();
	        m_pInputPin = NULL;
	    }
	    return NOERROR;
	}


	/* This is called when the input pin didn't give us a valid allocator */

	HRESULT
	CSourceStream::InitAllocator(IMemAllocator **ppAlloc)
	{
	    return CreateMemoryAllocator(ppAlloc);
	}


	/* Decide on an allocator, override this if you want to use your own allocator
	   Override DecideBufferSize to call SetProperties. If the input pin fails
	   the GetAllocator call then this will construct a CMemAllocator and call
	   DecideBufferSize on that, and if that fails then we are completely hosed.
	   If the you succeed the DecideBufferSize call, we will notify the input
	   pin of the selected allocator. NOTE this is called during Connect() which
	   therefore looks after grabbing and locking the object's critical section */

	// We query the input pin for its requested properties and pass this to
	// DecideBufferSize to allow it to fulfill requests that it is happy
	// with (eg most people don't care about alignment and are thus happy to
	// use the downstream pin's alignment request).

	HRESULT
	CSourceStream::DecideAllocator(IMemInputPin *pPin, IMemAllocator **ppAlloc)
	{
	    HRESULT hr = NOERROR;
	    *ppAlloc = NULL;

	    // get downstream prop request
	    // the derived class may modify this in DecideBufferSize, but
	    // we assume that he will consistently modify it the same way,
	    // so we only get it once
	    ALLOCATOR_PROPERTIES prop;
	    ZeroMemory(&prop, sizeof(prop));

	    // whatever he returns, we assume prop is either all zeros
	    // or he has filled it out.
	    pPin->GetAllocatorRequirements(&prop);

	    // if he doesn't care about alignment, then set it to 1
	    if (prop.cbAlign == 0) {
	        prop.cbAlign = 1;
	    }

	    /* Try the allocator provided by the input pin */

	    hr = pPin->GetAllocator(ppAlloc);
	    if (SUCCEEDED(hr)) {

	        hr = DecideBufferSize(*ppAlloc, &prop);
	        if (SUCCEEDED(hr)) {
	            hr = pPin->NotifyAllocator(*ppAlloc, FALSE);
	            if (SUCCEEDED(hr)) {
	                return NOERROR;
	            }
	        }
	    }

	    /* If the GetAllocator failed we may not have an interface */

	    if (*ppAlloc) {
	        (*ppAlloc)->Release();
	        *ppAlloc = NULL;
	    }

	    /* Try the output pin's allocator by the same method */

	    hr = InitAllocator(ppAlloc);
	    if (SUCCEEDED(hr)) {

	        // note - the properties passed here are in the same
	        // structure as above and may have been modified by
	        // the previous call to DecideBufferSize
	        hr = DecideBufferSize(*ppAlloc, &prop);
	        if (SUCCEEDED(hr)) {
	            hr = pPin->NotifyAllocator(*ppAlloc, FALSE);
	            if (SUCCEEDED(hr)) {
	                return NOERROR;
	            }
	        }
	    }

	    /* Likewise we may not have an interface to release */

	    if (*ppAlloc) {
	        (*ppAlloc)->Release();
	        *ppAlloc = NULL;
	    }
	    return hr;
	}


	/* This returns an empty sample buffer from the allocator WARNING the same
	   dangers and restrictions apply here as described below for Deliver() */

	HRESULT
	CSourceStream::GetDeliveryBuffer(IMediaSample ** ppSample,
	                                  REFERENCE_TIME * pStartTime,
	                                  REFERENCE_TIME * pEndTime,
	                                  DWORD dwFlags)
	{
	    if (m_pAllocator != NULL) {
	        return m_pAllocator->GetBuffer(ppSample,pStartTime,pEndTime,dwFlags);
	    } else {
	        return E_NOINTERFACE;
	    }
	}




	HRESULT
	CSourceStream::Deliver(IMediaSample * pSample)
	{
	    if (m_pInputPin == NULL) {
	        return VFW_E_NOT_CONNECTED;
	    }


	    return m_pInputPin->Receive(pSample);
	}


	// called from elsewhere in our filter to pass EOS downstream to
	// our connected input pin
	HRESULT
	CSourceStream::DeliverEndOfStream(void)
	{
	    // remember this is on IPin not IMemInputPin
	    if (m_Connected == NULL) {
	        return VFW_E_NOT_CONNECTED;
	    }
	    return m_Connected->EndOfStream();
	}


	/* Commit the allocator's memory, this is called through IMediaFilter
	   which is responsible for locking the object before calling us */

	HRESULT
	CSourceStream::Active(void)
	{
	    if (m_pAllocator == NULL) {
	        return VFW_E_NO_ALLOCATOR;
	    }
	    return m_pAllocator->Commit();
	}


	/* Free up or unprepare allocator's memory, this is called through
	   IMediaFilter which is responsible for locking the object first */

	HRESULT
	CSourceStream::Inactive(void)
	{
	    m_bRunTimeError = FALSE;
	    if (m_pAllocator == NULL) {
	        return VFW_E_NO_ALLOCATOR;
	    }
	    return m_pAllocator->Decommit();
	}

	// we have a default handling of EndOfStream which is to return
	// an error, since this should be called on input pins only
	STDMETHODIMP
	CSourceStream::EndOfStream(void)
	{
	    return E_UNEXPECTED;
	}


	// BeginFlush should be called on input pins only
	STDMETHODIMP
	CSourceStream::BeginFlush(void)
	{
	    return E_UNEXPECTED;
	}

	// EndFlush should be called on input pins only
	STDMETHODIMP
	CSourceStream::EndFlush(void)
	{
	    return E_UNEXPECTED;
	}

	// call BeginFlush on the connected input pin
	HRESULT
	CSourceStream::DeliverBeginFlush(void)
	{
	    // remember this is on IPin not IMemInputPin
	    if (m_Connected == NULL) {
	        return VFW_E_NOT_CONNECTED;
	    }
	    return m_Connected->BeginFlush();
	}

	// call EndFlush on the connected input pin
	HRESULT
	CSourceStream::DeliverEndFlush(void)
	{
	    // remember this is on IPin not IMemInputPin
	    if (m_Connected == NULL) {
	        return VFW_E_NOT_CONNECTED;
	    }
	    return m_Connected->EndFlush();
	}
	// deliver NewSegment to connected pin
	HRESULT
	CSourceStream::DeliverNewSegment(
	    REFERENCE_TIME tStart,
	    REFERENCE_TIME tStop,
	    double dRate)
	{
	    if (m_Connected == NULL) {
	        return VFW_E_NOT_CONNECTED;
	    }
	    return m_Connected->NewSegment(tStart, tStop, dRate);
	}

#endif
