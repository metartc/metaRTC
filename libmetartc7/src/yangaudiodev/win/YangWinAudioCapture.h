//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef Yang_YangWinAudioCapture_H1
#define Yang_YangWinAudioCapture_H1
#include <yangaudiodev/win/YangWinAudioCaptureHandle.h>
#include <yangaudiodev/YangAudioCapture.h>

#include <yangavutil/audio/YangPreProcess.h>
#include <yangavutil/audio/YangAudioMix.h>
#include <yangavutil/audio/YangRtcAec.h>

#if Yang_OS_WIN
#include <control.h>
#include <amstream.h>
#include <qedit.h>

class YangWinAudioCapture: public YangAudioCapture {
public:
    YangWinAudioCapture(YangContext *pcontext);
	~YangWinAudioCapture();

public:
	YangWinAudioCaptureHandle *m_ahandle;

    int32_t init();
	void setInAudioBuffer(vector<YangAudioPlayBuffer*> *pal);
	void setPreProcess(YangPreProcess *pp);
	void setCatureStart();
	void setCatureStop();
    void setOutAudioBuffer(YangAudioBuffer *pbuffer);
	void setPlayAudoBuffer(YangAudioBuffer *pbuffer);
    void setAec(YangRtcAec *paec);

protected:


	void startLoop();

	void stopLoop();
private:

	int32_t m_isInit;
	int32_t m_ret;
	void setPara();
	void getBaseFilter(const GUID gi,IBaseFilter *ib,int32_t ind);
	HRESULT GetUnconnectPin(IBaseFilter * pFilter,PIN_DIRECTION dir,IPin **ppPin);
	IPin * FindPin(IBaseFilter * pFilter,PIN_DIRECTION dir);
	IPin * FindPin1(IBaseFilter * pFilter,PIN_DIRECTION dir);
	HRESULT EnumerateDevices(REFGUID category, IEnumMoniker **ppEnum);

private:

	int32_t m_size;
	int32_t m_loops;
	int32_t m_channel;
	uint32_t  m_sample;
	int32_t m_frames;

	AM_MEDIA_TYPE *m_mt;
	IMediaControl *m_pm;
	IGraphBuilder *m_pg;
	ICaptureGraphBuilder2 *m_pb;

	IBaseFilter *m_audioSrc;
	IBaseFilter *m_nullRender;

	IMoniker   *m_moniker;
	ISampleGrabber *m_grabber;
    IBaseFilter *m_grabberF;
    IMediaEventEx *m_event;
};

#endif
#endif
