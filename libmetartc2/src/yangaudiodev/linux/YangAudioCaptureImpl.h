/*
 * YangAudioCaptureImpl.h
 *
 *  Created on: 2019年8月30日
 *      Author: yang
 */

#ifndef YANGCAPTURE_SRC_YANGAUDIOCAPTUREIMPL_H_
#define YANGCAPTURE_SRC_YANGAUDIOCAPTUREIMPL_H_
#include <yangaudiodev/YangAudioCaptureHandle.h>
#include <yangaudiodev/YangAudioCapture.h>
#include "yangavutil/audio/YangPreProcess.h"
#ifndef _WIN32
#include <alsa/asoundlib.h>
#include <vector>
using namespace std;
//#define REQ_BUF_NUM 4		//申请的缓冲区个数，最多5个，缓冲区太少可能会导致图像有间断

class YangAudioCaptureImpl: public YangAudioCapture {
public:
	YangAudioCaptureImpl(YangContext *pcontext);
	~YangAudioCaptureImpl();
public:
	YangAudioCaptureHandle *m_ahandle;
	int32_t init();
	void setPreProcess(YangPreProcess *pp);
	void setCatureStart();
	void setCatureStop();
	void setOutAudioBuffer(YangAudioBuffer *pbuffer);
	void setPlayAudoBuffer(YangAudioBuffer *pbuffer);
	void setInAudioBuffer(vector<YangAudioPlayBuffer*> *pal);
	void setAec(YangAecBase *paec);


protected:
	void startLoop();
	void stopLoop();
    //int32_t single2Double(short *pData, short* dData,int32_t nSize);

private:
	//YangContext *m_context;
	int32_t m_size;
	int32_t m_loops;
	int32_t m_channel;
	uint32_t  m_sample;
	snd_pcm_uframes_t m_frames;
	uint8_t *m_buffer;
	snd_pcm_t *m_handle;
	int32_t onlySupportSingle;

};
#endif
#endif /* YANGCAPTURE_SRC_YANGAUDIOCAPTUREIMPL_H_ */
