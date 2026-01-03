//
// Copyright (c) 2019-2026 yanggaofeng
//
#ifndef __YangEncoderMediacodec__
#define __YangEncoderMediacodec__
#include <yangencoder/YangVideoEncoder.h>
#include <yangutil/buffer/YangVideoBuffer.h>
#include <yangutil/buffer/YangVideoEncoderBuffer.h>
#include <yangutil/sys/YangLoadLib.h>
#include <yangavutil/video/YangYuvConvert.h>

#if Yang_OS_ANDROID

#include <media/NdkMediaCodec.h>

class YangEncoderMediacodec: public YangVideoEncoder {
public:
	YangEncoderMediacodec();
	~YangEncoderMediacodec(void);

	int32_t init(YangVideoInfo* videoInfo,YangVideoEncInfo* encInfo);

	void parseHeader(uint8_t *p, int32_t pLen, int32_t *pwid, int32_t *phei, int32_t *pfps);
	void sendMsgToEncoder(YangRtcEncoderMessage *msg);
private:
	AMediaCodec* m_mediaCodec;
	YangYuvConvert m_yuv;
	int32_t m_sendKeyframe;
	int32_t m_width;
	int32_t m_height;
	int32_t m_yuvLen;
	int32_t m_colorSpace;
	int32_t encode(YangFrame* pframe, YangEncoderCallback* pcallback);

private:
	int32_t metaLen;
	bool metaInit;
	bool m_hasHeader;
};
#endif

#endif
