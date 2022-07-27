//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangaudiodev/win/YangWinAudioApiAec.h>
#include <yangutil/yangtype.h>
#include <yangutil/sys/YangLog.h>
#ifdef _MSC_VER
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <audioclient.h>
#include <dmo.h>
#include <avrt.h>
#include <audiopolicy.h>
#include <wmcodecdsp.h>
#include <windows.h>
#include <uuids.h>
#include <iostream>
#include <Functiondiscoverykeys_devpkey.h>
#include <propvarutil.h>
#include <functiondiscoverykeys.h>
#pragma comment (lib,"wmcodecdspuuid.lib")
#pragma comment (lib,"dmoguids.lib")
#pragma comment (lib,"msdmo.lib")
#define Yang_Release(x) if(x){x->Release();x=NULL;}

class MediaBufferImpl final : public IMediaBuffer {
public:
    explicit MediaBufferImpl(BYTE* p,DWORD maxLength)
        : _data(p),
          _length(0),
          _maxLength(maxLength),
          _refCount(0) {


    }
    void init(BYTE* p){
        _data=p;
        _length=0;
    }
    // IMediaBuffer methods.
    STDMETHOD(GetBufferAndLength(BYTE** ppBuffer, DWORD* pcbLength)) {
      //  if (!ppBuffer || !pcbLength) {
           // return E_POINTER;
       // }

       if(ppBuffer) *ppBuffer = _data;
       if(pcbLength) *pcbLength = _length;

        return S_OK;
    }

    STDMETHOD(GetMaxLength(DWORD* pcbMaxLength)) {
        if (!pcbMaxLength) {
            return E_POINTER;
        }

        *pcbMaxLength = _maxLength;
        return S_OK;
    }

    STDMETHOD(SetLength(DWORD cbLength)) {
        if (cbLength > _maxLength) {
            return E_INVALIDARG;
        }

        _length = cbLength;
        return S_OK;
    }

    // IUnknown methods.
    STDMETHOD_(ULONG, AddRef()) { return InterlockedIncrement(&_refCount); }

    STDMETHOD(QueryInterface(REFIID riid, void** ppv)) {
        if (!ppv) {
            return E_POINTER;
        } else if (riid != IID_IMediaBuffer && riid != IID_IUnknown) {
            return E_NOINTERFACE;
        }

        *ppv = static_cast<IMediaBuffer*>(this);
        AddRef();
        return S_OK;
    }

    STDMETHOD_(ULONG, Release()) {
        LONG refCount = InterlockedDecrement(&_refCount);
        if (refCount == 0) {
            delete this;
        }

        return refCount;
    }

private:
    ~MediaBufferImpl() {_data=NULL; }

    BYTE* _data;
    DWORD _length;
    const DWORD _maxLength;
    LONG _refCount;
};
YangWinAudioApiAec::YangWinAudioApiAec() {
		m_isStart=0;
		m_loops=0;

	    m_dmo=NULL;

	    m_micIndex=0;
	    m_ps=NULL;
	    m_dataBuf=new BYTE[640];
	    CoCreateInstance(CLSID_CWMAudioAEC, NULL, CLSCTX_INPROC_SERVER,
	                                  IID_IMediaObject, reinterpret_cast<void**>(&m_dmo));
	   if(m_dmo) m_dmo->QueryInterface(IID_IPropertyStore, reinterpret_cast<void**>(&m_ps));
	   m_dwStatus=0;
	   memset(&m_audioFrame,0,sizeof(YangFrame));
	   m_audioData.initIn(16000, 1);
       m_audioData.initOut(48000, 2);
       m_shutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

YangWinAudioApiAec::~YangWinAudioApiAec() {
    if (NULL != m_shutdownEvent) {
        CloseHandle(m_shutdownEvent);
        m_shutdownEvent = NULL;
    }
	Yang_Release(m_ps);

    Yang_Release(m_dmo);
    delete[] m_dataBuf;

}
void YangWinAudioApiAec::setCaptureCallback(YangCaptureCallback* cb){
    m_audioData.m_cb=cb;

}
int YangWinAudioApiAec::initCapture() {
      getDefaultDeviceIndex(m_enum,eCapture,eConsole,&m_micIndex);
     int ret=initRecordingDMO();

        if(ret) return yang_error_wrap(ret,"int recording dmo fail..");

        return Yang_Ok;

}


void YangWinAudioApiAec::captureThread(){
	run();
}
  int YangWinAudioApiAec::startCpature(){
	  return Yang_Ok;
  }
  int YangWinAudioApiAec::stopCapture(){
	  return Yang_Ok;
  }
  void YangWinAudioApiAec::stop(){
  	stopLoop();
  }

  void YangWinAudioApiAec::stopLoop() {
      m_loops = 0;
      SetEvent(m_shutdownEvent);


  }

  void YangWinAudioApiAec::run() {
      m_isStart = 1;
      startLoop();
      m_isStart = 0;
  }




  int YangWinAudioApiAec::setVtI4Property(IPropertyStore* ptrPS,REFPROPERTYKEY key,LONG value) {
      PROPVARIANT pv;
      PropVariantInit(&pv);
      pv.vt = VT_I4;
      pv.lVal = value;
      HRESULT hr = ptrPS->SetValue(key, pv);
      PropVariantClear(&pv);
      if (FAILED(hr)) {
          return yang_error_wrap(1,"setVtI4Property error.....key=");

      }
      return 0;
  }
  int YangWinAudioApiAec::setBoolProperty(IPropertyStore* ptrPS, REFPROPERTYKEY key,VARIANT_BOOL value) {
      PROPVARIANT pv;
      PropVariantInit(&pv);
      pv.vt = VT_BOOL;
      pv.boolVal = value;
      HRESULT hr = ptrPS->SetValue(key, pv);
      PropVariantClear(&pv);
      if (FAILED(hr)) {
          return yang_error_wrap(1,"setVtI4Property error.....key=");
      }
      return 0;
  }

  int YangWinAudioApiAec::setDMOProperties() {
      HRESULT hr = S_OK;
      if(m_dmo == NULL) return 1;




      if (FAILED(hr) || m_ps == NULL) {
          return yang_error_wrap(hr,"get IID_IPropertyStore COM failed!.......");
      }
      // Set the AEC system mode.
      // SINGLE_CHANNEL_AEC - AEC processing only.
      if (setVtI4Property(m_ps, MFPKEY_WMAAECMA_SYSTEM_MODE, SINGLE_CHANNEL_AEC)) {
           yang_error("set SINGLE_CHANNEL_AEC COM failed!.......");
          return 1;
      }

      // Set the AEC source mode.
      // VARIANT_TRUE - Source mode (we poll the AEC for captured data).
      if (setBoolProperty(m_ps, MFPKEY_WMAAECMA_DMO_SOURCE_MODE, VARIANT_TRUE)) {
           yang_error("set MFPKEY_WMAAECMA_DMO_SOURCE_MODE COM failed!.......");
          return 1;
      }

      // Enable the feature mode.
      // This lets us override all the default processing settings below.
      if (setBoolProperty(m_ps, MFPKEY_WMAAECMA_FEATURE_MODE, VARIANT_TRUE) ) {
          yang_error("set MFPKEY_WMAAECMA_FEATURE_MODE COM failed!.......");
          return 1;
      }

      // Disable analog AGC (default enabled).
      if (setBoolProperty(m_ps, MFPKEY_WMAAECMA_MIC_GAIN_BOUNDER, VARIANT_FALSE) ) {
          yang_error("set MFPKEY_WMAAECMA_MIC_GAIN_BOUNDER COM failed!.......");
          return 1;
      }

      // Disable noise suppression (default enabled).
      // 0 - Disabled, 1 - Enabled
      if (setVtI4Property(m_ps, MFPKEY_WMAAECMA_FEATR_NS, 0)) {
            yang_error("set MFPKEY_WMAAECMA_FEATR_NS COM failed!.......");
          return 1;
      }

      // search for the device index.
      int inDevIndex = m_micIndex;

      int outDevIndex =0;
      this->getDefaultDeviceIndex(m_enum,eRender,eConsole,&outDevIndex);//0;

      DWORD devIndex = static_cast<uint32_t>(outDevIndex << 16) +
              static_cast<uint32_t>(0x0000ffff & inDevIndex);

      if (setVtI4Property(m_ps, MFPKEY_WMAAECMA_DEVICE_INDEXES, devIndex)) {

                  yang_error("set MFPKEY_WMAAECMA_FEATR_NS COM failed!.......");
          return 1;
      }

      return 0;
  }

  int YangWinAudioApiAec::initRecordingDMO() {


      if(m_dmo == NULL) return 1;

      if (setDMOProperties()) {
          yang_error("setDMOProperties failed");
          return 1;
      }

      DMO_MEDIA_TYPE mt = {};
      HRESULT hr = MoInitMediaType(&mt, sizeof(WAVEFORMATEX));
      if (FAILED(hr)) {
          MoFreeMediaType(&mt);
          yang_error("init MoInitMediaType failed");
          return 1;
      }

      mt.majortype = MEDIATYPE_Audio;
      mt.subtype = MEDIASUBTYPE_PCM;
      mt.formattype = FORMAT_WaveFormatEx;

      WAVEFORMATEX* ptrWav = reinterpret_cast<WAVEFORMATEX*>(mt.pbFormat);
      ptrWav->wFormatTag = WAVE_FORMAT_PCM;
      ptrWav->nChannels = 1;
      ptrWav->nSamplesPerSec = 16000;
      ptrWav->nAvgBytesPerSec = 32000;
      ptrWav->nBlockAlign = 2;
      ptrWav->wBitsPerSample = 16;
      ptrWav->cbSize = 0;

      // Set the VoE format equal to the AEC output format.


      // Set the DMO output format parameters.
      hr = m_dmo->SetOutputType(0, &mt, 0);
      MoFreeMediaType(&mt);
      if (FAILED(hr)) {
          yang_error("SetOutputType failed");
          return 1;
      }

      hr = m_dmo->AllocateStreamingResources();
      if (FAILED(hr)) {
          return 1;
      }
      return 0;
  }


  void YangWinAudioApiAec::startLoop(){
      DWORD taskIndex(0);
      HANDLE hMmTask = AvSetMmThreadCharacteristicsA("Pro Audio", &taskIndex);
      if (hMmTask) {
        if (FALSE == AvSetMmThreadPriority(hMmTask, AVRT_PRIORITY_CRITICAL)) {
          yang_warn( "failed to boost wincaecapture-thread using MMCSS");
        }
        yang_trace("wincaecapture thread is now registered with MMCSS (taskIndex=%d)",
             taskIndex );
      } else {
        yang_error( "failed to enable MMCSS on wincaecapture thread (err=%lu", GetLastError() );

      }
      m_loops = 1;
      uint8_t* tmp=new uint8_t[1024*4];
      YangAutoFreeA(uint8_t,tmp);

      MediaBufferImpl* m_mediaBuffer = new MediaBufferImpl(tmp,4096);
       DMO_OUTPUT_DATA_BUFFER OutputBufferStruct;
      OutputBufferStruct.pBuffer = m_mediaBuffer;
      OutputBufferStruct.pBuffer->AddRef();
      YangFrame audioFrame;
      memset(&audioFrame,0,sizeof(YangFrame));
      unsigned long bufLen=0;
      while(m_loops){
          DWORD waitResult = WaitForSingleObject(m_shutdownEvent, 5);
          switch (waitResult) {
            case WAIT_OBJECT_0:  // _hShutdownCaptureEvent
             break;
            case WAIT_TIMEOUT:  // timeout notification
              break;
            default:  // unexpected error
              yang_error("Unknown wait termination on capture side");
              break;
          }
          m_mediaBuffer->SetLength(0);
          bufLen=0;
          do{
                   OutputBufferStruct.dwStatus = 0;
                   HRESULT   hr = m_dmo->ProcessOutput(0, 1, &OutputBufferStruct, &m_dwStatus);

                      if (hr == S_FALSE) {
                         bufLen=0;
                      } else {
                          hr = m_mediaBuffer->GetBufferAndLength(NULL,  &bufLen);

                      }

           } while (OutputBufferStruct.dwStatus & DMO_OUTPUT_DATA_BUFFERF_INCOMPLETE);

          if(bufLen>0){
              audioFrame.payload=tmp;
              audioFrame.nb=bufLen;
              m_audioData.caputure(&audioFrame);

          }


      }
       Yang_Release(m_mediaBuffer);
  }

#endif
