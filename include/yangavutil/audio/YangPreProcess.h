/*
 * YangAudioPreProcess.h
 *
 *  Created on: 2020年9月29日
 *      Author: yang
 */

#ifndef YANGUTIL_AUDIO_YANGPREPROCESS_H_
#define YANGUTIL_AUDIO_YANGPREPROCESS_H_
#include <stdint.h>

class YangPreProcess {
public:
	YangPreProcess();
	virtual ~YangPreProcess();
	virtual void init(int32_t pFrameSize,int32_t sampling_rate,int32_t pchannel)=0;
	virtual void state_reset()=0;
	virtual void preprocess_run(short* pcm)=0;
	int32_t m_channel;
	int32_t m_frameSize;
	int32_t m_sampleRate;
};

#endif /* YANGUTIL_AUDIO_YANGPREPROCESS_H_ */
