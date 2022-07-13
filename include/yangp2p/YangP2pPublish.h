//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YangP2pPublish_H
#define YangP2pPublish_H


#include <yangp2p/YangP2pCommon.h>
#include "yangutil/buffer/YangAudioEncoderBuffer.h"
#include "yangutil/buffer/YangAudioBuffer.h"
#include "yangutil/sys/YangIni.h"
#include "yangutil/buffer/YangVideoEncoderBuffer.h"
#include "yangutil/buffer/YangVideoBuffer.h"
class YangP2pPublish: public YangSendRequestCallback {
public:
	YangP2pPublish(){};
	virtual ~YangP2pPublish(){};

	virtual void setCaptureType(int pct)=0;

	virtual void startCamera()=0;

	virtual void stopCamera()=0;

	virtual void setNetBuffer(YangP2pRtcStream *prr)=0;

	virtual void initAudioEncoding()=0;
	virtual void initVideoEncoding()=0;
	virtual void setVideoInfo(YangVideoInfo *pvideo)=0;
	virtual int32_t startAudioCapture()=0;
	virtual int32_t startVideoCapture()=0;



	virtual void startAudioEncoding()=0;
	virtual void startVideoEncoding()=0;
	virtual void deleteVideoEncoding()=0;
	virtual void startAudioCaptureState()=0;
	virtual void startVideoCaptureState()=0;

	virtual void stopAudioCaptureState()=0;
	virtual void stopVideoCaptureState()=0;

	virtual YangVideoBuffer* getPreVideoBuffer()=0;
	virtual YangVideoBuffer* getOutPreVideoBuffer()=0;
	virtual YangVideoBuffer* getOutVideoBuffer()=0;
	virtual void stopAll()=0;
	virtual void setInAudioBuffer(std::vector<YangAudioPlayBuffer*> *pbuf)=0;
	virtual void change(int32_t st)=0;

	virtual void sendRequest(int32_t puid, uint32_t ssrc, YangRequestType req)=0;
	virtual void sendMsgToEncoder(YangRequestType req)=0;
	virtual void* getPushCapture()=0;

};

#endif //
