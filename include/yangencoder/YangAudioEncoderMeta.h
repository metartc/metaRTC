//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGENCODER_INCLUDE_YANGAUDIOENCODERMETA_H_
#define YANGENCODER_INCLUDE_YANGAUDIOENCODERMETA_H_
#include "faac.h"
#include "yangutil/sys/YangLoadLib.h"

class YangAudioEncoderMeta{
public:
	YangAudioEncoderMeta();
	~YangAudioEncoderMeta();
	void createMeta(uint8_t *pasc,int32_t *asclen);
private:
	YangLoadLib m_lib;
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


	int32_t  (*yang_faacEncGetDecoderSpecificInfo)(faacEncHandle hEncoder, uint8_t **ppBuffer,
						  unsigned long *pSizeOfDecoderSpecificInfo);
};



#endif /* YANGENCODER_INCLUDE_YANGAUDIOENCODERMETA_H_ */
