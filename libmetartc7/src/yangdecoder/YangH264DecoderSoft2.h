//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGDECODER_SRC_YANGH264DECODERSOFT2_H_
#define YANGDECODER_SRC_YANGH264DECODERSOFT2_H_

#include <yangdecoder/YangVideoDecoder.h>
#include <yangdecoder/YangDecoderH264.h>
#include <yangutil/sys/YangLoadLib.h>
#include <yangutil/yangtype.h>


#if !Yang_Enable_H264Decoder_So
class YangH264DecoderSoft2: public YangVideoDecoder {
public:
	YangH264DecoderSoft2();
	virtual ~YangH264DecoderSoft2();
	void parseRtmpHeader(uint8_t *p, int32_t pLen, int32_t *pwid, int32_t *phei, int32_t *pfps);
	void init();
	int32_t decode(YangFrame* videoFrame,YangYuvType yuvtype,YangDecoderCallback* pcallback);

private:
	uint8_t* m_buffer;
	int32_t m_bufLen;
private:

	void getH264RtmpHeader(uint8_t *buf, uint8_t *src, int32_t *hLen);
	YangDecoderH264* m_decoder;
};
#endif
#endif /* YANGDECODER_SRC_YANGH264DECODERSOFT_H_ */
