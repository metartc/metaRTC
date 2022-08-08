//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGAPP_YangP2pDecoderImpl_H_
#define YANGAPP_YangP2pDecoderImpl_H_
#include <yangdecoder/YangAudioDecoderHandles.h>
#include <yangdecoder/YangVideoDecoderHandles.h>
#include <yangp2p/YangP2pDecoder.h>

#include <vector>


#include "yangutil/sys/YangIni.h"

using namespace std;
class YangP2pDecoderImpl:public YangP2pDecoder {
public:
	YangP2pDecoderImpl(YangContext* pcontext);
	virtual ~YangP2pDecoderImpl();
	void initAudioDecoder();
	void initVideoDecoder();
	void setInVideoBuffer(YangVideoDecoderBuffer *pvel);
	void setInAudioBuffer(YangAudioEncoderBuffer *pael);
	vector<YangVideoBuffer*>* getOutVideoBuffer();
	vector<YangAudioPlayBuffer*>*  getOutAudioBuffer();
	void startAudioDecoder();
	void startVideoDecoder();

	void stopAll();
	void addVideoStream(uint8_t *ps,int32_t pind,int32_t pisAdd);
	void removeAudioStream(int32_t puid);
private:
	YangVideoDecoderHandles *m_videoDec;
	YangAudioDecoderHandles *m_audioDec;
	YangContext *m_context;
	vector<YangVideoBuffer*>* m_out_videoBuffer;
	vector<YangAudioPlayBuffer*>* m_out_audioBuffer;

};


#endif /* YANGAPP_YANGDECODERAPP_H_ */
