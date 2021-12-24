/*
 * YangAudioUtil.cpp
 *
 *  Created on: 2020年9月28日
 *      Author: yang
 */
#include <yangutil/yangtype.h>
#include <yangutil/sys/YangLog.h>
#include <math.h>
#include <yangavutil/audio/YangAudioUtil.h>
#include <algorithm>

YangAudioResample::YangAudioResample(int ms) {
	m_intervalTime = ms;
	m_inSample = 48000;
	m_outSample = 48000;
	m_inChannel = 2;
	m_outChannel = 2;
	m_inBuf = NULL;
	m_outBuf = NULL;
	m_sampleTrans = false;
	m_channelTrans = false;
	m_isInitTrans = false;
	m_isTrans = true;
	initParam();
}
YangAudioResample::~YangAudioResample() {
	yang_deleteA(m_inBuf);
	yang_deleteA(m_outBuf);
}
void YangAudioResample::initIn(int psample, int pchannel) {
	m_inSample = psample;
	m_inChannel = pchannel;
	m_sampleTrans = (m_inSample == m_outSample ? false : true);
	m_channelTrans = (m_inChannel == m_outChannel ? false : true);
	m_inFrames = m_inSample * m_intervalTime / 1000;
	m_inLen = m_inFrames * 2 * m_inChannel;
	m_isTrans = (m_sampleTrans || m_channelTrans) ? true : false;
}
void YangAudioResample::initOut(int psample, int pchannel) {
	m_outSample = psample;
	m_outChannel = pchannel;
	m_sampleTrans = (m_inSample == m_outSample ? false : true);
	m_channelTrans = (m_inChannel == m_outChannel ? false : true);

	m_outFrames = m_outSample * m_intervalTime / 1000;
	m_outLen = m_outFrames * 2 * m_outChannel;
	m_isTrans = (m_sampleTrans || m_channelTrans) ? true : false;
}
void YangAudioResample::init(int insample, int inchannel, int outsample,
		int outchannel, int ms) {
	m_inSample = insample;
	m_inChannel = inchannel;
	m_outSample = outsample;
	m_outChannel = outchannel;
	m_intervalTime = ms;
	initParam();
}
void YangAudioResample::initTrans() {
    yang_trace("\n...insample==%d,inchannel==%d,out sample==%d,outchannel==%d....\n",m_inSample,m_inChannel,m_outSample,m_outChannel);
    yang_trace("\n...m_isTrans==%d,m_channelTrans==%d,out m_sampleTrans==%d....\n",m_isTrans,m_channelTrans,m_sampleTrans);
	if(!m_isTrans) return;
		if(m_channelTrans){
			if (m_inBuf == NULL) 	m_inBuf = new uint8_t[m_inFrames<<2];
		}
		if(m_sampleTrans){
				if (m_outBuf == NULL)	m_outBuf = new uint8_t[m_outFrames<<2];
                //m_res1.init(m_outChannel, m_inSample, m_outSample,m_outFrames);
                m_res.init(m_outChannel, m_inSample, m_outSample);

		}

}
void YangAudioResample::setIntervalTime(int ms) {
	m_intervalTime = ms;
}

void YangAudioResample::initParam() {
	m_inFrames = m_inSample * m_intervalTime / 1000;
	m_outFrames = m_outSample * m_intervalTime / 1000;
	m_inLen = m_inFrames * 2 * m_inChannel;
	m_outLen = m_outFrames * 2 * m_outChannel;
	m_isTrans = (m_sampleTrans || m_channelTrans) ? true : false;
}

int YangAudioResample::resample(YangFrame *audioFrame) {
    if (!m_isTrans||!audioFrame||!audioFrame->payload)		return Yang_Ok;

	if (!m_isInitTrans){
		initTrans();
		m_isInitTrans = true;
	}
	uint8_t *buf = audioFrame->payload;
    uint8_t *buf_in=buf;
	if (m_channelTrans) {
		if (m_inChannel == 1)
			MonoToStereo((short*) buf, (short*) m_inBuf, m_inFrames);
		else
			StereoToMono((short*) buf, (short*) m_inBuf, m_inFrames);


        buf_in=m_inBuf;
	}
	if (m_sampleTrans) {
		uint32_t outlen = 6400;
        m_res.resample((short*) (buf_in), m_inFrames,(short*) m_outBuf, &outlen);
       // m_res1.resample( (m_channelTrans ? m_inBuf : buf), m_inFrames, m_outBuf, &outlen);
       // yang_trace("out_%d,",outlen);
		audioFrame->payload = m_outBuf;
	} else {
		audioFrame->payload = m_inBuf;
	}

	audioFrame->nb = m_outLen;
    buf_in=NULL;
    buf=NULL;
	return Yang_Ok;
}
int YangAudioResample::getOutBytes() {
	return m_outLen;
}

int YangAudioResample::getInBytes() {
	return m_inLen;
}
int YangAudioResample::getInSample() {
	return m_inSample;
}
int YangAudioResample::getOutSample() {
	return m_outSample;
}

int YangAudioResample::getInChannel() {
	return m_inChannel;
}
int YangAudioResample::getOutChannel() {
	return m_outChannel;
}

int32_t MonoToStereo(int16_t *pData, int16_t *dData,
		int32_t samples_per_channel) {

	/**for (int32_t i = 0; i < nSize; i++) {
	 if (i % 2 == 0) {
	 dData[i] = pData[i / 2];
	 } else {
	 dData[i] = pData[i - 1];
	 }
	 }**/

	for (int32_t i = 0; i < samples_per_channel; i++) {
		dData[2 * i] = pData[i];
		dData[2 * i + 1] = pData[i];
	}
	return samples_per_channel * 2;
}

int32_t StereoToMono(const int16_t *src_audio, int16_t *dst_audio,
		int32_t samples_per_channel) {
	for (int i = 0; i < samples_per_channel; i++) {
		dst_audio[i] = (static_cast<int32_t>(src_audio[2 * i])
				+ src_audio[2 * i + 1]) >> 1;
	}
	return Yang_Ok;
}

short FloatS16ToS16(float v) {

	v = yang_min(v, 32767.f);
	v = yang_max(v, -32768.f);
#ifdef _WIN32
	return static_cast<int16_t>(v + copysign(0.5f, v));
#else
	return static_cast<int16_t>(v + std::copysign(0.5f, v));
 #endif
}

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

static const float kMaxSquaredLevel = 32768 * 32768;
constexpr float kMinLevel = 30.f;

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
	return static_cast<int>(rms + 0.5);
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
