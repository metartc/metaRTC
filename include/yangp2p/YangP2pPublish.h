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

	virtual void setVideoInfo(YangVideoInfo *pvideo)=0;
	virtual void initCodec(bool hasAudio)=0;
	virtual void startCocdec()=0;
	virtual void startCaptureState()=0;


	virtual void startCamera()=0;
	virtual void stopCamera()=0;
	virtual void stopAll()=0;

	virtual void deleteVideoEncoding()=0;

	virtual void resetList()=0;
	virtual YangAudioEncoderBuffer * getOutAudioBuffer()=0;
	virtual YangVideoEncoderBuffer * getOutVideoBuffer()=0;
	virtual YangVideoMeta * getOutVideoMetaData()=0;
	virtual YangVideoBuffer* getPreVideoBuffer()=0;
	virtual YangVideoBuffer* getOutPreVideoBuffer()=0;


	virtual void sendRequest(int32_t puid, uint32_t ssrc, YangRequestType req)=0;
	virtual void sendMsgToEncoder(YangRequestType req)=0;
	virtual void* getPushCapture()=0;
};

#endif //
