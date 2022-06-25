//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGAPP_YangP2pCapture_H_
#define YANGAPP_YangP2pCapture_H_
#include <yangutil/yangtype.h>


#include <yangutil/buffer/YangAudioBuffer.h>
#include <yangutil/buffer/YangVideoBuffer.h>

#include <yangutil/buffer/YangAudioPlayBuffer.h>
#include <yangaudiodev/YangAudioCapture.h>
class YangP2pCapture {
public:
	YangP2pCapture();
	virtual ~YangP2pCapture();
public:
	virtual void startVideoSource() = 0;
	virtual void stopVideoSource() = 0;

	virtual void startVideoCaptureState() = 0;
	virtual void stopVideoCaptureState() = 0;

	virtual int32_t initVideo() = 0;

	virtual void startVideoCapture() = 0;

	YangVideoBuffer * getOutVideoBuffer();
	YangVideoBuffer * getPreVideoBuffer();
	virtual int32_t initAudio(YangPreProcess *pp) = 0;
	virtual int32_t initAudio2(YangPreProcess *pp) = 0;
	virtual void startAudioCapture() = 0;
	virtual YangAudioBuffer* getOutAudioBuffer() = 0;
	virtual void stopAudioCaptureState() = 0;
	virtual void startAudioCaptureState() = 0;
	virtual void setAec(YangRtcAec *paec) = 0;
	virtual void setInAudioBuffer(vector<YangAudioPlayBuffer*> *pbuf) = 0;

	virtual int32_t change(int32_t st)=0;
	virtual void stopAll() = 0;
protected:
	YangVideoBuffer *m_out_videoBuffer;
	YangVideoBuffer *m_pre_videoBuffer;

	YangContext *m_context;
	YangAudioBuffer *m_out_audioBuffer;





};

#endif /* YANGAPP_YANGCAPTUREAPP_H_ */
