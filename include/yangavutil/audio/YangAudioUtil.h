//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGUTIL_YANGAUDIOUTIL_H_
#define SRC_YANGUTIL_YANGAUDIOUTIL_H_
#include <stdint.h>
#include <stddef.h>
#include <yangutil/yangavtype.h>
#include <yangavutil/audio/YangResample.h>
typedef struct {
	int32_t inSample;
	int32_t outSample;
	int32_t inChannel;
	int32_t outChannel;

	int32_t intervalTime;

	int32_t inBytes;
	int32_t outBytes;
	int32_t inFrames;
	int32_t outFrames;
	int32_t isInitTrans;
	int32_t sampleTrans;
	int32_t channelTrans;
	int32_t isTrans;
	uint8_t *inBuf;
	uint8_t *outBuf;

	YangResample res;
} YangAudioResampleContext;
typedef struct {
	YangAudioResampleContext* context;
	void (*initIn)(YangAudioResampleContext* context,int32_t psample,int32_t pchannel);
	void (*initOut)(YangAudioResampleContext* context,int32_t psample,int32_t pchannel);
	void (*init)(YangAudioResampleContext* context,int32_t insample,int32_t inchannel,int32_t outsample,int32_t outchannel,int32_t ms);
	int32_t (*resample)(YangAudioResampleContext* context,YangFrame* audioFrame);
}YangAudioResample;
#ifdef __cplusplus
extern "C" {
#endif
void yang_create_audioresample(YangAudioResample* res);
void yang_destroy_audioresample(YangAudioResample* res);
int32_t MonoToStereo(int16_t *pData, int16_t *dData, int32_t samples_per_channel);
int32_t StereoToMono(const int16_t* src_audio,int16_t* dst_audio,int32_t samples_per_channe);
uint64_t Resample_s16(const int16_t *input, int16_t *output, int32_t inSampleRate, int32_t outSampleRate, uint64_t inputSize,
                      uint32_t channels);
short FloatS16ToS16(float v);
int16_t maxAbsValueW16C(const int16_t* vector, size_t length);
void ComputeLevel(const int16_t* data, size_t length);
#ifdef __cplusplus
}
#endif



#endif /* SRC_YANGUTIL_YANGAUDIOUTIL_H_ */
