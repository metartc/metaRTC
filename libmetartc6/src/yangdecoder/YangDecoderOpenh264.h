//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGDECODER_SRC_YangDecoderOpenh264_H_
#define YANGDECODER_SRC_YangDecoderOpenh264_H_
#include <yangdecoder/YangVideoDecoder.h>
#include <yangutil/sys/YangLoadLib.h>
#include <yangutil/yangtype.h>
#include <wels/codec_api.h>
class YangDecoderOpenh264: public YangVideoDecoder {
public:
	YangDecoderOpenh264();
	virtual ~YangDecoderOpenh264();
	void parseRtmpHeader(uint8_t *p, int32_t pLen, int32_t *pwid, int32_t *phei, int32_t *pfps);
	void init();
	int32_t decode(YangFrame* videoFrame,YangYuvType yuvtype,YangDecoderCallback* pcallback);

private:
	uint8_t* m_buffer;
	int32_t m_bufLen;
	int32_t m_width,m_height;
	int32_t m_uPos,m_vPos;
	SBufferInfo m_dstBufInfo;
private:
	ISVCDecoder *m_svcDecoder;
	void getH264RtmpHeader(uint8_t *buf, uint8_t *src, int32_t *hLen);
	void writeData(YangFrame* videoFrame,uint8_t** pData,YangDecoderCallback* pcallback);

};

#endif /* YANGDECODER_SRC_YANGH264DECODERSOFT_H_ */
