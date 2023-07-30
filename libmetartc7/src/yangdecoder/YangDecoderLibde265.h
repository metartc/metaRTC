//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGDECODER_SRC_YangDecoderLibde265_H_
#define YANGDECODER_SRC_YangDecoderLibde265_H_
#include <yangdecoder/YangVideoDecoder.h>
#include <yangutil/sys/YangLoadLib.h>
#include <yangutil/yangtype.h>

class YangDecoderLibde265: public YangVideoDecoder {
public:
	YangDecoderLibde265();
	virtual ~YangDecoderLibde265();
	void parseRtmpHeader(uint8_t *p, int32_t pLen, int32_t *pwid, int32_t *phei, int32_t *pfps);
	void init();
	int32_t decode(YangFrame* videoFrame,YangYuvType yuvtype,YangDecoderCallback* pcallback);

private:
	uint8_t* m_buffer;
	int32_t m_bufLen;
private:

	void getH264RtmpHeader(uint8_t *buf, uint8_t *src, int32_t *hLen);

};

#endif /* YANGDECODER_SRC_YANGH264DECODERSOFT_H_ */
