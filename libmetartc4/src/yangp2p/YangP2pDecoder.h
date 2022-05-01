//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGAPP_YANGP2pDECODERAPP_H_
#define YANGAPP_YANGP2pDECODERAPP_H_
#include <yangdecoder/YangAudioDecoderHandles.h>
#include <yangdecoder/YangVideoDecoderHandles.h>

#include <vector>


#include "yangutil/sys/YangIni.h"

using namespace std;
class YangP2pDecoder {
public:
	YangP2pDecoder(YangContext* pcontext);
	virtual ~YangP2pDecoder();
	void initAudioDecoder();
	void initVideoDecoder();
	void setInVideoBuffer(YangVideoDecoderBuffer *pvel);
	void setInAudioBuffer(YangAudioEncoderBuffer *pael);
	vector<YangVideoBuffer*>* getOutVideoBuffer();
	vector<YangAudioPlayBuffer*>*  getOutAudioBuffer();
	void startAudioDecoder();
	void startVideoDecoder();

	void setRoomState(int32_t pst);

	void stopAll();
	YangVideoDecoderHandles *m_videoDec;
	YangAudioDecoderHandles *m_audioDec;
private:
	YangContext *m_context;
	vector<YangVideoBuffer*>* m_out_videoBuffer;
	vector<YangAudioPlayBuffer*>* m_out_audioBuffer;

};


#endif /* YANGAPP_YANGDECODERAPP_H_ */
