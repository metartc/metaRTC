//
// Copyright (c) 2019-2025 yanggaofeng
//

#ifndef SRC_YANGDECODER_YANGVIDEODECODERMAC_H_
#define SRC_YANGDECODER_YANGVIDEODECODERMAC_H_

#include <yangdecoder/YangVideoDecoder.h>

#include <yangutil/yangtype.h>
#include <yangmac/YangDecoderMac.h>

class YangVideoDecoderMac: public YangVideoDecoder {
public:
	YangVideoDecoderMac();
	virtual ~YangVideoDecoderMac();
	YangDecoderCallback* m_callback;
	void parseRtmpHeader(uint8_t *p, int32_t pLen, int32_t *pwid, int32_t *phei, int32_t *pfps);
	void init();
	int32_t decode(YangFrame* videoFrame,YangColorSpace yuvtype,YangDecoderCallback* pcallback);

private:
	uint8_t* m_buffer;
	int32_t m_bufLen;
	YangCodecCallback m_macCallback;
private:
	YangCVideoDecoder* m_decoder;


};

#endif /* SRC_YANGDECODER_YANGVIDEODECODERMAC_H_ */
