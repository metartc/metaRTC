//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGDECODER_INCLUDE_YANGAUDIODECODER_H_
#define YANGDECODER_INCLUDE_YANGAUDIODECODER_H_
#include <yangutil/yangavinfotype.h>
#include "yangstream/YangStreamType.h"
#include "YangDecoder.h"


class YangAudioDecoder{
public:
	YangAudioDecoder();
	virtual ~YangAudioDecoder();
    virtual void init()=0;
    virtual int32_t decode(YangFrame* pframe,YangDecoderCallback* pcallback)=0;
    int32_t m_uid;
protected:
	int32_t m_isInit;
	int32_t m_frameSize;
	int32_t m_alen;
	YangAudioParam *m_context;
	uint8_t* m_dstBuffer;
	int32_t m_dstLen;
};

#endif /* YANGDECODER_INCLUDE_YANGAUDIODECODER_H_ */
