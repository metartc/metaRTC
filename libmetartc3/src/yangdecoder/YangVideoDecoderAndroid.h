//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGDECODERYANGVIDEODECODERAndroid_H_
#define YANGDECODERYANGVIDEODECODERAndroid_H_
#include <yangdecoder/YangAudioDecoderHandles.h>
#include <yangutil/yangavinfotype.h>
#include "yangdecoder/YangVideoDecoder.h"
#include "yangutil/sys/YangLoadLib.h"



class YangVideoDecoderAndroid: public YangVideoDecoder {
public:
	YangVideoDecoderAndroid(YangVideoInfo *pcontext,
			YangVideoCodec pencdectype);
	virtual ~YangVideoDecoderAndroid();
	void parseRtmpHeader(uint8_t *p, int32_t pLen, int32_t *pwid, int32_t *phei, int32_t *pfps);
	void init();
	int32_t decode(YangFrame* videoFrame,YangYuvType yuvtype,YangDecoderCallback* pcallback);
	//int32_t decode( int32_t isIframe, uint8_t *pData, int32_t nSize, uint8_t *dest, int32_t *pnFrameReturned);
	void decode_close();
	static YangVideoHwType g_hwType;
protected:
	uint8_t* m_buffer;
		int32_t m_bufLen;
private:
	YangVideoCodec m_encDecType;
	YangVideoInfo *m_context;
	int32_t m_bitDepth;
	int32_t ret;

	int32_t m_width, m_height;
	int32_t yLen;
	int32_t uLen;
	int32_t allLen;
	uint8_t *buffer = NULL;
	int32_t usingHw;


	int32_t decode_1(YangFrame* videoFrame,YangYuvType yuvtype,YangDecoderCallback* pcallback);
	int32_t decode_2(YangFrame* videoFrame,YangYuvType yuvtype,YangDecoderCallback* pcallback);


	void parseHeaderH265(uint8_t *p, int32_t pLen, int32_t *pwid, int32_t *phei, int32_t *pfps);
	void parseHeaderH264(uint8_t *p, int32_t pLen, int32_t *pwid, int32_t *phei, int32_t *pfps);


};

#endif /* YANGDECODER_SRC_YANGH2645VIDEODECODERFFMPEG_H_ */
