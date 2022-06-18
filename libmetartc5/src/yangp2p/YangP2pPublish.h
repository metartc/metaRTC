//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YangP2pPublish_H
#define YangP2pPublish_H
#include <yangp2p/YangP2pCapture.h>
#include <yangpush/YangPushEncoder.h>
#include <yangp2p/YangP2pRtc.h>

#include "yangutil/buffer/YangAudioEncoderBuffer.h"
#include "yangutil/buffer/YangAudioBuffer.h"
#include "yangutil/sys/YangIni.h"
#include "yangutil/buffer/YangVideoEncoderBuffer.h"
#include "yangutil/buffer/YangVideoBuffer.h"
class YangP2pPublish: public YangSendRequestCallback {
public:
	YangP2pPublish(YangContext *pcontext);
	virtual ~YangP2pPublish();

	void setCaptureType(int pct);

	void startCamera();

	void stopCamera();

	void setNetBuffer(YangP2pRtc *prr);
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

	YangVideoBuffer* getPreVideoBuffer();
	YangVideoBuffer* getOutPreVideoBuffer();
	YangVideoBuffer* getOutVideoBuffer();
	void stopAll();
	void setInAudioBuffer(vector<YangAudioPlayBuffer*> *pbuf);
	void change(int32_t st);

	void sendRequest(int32_t puid, uint32_t ssrc, YangRequestType req);
	void sendMsgToEncoder(YangRequestType req);
	YangP2pCapture* getPushCapture();
protected:

private:
	YangVideoBuffer* m_outVideoBuffer;
	YangVideoBuffer* m_outPreVideoBuffer;
private:
	YangContext *m_context;
	YangPushEncoder *m_encoder;
	YangP2pCapture *m_capture;

	YangVideoInfo *m_videoInfo;
	int32_t isStartAudioCapture, isStartVideoCapture;
	int32_t isStartAudioEncoder, isStartVideoEncoder;
	void stopAudioState();
	void stopVideoState();
	void initCapture();
	int m_captureType;
};

#endif //
