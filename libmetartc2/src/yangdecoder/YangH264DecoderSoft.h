/*
 * YangH264DecoderSoft.h
 *
 *  Created on: 2020年9月28日
 *      Author: yang
 */

#ifndef YANGDECODER_SRC_YANGH264DECODERSOFT_H_
#define YANGDECODER_SRC_YANGH264DECODERSOFT_H_
#include "stdint.h"
#include <yangdecoder/YangVideoDecoder.h>
#include <yangutil/sys/YangLoadLib.h>
#include "YangH264Dec.h"

class YangH264DecoderSoft: public YangVideoDecoder {
public:
	YangH264DecoderSoft();
	virtual ~YangH264DecoderSoft();
	void parseRtmpHeader(uint8_t *p, int32_t pLen, int32_t *pwid, int32_t *phei, int32_t *pfps);
	void init();
	int32_t decode(YangFrame* videoFrame,YangYuvType yuvtype,YangDecoderCallback* pcallback);
	//int32_t decode(int32_t isIframe, uint8_t *pData, int32_t nSize, uint8_t *dest,int32_t *pnFrameReturned);
private:
	uint8_t* m_buffer;
	int32_t m_bufLen;
private:
	struct YangH264DecContext *context;
	void getH264RtmpHeader(uint8_t *buf, uint8_t *src, int32_t *hLen);
	YangLoadLib m_lib;

	struct YangH264DecContext* (*yang_getYangH264DecContext)();
	void (*yang_init_H264DecCont)(struct YangH264DecContext *cont,
			uint8_t *headers, int32_t headerLen);
	int32_t (*yang_decode)(struct YangH264DecContext *cont, int32_t isIframe,
			uint8_t *pData, int32_t nSize, enum YangYuvType pyuvType,uint8_t *dest,
			int32_t *pnFrameReturned);
	void (*yang_decode_close)(struct YangH264DecContext *cont);
};

#endif /* YANGDECODER_SRC_YANGH264DECODERSOFT_H_ */
