//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGDECODER_SRC_YANGH264DECODERSOFT_H_
#define YANGDECODER_SRC_YANGH264DECODERSOFT_H_
#include <yangdecoder/YangVideoDecoder.h>
#include <yangutil/sys/YangLoadLib.h>
#include <yangutil/yangtype.h>

extern "C"{
#include <yangdecoder/YangH264Dec.h>
}

class YangH264DecoderSoft: public YangVideoDecoder {
public:
	YangH264DecoderSoft();
	virtual ~YangH264DecoderSoft();
	void parseRtmpHeader(uint8_t *p, int32_t pLen, int32_t *pwid, int32_t *phei, int32_t *pfps);
	void init();
	int32_t decode(YangFrame* videoFrame,YangColorSpace yuvtype,YangDecoderCallback* pcallback);

private:
	uint8_t* m_buffer;
	int32_t m_bufLen;
private:
	struct YangH264DecContext *context;
	void getH264RtmpHeader(uint8_t *buf, uint8_t *src, int32_t *hLen);
#if	Yang_Enable_H264Decoder_So
	YangLoadLib m_lib;

	struct YangH264DecContext* (*yang_getYangH264DecContext)();
	//void  (*yang_parseHeader)(uint8_t *p,int32_t pLen,int32_t *pwid,int32_t *phei,int32_t *pfps);
	void (*yang_init_H264DecCont)(struct YangH264DecContext *cont,
			uint8_t *headers, int32_t headerLen);
	int32_t (*yang_decode)(struct YangH264DecContext *cont, int32_t isIframe,
			uint8_t *pData, int32_t nSize, enum YangYuvType pyuvType,uint8_t *dest,
			int32_t *pnFrameReturned);
	void (*yang_decode_close)(struct YangH264DecContext *cont);
#else
	#define yang_getYangH264DecContext getYangH264DecContext
	//#define yang_parseHeader  parseHeader
	#define yang_init_H264DecCont init_H264DecCont
    //#define yang_decode decode
	#define yang_decode_close decode_close
#endif
};

#endif /* YANGDECODER_SRC_YANGH264DECODERSOFT_H_ */
