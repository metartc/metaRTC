//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YangPushPublish_H
#define YangPushPublish_H
#include <yangutil/buffer/YangAudioEncoderBuffer.h>
#include <yangutil/buffer/YangVideoEncoderBuffer.h>
#include <yangutil/buffer/YangAudioBuffer.h>
#include <yangutil/buffer/YangVideoBuffer.h>
#include <yangpush/YangPushCapture.h>
#include <yangpush/YangPushEncoder.h>
#include <yangpush/YangRtcPublish.h>

class YangPushPublish: public YangSendRequestCallback {
public:
	YangPushPublish(YangContext *pcontext);
	virtual ~YangPushPublish();

	void setCaptureType(int pct);

	void startCamera();

	void stopCamera();

	void setScreenInterval(int32_t pinterval);
	void setDrawmouse(bool isDraw);
	void setRtcNetBuffer(YangRtcPublish *prr);

	void startPubVideo();
	void startPubAudio();
	void initAudioEncoding();
	void initVideoEncoding();
	void setVideoInfo(YangVideoInfo *pvideo);
	int32_t startAudioCapture();
	int32_t startVideoCapture();


	void initVideoMeeting();
	void startAudioEncoding();
	void startVideoEncoding();
	void deleteVideoEncoding();
	void startAudioCaptureState();
	void startVideoCaptureState();

	void stopAudioCaptureState();
	void stopVideoCaptureState();
	void stopScreenCaptureState();
	YangVideoBuffer* getPreVideoBuffer();

	YangVideoBuffer* getOutPreVideoBuffer();
	YangVideoBuffer* getOutVideoBuffer();
	void stopAll();
	void setInAudioBuffer(vector<YangAudioPlayBuffer*> *pbuf);
	void change(int32_t st);

	void sendRequest(int32_t puid, uint32_t ssrc, YangRequestType req);
	void sendMsgToEncoder(YangRequestType req);
	YangPushCapture* getPushCapture();
protected:

private:
	YangVideoBuffer* m_outVideoBuffer;
	YangVideoBuffer* m_outPreVideoBuffer;
private:
	YangContext *m_context;
	YangPushEncoder *m_encoder;
	YangPushCapture *m_capture;

	YangVideoInfo *m_videoInfo;
	int32_t isStartAudioCapture, isStartVideoCapture, isStartScreenCapture;
	int32_t isStartAudioEncoder, isStartVideoEncoder;
	void stopAudioState();
	void stopVideoState();
	void initCapture();
	int m_captureType;
};

#endif //
