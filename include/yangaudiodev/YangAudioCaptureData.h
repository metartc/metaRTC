//
// Copyright (c) 2019-2026 yanggaofeng
//
#ifndef YANGCAPTURE_WIN_API_YANGCAPTUREAUDIODATA_H_
#define YANGCAPTURE_WIN_API_YANGCAPTUREAUDIODATA_H_

#include <yangaudiodev/YangCaptureCallback.h>
#include <yangutil/yangavinfo.h>
#include <yangaudio/YangAudioUtil.h>
#include <yangaudio/YangRtcAec.h>
#include <yangutil/buffer/YangAudioBuffer.h>
class YangAudioCaptureData {
public:
	YangAudioCaptureData();
	virtual ~YangAudioCaptureData();
	void initIn(int psample,int pchannel);
	void initOut(int psample,int pchannel);
	void caputure(YangFrame* audioFrame);
	int getOutLength();
	YangCaptureCallback* m_cb;

private:
	uint8_t* m_cache;
	int m_cacheLen;
	int m_size;
	int m_pos;

private:
	YangAudioResample m_res;
	YangFrame m_audioFrame;

	void captureData();
};

#endif /* YANGCAPTURE_WIN_API_YANGCAPTUREAUDIODATA_H_ */
