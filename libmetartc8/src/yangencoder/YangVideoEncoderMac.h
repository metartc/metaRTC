//
// Copyright (c) 2019-2026 yanggaofeng
//

#ifndef SRC_YANGENCODER_YANGVIDEOENCODERMAC_H_
#define SRC_YANGENCODER_YANGVIDEOENCODERMAC_H_

#include <yangencoder/YangVideoEncoder.h>
#include <yangutil/buffer/YangVideoBuffer.h>
#include <yangutil/buffer/YangVideoEncoderBuffer.h>

#include <yangmac/YangEncoderMac.h>

class YangVideoEncoderMac: public YangVideoEncoder {
public:
	YangVideoEncoderMac();
	virtual ~YangVideoEncoderMac();
	YangEncoderCallback* m_callback;

	int32_t init(YangVideoInfo* videoInfo,YangVideoEncInfo* encInfo);

	void parseHeader(uint8_t *p, int32_t pLen, int32_t *pwid, int32_t *phei, int32_t *pfps);
	void sendMsgToEncoder(YangRtcEncoderMessage *msg);

	int32_t encode(YangFrame* pframe, YangEncoderCallback* pcallback);

private:
	YangCVideoEncoder* m_encoder;
	YangCodecCallback m_mppCallback;

};

#endif /* SRC_YANGENCODER_YANGVIDEOENCODERMAC_H_ */
