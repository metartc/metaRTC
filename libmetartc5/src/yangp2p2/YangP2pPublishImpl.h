//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YangP2pPublishImpl_H
#define YangP2pPublishImpl_H
#include <yangp2p2/YangP2pCapture.h>
#include <yangp2p/YangP2pPublish.h>
#include <yangpush/YangPushEncoder.h>
#include <yangp2p/YangP2pCommon.h>

#include "yangutil/buffer/YangAudioEncoderBuffer.h"
#include "yangutil/buffer/YangAudioBuffer.h"
#include "yangutil/sys/YangIni.h"
#include "yangutil/buffer/YangVideoEncoderBuffer.h"
#include "yangutil/buffer/YangVideoBuffer.h"
class YangP2pPublishImpl: public YangP2pPublish {
public:
	YangP2pPublishImpl(YangContext *pcontext);
	virtual ~YangP2pPublishImpl();


	void initCodec(bool hasAudio);
	void startCocdec();
	void startCaptureState();


	void setCaptureType(int pct);

	void startCamera();

	void stopCamera();

	void setNetBuffer(YangP2pRtcBuffer *prr);

	void initAudioEncoding();
	void initVideoEncoding();
	void setVideoInfo(YangVideoInfo *pvideo);
    int32_t startAudioCapture();
	int32_t startVideoCapture();

	void startAudioEncoding();
	void startVideoEncoding();
	void deleteVideoEncoding();
	void startAudioCaptureState();
	void startVideoCaptureState();

	void stopAudioCaptureState();
	void stopVideoCaptureState();

	void resetList();
	YangAudioEncoderBuffer * getOutAudioBuffer();
	YangVideoEncoderBuffer * getOutVideoBuffer();
	YangVideoMeta * getOutVideoMetaData();
	YangVideoBuffer* getPreVideoBuffer();
	YangVideoBuffer* getOutPreVideoBuffer();
	//YangVideoBuffer* getOutVideoBuffer();
	void stopAll();
	void setInAudioBuffer(vector<YangAudioPlayBuffer*> *pbuf);
	void change(int32_t st);

	void sendRequest(int32_t puid, uint32_t ssrc, YangRequestType req);
	void sendMsgToEncoder(YangRequestType req);
	void* getPushCapture();
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
	bool m_hasAudio;
};

#endif //
