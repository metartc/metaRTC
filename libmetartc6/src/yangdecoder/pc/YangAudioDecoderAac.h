//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YangAudioDecoderAac_H
#define YangAudioDecoderAac_H

#include <yangutil/buffer/YangAudioEncoderBuffer.h>
#include <yangdecoder/YangAudioDecoder.h>
#include <yangutil/sys/YangLoadLib.h>
#include <yangutil/yangavinfotype.h>
#include <faad.h>
#include <vector>

using namespace std;
class YangAudioDecoderAac :public YangAudioDecoder
{
public:
	YangAudioDecoderAac(YangAudioParam  *pcontext);
	 ~YangAudioDecoderAac();
	void init();
	int32_t decode(YangFrame* pframe,YangDecoderCallback* pcallback);


protected:

private:

	void closeAacdec();
	int32_t isConvert;
	uint8_t *temp;
	NeAACDecHandle m_handle;

	int32_t isFirst;
	NeAACDecFrameInfo m_info;
	unsigned long m_samplerate;
	uint8_t m_channel;
	unsigned long m_bufLen;
	uint8_t *m_buffer;
	YangLoadLib m_lib;
	void loadLib();
	void unloadLib();
	 NeAACDecHandle (*yang_NeAACDecOpen)(void);
	 NeAACDecConfigurationPtr (*yang_NeAACDecGetCurrentConfiguration)(NeAACDecHandle hDecoder);
	 uint8_t (*yang_NeAACDecSetConfiguration)(NeAACDecHandle hDecoder,
	                                                   NeAACDecConfigurationPtr config);
	 long (*yang_NeAACDecInit)(NeAACDecHandle hDecoder,
	                              uint8_t *buffer,
	                              unsigned long buffer_size,
	                              unsigned long *samplerate,
	                              uint8_t *channels);
	 void* (*yang_NeAACDecDecode)(NeAACDecHandle hDecoder,
	                                 NeAACDecFrameInfo *hInfo,
	                                 uint8_t *buffer,
	                                 unsigned long buffer_size);
	 void (*yang_NeAACDecClose)(NeAACDecHandle hDecoder);


};

#endif // AACDECODER_H
