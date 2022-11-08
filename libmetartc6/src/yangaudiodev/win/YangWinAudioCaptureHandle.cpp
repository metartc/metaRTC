//
// Copyright (c) 2019-2022 yanggaofeng
//
#include "yangutil/yang_unistd.h"
#ifdef _WIN32
#include <strmif.h>
#include "YangWinAudioCaptureHandle.h"
#include <yangutil/sys/YangLog.h>
YangWinAudioCaptureHandle::YangWinAudioCaptureHandle(YangContext *pcontext):YangAudioCaptureHandle(pcontext)
{
    readStart=0;
#if Yang_Enable_RenderApi
    m_player = new YangWinAudioApiRender(pcontext);
    if (m_player)     m_player->init();
    m_isStartRender=0;
#else
    m_player=new YangAudioPlaySdl(pcontext);
    if(m_player) m_player->init();
#endif
    if (pcontext->avinfo.audio.enableMono) {
        m_len=320*2;
    } else {
        m_len = 1024*4;
    }
    m_buf=new uint8_t[m_len];
    m_preProcess=NULL;
    m_aec=NULL;

    m_size=m_len;

    m_asyn=1;


}
YangWinAudioCaptureHandle::~YangWinAudioCaptureHandle(void)
{
#if Yang_Enable_RenderApi
	if(m_asyn){
        if(m_player&&m_player->m_isStart){
			yang_stop(m_player);
			yang_stop_thread(m_player);
		}
	}else{
		if(m_isStartRender&&m_player) m_player->stopRender();
	}
#endif
    m_preProcess = NULL;
    if(m_player) delete m_player;
    m_player=NULL;
    if(m_buf) delete[] m_buf;
    m_buf=NULL;

}


STDMETHODIMP_(ULONG) YangWinAudioCaptureHandle::AddRef() { return 1; }
STDMETHODIMP_(ULONG) YangWinAudioCaptureHandle::Release() { return 2; }

STDMETHODIMP YangWinAudioCaptureHandle::QueryInterface(REFIID riid, void **ppvObject)
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

HRESULT STDMETHODCALLTYPE YangWinAudioCaptureHandle::SampleCB(double Time, IMediaSample *pSample)
{

    return E_NOTIMPL;
}
void YangWinAudioCaptureHandle::setInAudioBuffer(vector<YangAudioPlayBuffer*> *pal) {

}

void YangWinAudioCaptureHandle::startRender(){
#if Yang_Enable_RenderApi
    if(m_asyn&&m_player) m_player->start();
#endif
}
void YangWinAudioCaptureHandle::stopRender(){
#if Yang_Enable_RenderApi
    if(m_asyn){
       yang_stop(m_player);
       yang_stop_thread(m_player);
    }
#endif
}
void YangWinAudioCaptureHandle::setAec(YangRtcAec *aec){
	this->m_aec=aec;
	if(m_player) m_player->setAec(aec);
}
HRESULT STDMETHODCALLTYPE YangWinAudioCaptureHandle::BufferCB(double Time, BYTE *pBuffer, LONG BufferLen)
{
    if (!readStart)		{
            readStart=m_player->m_hasRenderEcho;
    }
    if (readStart)
        putEchoBuffer(pBuffer,BufferLen);
    else
        putBuffer2(pBuffer,BufferLen);


    return E_NOTIMPL;
}


#endif
