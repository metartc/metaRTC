//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangavutil/audio/YangAudioUtil.h>
#include <yangutil/yangtype.h>

#include <yangutil/sys/YangLog.h>
#include <math.h>


void yang_audiores_initParam(YangAudioResampleContext* context) {
	if(context==NULL) return;

	context->inFrames = context->inSample * context->intervalTime / 1000;
	context->outFrames = context->outSample * context->intervalTime / 1000;
	context->inBytes = context->inFrames * 2 * context->inChannel;
	context->outBytes = context->outFrames * 2 * context->outChannel;
    context->sampleTrans = (context->inSample == context->outSample ? 0 : 1);
    context->channelTrans = (context->inChannel == context->outChannel ? 0 : 1);
    context->isTrans = (context->sampleTrans || context->channelTrans) ? 1 : 0;
}
void yang_audiores_initIn(YangAudioResampleContext* context,int32_t psample, int32_t pchannel) {
	if(context==NULL) return;

	context->inSample = psample;
	context->inChannel = pchannel;
    context->sampleTrans = (context->inSample == context->outSample ? 0 : 1);
    context->channelTrans = (context->inChannel == context->outChannel ? 0 : 1);
	context->inFrames = context->inSample * context->intervalTime / 1000;
	context->inBytes = context->inFrames * 2 * context->inChannel;
    context->isTrans = (context->sampleTrans || context->channelTrans) ? 1 : 0;
}
void yang_audiores_initOut(YangAudioResampleContext* context,int32_t psample, int32_t pchannel) {
	if(context==NULL) return;

	context->outSample = psample;
	context->outChannel = pchannel;
    context->sampleTrans = (context->inSample == context->outSample ? 0 : 1);
    context->channelTrans = (context->inChannel == context->outChannel ? 0 : 1);

	context->outFrames = context->outSample * context->intervalTime / 1000;
	context->outBytes = context->outFrames * 2 * context->outChannel;
    context->isTrans = (context->sampleTrans || context->channelTrans) ? 1 : 0;
}
void yang_audiores_init(YangAudioResampleContext* context,int32_t insample, int32_t inchannel, int32_t outsample,
		int32_t outchannel, int32_t ms) {
	if(context==NULL) return;

	context->inSample = insample;
	context->inChannel = inchannel;
	context->outSample = outsample;
	context->outChannel = outchannel;
	context->intervalTime = ms;
	yang_audiores_initParam(context);
}
void yang_audiores_initTrans(YangAudioResampleContext* context) {
	if(context==NULL) return;


	if(!context->isTrans) return;
		if(context->channelTrans){
			if (context->inBuf == NULL) 	context->inBuf =(uint8_t*) calloc( context->inFrames<<2,1);
		}
		if(context->sampleTrans){
				if (context->outBuf == NULL)	context->outBuf = (uint8_t*) calloc( context->outFrames<<2,1);

                context->res.init(context->res.context,context->outChannel, context->inSample, context->outSample);

		}

}




int32_t yang_audiores_resample(YangAudioResampleContext* context,YangFrame *audioFrame) {
    if(context==NULL) return 1;

    if (!context->isTrans||!audioFrame||!audioFrame->payload)		return Yang_Ok;

	if (!context->isInitTrans){
		yang_audiores_initTrans(context);
        context->isInitTrans = 1;
	}
	uint8_t *buf = audioFrame->payload;
	uint8_t *buf_in=buf;
	if (context->channelTrans) {
		if (context->inChannel == 1)
			MonoToStereo((short*) buf, (short*) context->inBuf, context->inFrames);
		else
			StereoToMono((short*) buf, (short*) context->inBuf, context->inFrames);


        buf_in=context->inBuf;
	}
	if (context->sampleTrans) {
		uint32_t outlen = 6400;
        context->res.resample(context->res.context,(short*) (buf_in), context->inFrames,(short*) context->outBuf, &outlen);
  		audioFrame->payload = context->outBuf;
	} else {
		audioFrame->payload = context->inBuf;
	}

	audioFrame->nb = context->outBytes;
    buf_in=NULL;
    buf=NULL;
	return Yang_Ok;
}



void yang_create_audioresample(YangAudioResample* res){
	if(res==NULL) return;
	res->context=(YangAudioResampleContext*)calloc(sizeof(YangAudioResampleContext),1);
	res->context->intervalTime = 20;
	res->context->inSample = 48000;
	res->context->outSample = 48000;
	res->context->inChannel = 2;
	res->context->outChannel = 2;
	res->context->inBuf = NULL;
	res->context->outBuf = NULL;
    res->context->sampleTrans = 0;
    res->context->channelTrans = 0;
    res->context->isInitTrans = 0;
    res->context->isTrans = 1;
	yang_audiores_initParam(res->context);
	res->init=yang_audiores_init;
	res->initIn=yang_audiores_initIn;
	res->initOut=yang_audiores_initOut;
	res->resample=yang_audiores_resample;
	yang_create_resample(&res->context->res);
}
void yang_destroy_audioresample(YangAudioResample* res){
	if(res==NULL) return;
	yang_destroy_resample(&res->context->res);
	yang_free(res->context->inBuf);
	yang_free(res->context->outBuf);
	yang_free(res->context);
}
int32_t MonoToStereo(int16_t *pData, int16_t *dData,
		int32_t samples_per_channel) {


	for (int32_t i = 0; i < samples_per_channel; i++) {
		dData[2 * i] = pData[i];
		dData[2 * i + 1] = pData[i];
	}
	return samples_per_channel * 2;
}

int32_t StereoToMono(const int16_t *src_audio, int16_t *dst_audio,
		int32_t samples_per_channel) {
	for (int i = 0; i < samples_per_channel; i++) {
		dst_audio[i] = ((int32_t)(src_audio[2 * i])
				+ src_audio[2 * i + 1]) >> 1;
	}
	return Yang_Ok;
}
/**
short FloatS16ToS16(float v) {

	v = yang_min(v, 32767.f);
	v = yang_max(v, -32768.f);
#ifdef _WIN32
	return (int16_t)(v + copysign(0.5f, v));
#else
	return (int16_t)(v + std::copysign(0.5f, v));
 #endif
}
**/
uint64_t Resample_s16(const int16_t *input, int16_t *output,
		int32_t inSampleRate, int32_t outSampleRate, uint64_t inputSize,
		uint32_t channels) {
	if (input == NULL)
		return 0;
	uint64_t outputSize = inputSize * outSampleRate / inSampleRate;
	if (output == NULL)
		return outputSize;
	double stepDist = ((double) inSampleRate / (double) outSampleRate);
	const uint64_t fixedFraction = (1LL << 32);
	const double normFixed = (1.0 / (1LL << 32));
	uint64_t step = ((uint64_t) (stepDist * fixedFraction + 0.5));
	uint64_t curOffset = 0;
	for (uint32_t i = 0; i < outputSize; i += 1) {
		for (uint32_t c = 0; c < channels; c += 1) {
			*output++ = (int16_t) (input[c]
					+ (input[c + channels] - input[c])
							* ((double) (curOffset >> 32)
									+ ((curOffset & (fixedFraction - 1))
											* normFixed)));
		}
		curOffset += step;
		input += (curOffset >> 32) * channels;
		curOffset &= (fixedFraction - 1);
	}
	return outputSize;
}

#define VOLUMEMAX   32767
int32_t SimpleCalculate_DB(short *pcmData, int32_t sample) {
	signed short ret = 0;
	if (sample > 0) {
		int32_t sum = 0;
		signed short *pos = (signed short*) pcmData;
		for (int32_t i = 0; i < sample; i++) {
			sum += abs(*pos);
			pos++;
		}
		ret = sum * 500.0 / (sample * VOLUMEMAX);
		if (ret >= 100) {
			ret = 100;
		}
	}
	return ret;
}
/**
static const float kMaxSquaredLevel = 32768 * 32768;
const float kMinLevel = 30.f;

float Process(const int16_t *data, size_t length) {
	float sum_square_ = 0;
	size_t sample_count_ = 0;
	for (size_t i = 0; i < length; ++i) {
		sum_square_ += data[i] * data[i];
	}
	sample_count_ += length;
	float rms = sum_square_ / (sample_count_ * kMaxSquaredLevel);
	//20log_10(x^0.5) = 10log_10(x)
	rms = 10 * log10(rms);
	if (rms < -kMinLevel)
		rms = -kMinLevel;
	rms = -rms;
	return (int)(rms + 0.5);
}

//计算分贝
int16_t maxAbsValueW16C(const int16_t *vector, size_t length) {
	size_t i = 0;
	int32_t absolute = 0, maximum = 0;
	for (i = 0; i < length; i++) {
		absolute = abs((int) vector[i]);
		if (absolute > maximum) {
			maximum = absolute;
		}
	}
	if (maximum > 32767) {
		maximum = 32767;
	}
	return (int16_t) maximum;
}
const char permutation[33] = { 0, 1, 2, 3, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 6, 7,
		7, 7, 7, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9 };
void ComputeLevel(const int16_t *data, size_t length) {
	int16_t _absMax = 0;
	int16_t _count = 0;
	char _currentLevel = 0;
	int16_t absValue(0);
	absValue = maxAbsValueW16C(data, length);
	if (absValue > _absMax)
		_absMax = absValue;
	if (_count++ == 10) {
		_count = 0;
		int32_t position = _absMax / 1000;
		if ((position == 0) && (_absMax > 250)) {
			position = 1;
		}
		_currentLevel = permutation[position];
		_absMax >>= 2;
	}
}

**/
