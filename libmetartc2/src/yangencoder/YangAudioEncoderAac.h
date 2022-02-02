#ifndef __AudioAacEncoder__
#define __AudioAacEncoder__
#include "faac.h"

#include "yangutil/buffer/YangAudioBuffer.h"
#include "yangutil/buffer/YangAudioEncoderBuffer.h"
#include "yangutil/sys/YangLoadLib.h"
#include "yangencoder/YangAudioEncoder.h"

class YangAudioEncoderAac: public YangAudioEncoder
{
public:
	YangAudioEncoderAac();
	~YangAudioEncoderAac(void);
	void init(YangAudioInfo *pap);
	int32_t encoder(YangFrame* pframe,YangEncoderCallback* pcallback);

private:
	uint8_t *m_aacBuffer;

	int32_t frames;
	int32_t mnInputSamples;
	uint32_t  isamples, maxsample;
	unsigned long nSampleRate;
	uint32_t  nChannels;
	uint32_t  nPCMBitSize;
	unsigned long nInputSamples;
	unsigned long nMaxOutputBytes;

	int32_t nRet;
	int32_t nBytesRead;
	int32_t nPCMBufferSize;

private:
	YangLoadLib m_lib;
	void encoder(int32_t *p_buf);
	void closeAac();
	void saveWave();
	int32_t ret;//, isConvert;
	int32_t isRec;
	faacEncHandle hEncoder;
	void loadLib();
	void unloadLib();
	faacEncHandle  (*yang_faacEncOpen)(unsigned long sampleRate, uint32_t  numChannels,
			unsigned long *inputSamples,	unsigned long *maxOutputBytes);
	int32_t  (*yang_faacEncSetConfiguration)(faacEncHandle hEncoder,faacEncConfigurationPtr config);
	int32_t  (*yang_faacEncEncode)(faacEncHandle hEncoder, int32_t * inputBuffer, uint32_t  samplesInput,
				 uint8_t *outputBuffer, uint32_t  bufferSize);
	int32_t  (*yang_faacEncClose)(faacEncHandle hEncoder);
	faacEncConfigurationPtr
	  (*yang_faacEncGetCurrentConfiguration)(faacEncHandle hEncoder);
};

#endif
