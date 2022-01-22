/*
 * YangPreProcessSpeex.h
 *
 *  Created on: 2020年9月29日
 *      Author: yang
 */

#ifndef YANGAUDIOPROC_YANGPREPROCESSSPEEX_H_
#define YANGAUDIOPROC_YANGPREPROCESSSPEEX_H_
#include <yangavutil/audio/YangPreProcess.h>
#include "speex/speex_preprocess.h"
#include "yangutil/sys/YangLoadLib.h"

class YangPreProcessSpeex: public YangPreProcess {
public:
	YangPreProcessSpeex();
	virtual ~YangPreProcessSpeex();
	 void init(int32_t pFrameSize,int32_t sampling_rate,int32_t pchannel);
	 void state_reset();
	 void preprocess_run(short* pcm);
	 void closePre();
private:
	 bool      m_bHasInit;
	SpeexPreprocessState* m_preprocState;
	YangLoadLib m_lib;
	void loadLib();
	void unloadLib();
	SpeexPreprocessState *(*yang_speex_preprocess_state_init)(int32_t frame_size, int32_t sampling_rate);
	int32_t (*yang_speex_preprocess_ctl)(SpeexPreprocessState *st, int32_t request, void *ptr);
	void (*yang_speex_preprocess_state_destroy)(SpeexPreprocessState *st);
	int32_t (*yang_speex_preprocess_run)(SpeexPreprocessState *st, spx_int16_t *x);
};

#endif /* YANGAUDIOPROC_YANGPREPROCESSSPEEX_H_ */
