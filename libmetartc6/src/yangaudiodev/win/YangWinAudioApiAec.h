//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGCAPTURE_WIN_API_YANGWINAUDIOAPIAEC_H_
#define YANGCAPTURE_WIN_API_YANGWINAUDIOAPIAEC_H_
#ifdef _MSC_VER
#include <mmdeviceapi.h>
#include <dmo.h>
#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangLoadLib.h>
#include <yangaudiodev/win/YangAudioApiCapture.h>
#include <yangaudiodev/win/YangWinAudioApi.h>
#include <yangaudiodev/YangAudioCaptureData.h>

class YangWinAudioApiAec:public YangAudioApiCapture,public YangWinAudioApi {
public:
    YangWinAudioApiAec();
	virtual ~YangWinAudioApiAec();
	void setCaptureCallback(YangCaptureCallback* cb);
    int initCapture();
    int startCpature();
    int stopCapture();
    void captureThread();
    int getAudioOutLength(){return 640;}


     int initRecordingDMO();
     int setDMOProperties();
     int getRenderIndex();
     YangAudioCaptureData m_audioData;
     HANDLE m_shutdownEvent;

	void stop();
protected:
	void run();
	void startLoop();
	void stopLoop();
	 int setBoolProperty(IPropertyStore* ptrPS, REFPROPERTYKEY key,VARIANT_BOOL value);
	 int setVtI4Property(IPropertyStore* ptrPS,REFPROPERTYKEY key,LONG value);
	int m_loops;

private:
	   IMediaObject* m_dmo;


	    DWORD m_dwStatus;

	    int m_micIndex;
	    BYTE* m_dataBuf;
	   IPropertyStore* m_ps;

	   YangFrame m_audioFrame;
};
#endif
#endif /* YANGCAPTURE_WIN_API_YANGWINAUDIOAPIAEC_H_ */
