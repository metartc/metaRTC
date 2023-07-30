//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangencoder/YangAudioEncoderMeta.h>
#include <yangutil/yangtype.h>



void YangAudioEncoderMeta::loadLib(){
	yang_faacEncOpen=(faacEncHandle  (*)(unsigned long sampleRate, uint32_t  numChannels,
			unsigned long *inputSamples, unsigned long *maxOutputBytes))m_lib.loadFunction("faacEncOpen");

	yang_faacEncSetConfiguration=(int32_t  (*)(faacEncHandle hEncoder,faacEncConfigurationPtr config))m_lib.loadFunction("faacEncSetConfiguration");
	yang_faacEncEncode=(int32_t  (*)(faacEncHandle hEncoder, int32_t * inputBuffer, uint32_t  samplesInput,
				 uint8_t *outputBuffer,
				 uint32_t  bufferSize))m_lib.loadFunction("faacEncEncode");
	yang_faacEncGetCurrentConfiguration=(faacEncConfigurationPtr
	  (*)(faacEncHandle hEncoder))m_lib.loadFunction("faacEncGetCurrentConfiguration");
	yang_faacEncClose=(int32_t  (*)(faacEncHandle hEncoder))m_lib.loadFunction("faacEncClose");

	yang_faacEncGetDecoderSpecificInfo=(int32_t  (*)(faacEncHandle hEncoder, uint8_t **ppBuffer,
							  unsigned long *pSizeOfDecoderSpecificInfo))m_lib.loadFunction("faacEncGetDecoderSpecificInfo");

}

void YangAudioEncoderMeta::unloadLib(){
	yang_faacEncGetCurrentConfiguration=NULL;
	yang_faacEncOpen=NULL;
	yang_faacEncSetConfiguration=NULL;
	yang_faacEncEncode=NULL;
	yang_faacEncClose=NULL;
	yang_faacEncGetDecoderSpecificInfo=NULL;

}
YangAudioEncoderMeta::YangAudioEncoderMeta(){
	unloadLib();
}

YangAudioEncoderMeta::~YangAudioEncoderMeta(){
	unloadLib();
	m_lib.unloadObject();
}


void YangAudioEncoderMeta::createMeta(uint8_t *pasc,int32_t *asclen){

	m_lib.loadObject("libfaac");
	loadLib();
	faacEncHandle hEncoder=NULL;
	unsigned long nSampleRate = 44100;
	uint32_t  nChannels = 2;

	unsigned long nInputSamples = 0;
	unsigned long nMaxOutputBytes = 0;
	hEncoder = yang_faacEncOpen(nSampleRate, nChannels, &nInputSamples,&nMaxOutputBytes);
	faacEncConfigurationPtr aconfiguration = yang_faacEncGetCurrentConfiguration(hEncoder);

		aconfiguration->version = MPEG4;

		aconfiguration->aacObjectType = LOW;	//MAIN;//LOW;//MAIN;
		aconfiguration->allowMidside = 1;
		aconfiguration->useTns = 0;
		aconfiguration->shortctl = SHORTCTL_NORMAL;
		//aconfiguration->
		//aconfiguration->nputformat=FAAC_INPUT_16BIT;
		aconfiguration->outputFormat = 0;

		aconfiguration->inputFormat = FAAC_INPUT_16BIT;
		aconfiguration->bitRate = 128000 / 2;

		int32_t nRet = yang_faacEncSetConfiguration(hEncoder, aconfiguration);


			uint8_t *asc=NULL;
			unsigned long len=0;
	        yang_faacEncGetDecoderSpecificInfo(hEncoder, &asc, &len);
            if(asc){
                memcpy(pasc,asc,len);
                *asclen=len;
                yang_faacEncClose(hEncoder);
                hEncoder=NULL;
            }

}
