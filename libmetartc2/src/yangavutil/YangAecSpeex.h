/*
 * YangAceSpeex.h
 *
 *  Created on: 2020年9月8日
 *      Author: yang
 */

#ifndef YANGAUDIOPROC_YANGAECSPEEX_H_
#define YANGAUDIOPROC_YANGAECSPEEX_H_
#include <yangavutil/audio/YangAecBase.h>

#include "speex/speex_echo.h"
#include "speex/speex_preprocess.h"
#include "yangutil/sys/YangLoadLib.h"


class YangAecSpeex: public YangAecBase {
public:
	YangAecSpeex();
	virtual ~YangAecSpeex();
	void init(int32_t pFrameSizeInMS,int32_t sampling_rate,int32_t pchannel);
	void echo_state_reset();
	void echo_playback(short *play);
	void put_echo_playback(uint8_t *play,int32_t plen);
	void echo_capture(short *rec, short *out);
	void preprocess_run(short* pcm);
	void setPlayBuffer(YangAudioBuffer *pab);
	 void echo_cancellation(const short *rec, const short *play, short *out);
	void closeAec();
	void resetAec();
private:
	bool      m_bHasInit;
		SpeexEchoState*   m_state;
		SpeexPreprocessState* m_preprocState;
		YangAudioBuffer *m_audioBuffer;
		YangFrame m_audioFrame;
		//int32_t      m_nFrameSize;
		//int32_t      m_nFilterLen;
		//int32_t      m_nSampleRate;
		YangLoadLib m_lib;
		void loadLib();
			void unloadLib();
			SpeexPreprocessState *(*yang_speex_preprocess_state_init)(int32_t frame_size, int32_t sampling_rate);
				int32_t (*yang_speex_preprocess_ctl)(SpeexPreprocessState *st, int32_t request, void *ptr);
				void (*yang_speex_preprocess_state_destroy)(SpeexPreprocessState *st);
				int32_t (*yang_speex_preprocess_run)(SpeexPreprocessState *st, spx_int16_t *x);

				SpeexEchoState *(*yang_speex_echo_state_init_mc)(int32_t frame_size, int32_t filter_length, int32_t nb_mic, int32_t nb_speakers);
				int32_t (*yang_speex_echo_ctl)(SpeexEchoState *st, int32_t request, void *ptr);
				void (*yang_speex_echo_state_reset)(SpeexEchoState *st);
				void (*yang_speex_echo_playback)(SpeexEchoState *st, const spx_int16_t *play);
				void (*yang_speex_echo_capture)(SpeexEchoState *st, const spx_int16_t *rec, spx_int16_t *out);
				void (*yang_speex_echo_cancellation)(SpeexEchoState *st, const spx_int16_t *rec, const spx_int16_t *play, spx_int16_t *out);
				void (*yang_speex_echo_state_destroy)(SpeexEchoState *st);
};

#endif /* YANGAUDIOPROC_YANGAECSPEEX_H_ */
