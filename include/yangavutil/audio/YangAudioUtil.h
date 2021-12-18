/*
 * YangAudioUtil.h
 *
 *  Created on: 2020年9月28日
 *      Author: yang
 */

#ifndef SRC_YANGUTIL_YANGAUDIOUTIL_H_
#define SRC_YANGUTIL_YANGAUDIOUTIL_H_
#include <stdint.h>
#include <stddef.h>
#include <yangutil/yangavtype.h>
#include <yangavutil/audio/YangResample.h>
//#include <yangavutil/audio/YangSwResample.h>
class YangAudioResample{
public:
	YangAudioResample(int ms=20);
	virtual ~YangAudioResample();
	void initIn(int psample,int pchannel);
	void initOut(int psample,int pchannel);
	void init(int insample,int inchannel,int outsample,int outchannel,int ms);
	void setIntervalTime(int ms);
	int getOutBytes();
	int getInBytes();
	int getInSample();
	int getOutSample();
	int getInChannel();
	int getOutChannel();
	int resample(YangFrame* audioFrame);
private:
	int m_inSample;
	int m_outSample;
	int m_inChannel;
	int m_outChannel;

	int m_intervalTime;


	int m_inLen;
	int m_outLen;
	int m_inFrames;
	int m_outFrames;
	bool m_sampleTrans;
	bool m_channelTrans;
	bool m_isTrans;
	uint8_t* m_inBuf;
	uint8_t* m_outBuf;

	YangResample m_res;
        //YangSwResample m_res1;

	bool m_isInitTrans;
	void initTrans();
	void initParam();

};


int32_t MonoToStereo(int16_t *pData, int16_t *dData, int32_t samples_per_channel);
int32_t StereoToMono(const int16_t* src_audio,int16_t* dst_audio,int32_t samples_per_channe);
uint64_t Resample_s16(const int16_t *input, int16_t *output, int32_t inSampleRate, int32_t outSampleRate, uint64_t inputSize,
                      uint32_t channels);
short FloatS16ToS16(float v);
int16_t maxAbsValueW16C(const int16_t* vector, size_t length);
void ComputeLevel(const int16_t* data, size_t length);

#endif /* SRC_YANGUTIL_YANGAUDIOUTIL_H_ */
