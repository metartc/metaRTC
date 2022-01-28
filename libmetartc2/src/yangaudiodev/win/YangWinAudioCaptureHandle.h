#ifndef ___YangWinAudioCaptureHandlee__
#define ___YangWinAudioCaptureHandlee__
#ifdef _WIN32
#include "qedit.h"
#include "yangavutil/audio/YangMakeWave.h"
#include "yangutil/buffer/YangAudioBuffer.h"
#include <yangavutil/audio/YangPreProcess.h>
#include "../YangAudioCaptureHandle.h"
#define UsingRenderApi 1

#if UsingRenderApi
#include "YangWinAudioApiRender.h"
#else
#include "YangAudioPlaySdl.h"
#endif


class YangWinAudioCaptureHandle: public ISampleGrabberCB,public YangAudioCaptureHandle
{
public:
    YangWinAudioCaptureHandle(YangContext *pcontext);
    virtual ~YangWinAudioCaptureHandle(void);
    int32_t   readStart;

    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();
    STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject);
    HRESULT STDMETHODCALLTYPE SampleCB(double Time, IMediaSample *pSample);
    HRESULT STDMETHODCALLTYPE BufferCB(double Time, BYTE *pBuffer, LONG BufferLen);

public:
    YangPreProcess *m_preProcess;
    void setInAudioBuffer(vector<YangAudioPlayBuffer*> *pal);
    void setAec(YangAecBase *aec);
    void startRender();
    void stopRender();


private:
    uint8_t *m_buf;
    int32_t m_len;

    int32_t m_size;
    int m_asyn;
   // int m_asynStart;



#if UsingRenderApi
    YangWinAudioApiRender* m_player;
    int m_isStartRender;
#else
    YangAudioPlaySdl *m_player;
#endif


};
#endif
#endif
