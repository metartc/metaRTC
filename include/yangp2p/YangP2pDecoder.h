//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGAPP_YANGP2pDECODERAPP_H_
#define YANGAPP_YANGP2pDECODERAPP_H_
#include <yangutil/buffer/YangVideoBuffer.h>
#include <yangutil/buffer/YangAudioPlayBuffer.h>
#include <yangutil/buffer/YangVideoDecoderBuffer.h>
#include <yangutil/buffer/YangAudioEncoderBuffer.h>
#include <vector>
using namespace std;
class YangP2pDecoder {
public:
	YangP2pDecoder(){};
	virtual ~YangP2pDecoder(){};
	virtual void initAudioDecoder()=0;
	virtual void initVideoDecoder()=0;
	virtual void setInVideoBuffer(YangVideoDecoderBuffer *pvel)=0;
	virtual void setInAudioBuffer(YangAudioEncoderBuffer *pael)=0;
	virtual vector<YangVideoBuffer*>* getOutVideoBuffer()=0;
	virtual vector<YangAudioPlayBuffer*>*  getOutAudioBuffer()=0;
	virtual void startAudioDecoder()=0;
	virtual void startVideoDecoder()=0;

	virtual void stopAll()=0;
	virtual void addVideoStream(uint8_t *ps,int32_t pind,int32_t pisAdd)=0;
	virtual void removeAudioStream(int32_t puid)=0;


};


#endif /* YANGAPP_YANGDECODERAPP_H_ */
