
#ifndef Yang_YangWinRecordAudioCapture_H1
#define Yang_YangWinRecordAudioCapture_H1

#ifdef _WIN32
#include "amstream.h"
#include <control.h>
#include "qedit.h"
#include "yangavutil/audio/YangAudioMix.h"
#include "yangavutil/audio/YangPreProcess.h"
#include <yangaudiodev/YangAudioCapture.h>
#include <yangaudiodev/win/YangRecAudioCaptureHandle.h>

#include "yangavutil/audio/YangAecBase.h"
class YangWinRecordAudioCapture: public YangAudioCapture {
public:
    YangWinRecordAudioCapture(YangContext *pcontext);
	~YangWinRecordAudioCapture();

public:
    YangRecAudioCaptureHandle *m_ahandle;

    int32_t init();
	void setInAudioBuffer(vector<YangAudioPlayBuffer*> *pal);
	void setPreProcess(YangPreProcess *pp);
	void setCatureStart();
	void setCatureStop();
    void setOutAudioBuffer(YangAudioBuffer *pbuffer);
	void setPlayAudoBuffer(YangAudioBuffer *pbuffer);
    void setAec(YangAecBase *paec);

protected:
	//void run();

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
	//YangAlsaDevice *m_dev;
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
//#ifdef __cplusplus
//}
//#endif
#endif
#endif
