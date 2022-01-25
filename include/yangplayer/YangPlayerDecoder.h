/*
 * YangDeocoderBase.h
 *
 *  Created on: 2019年10月11日
 *      Author: yang
 */

#ifndef YANGAPP_YANGDECODERAPP_H_
#define YANGAPP_YANGDECODERAPP_H_
#include <yangdecoder/YangAudioDecoderHandle.h>
#include <yangdecoder/YangVideoDecoderHandle.h>

#include <vector>


#include "yangutil/sys/YangIni.h"

using namespace std;
class YangPlayerDecoder {
public:
	YangPlayerDecoder(YangContext* pcontext);
	virtual ~YangPlayerDecoder();
	void initAudioDecoder();
	void initVideoDecoder();
	void setInVideoBuffer(YangVideoDecoderBuffer *pvel);
	void setInAudioBuffer(YangAudioEncoderBuffer *pael);
	YangVideoBuffer* getOutVideoBuffer();
	YangAudioPlayBuffer*  getOutAudioBuffer();
	void startAudioDecoder();
	void startVideoDecoder();
	//YangRoomI* getYangRoomI();
	void setRoomState(int32_t pst);
	//void setAddSdl(YangSdlAdd *psa);
	void stopAll();
	YangVideoDecoderHandle *m_videoDec;
	//YangAudioInfo m_audio;//={0};
private:
	YangContext *m_context;
        //YangSynBuffer m_syn;
	YangVideoBuffer* m_out_videoBuffer;
	YangAudioPlayBuffer* m_out_audioBuffer;
	YangAudioDecoderHandle *m_audioDec;
	//YangVideoInfo m_video;//={0};

	//void initList();
};


#endif /* YANGAPP_YANGDECODERAPP_H_ */
