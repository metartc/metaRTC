//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef __YangEncoderMediacodec__
#define __YangEncoderMediacodec__
#include <yangencoder/YangVideoEncoder.h>
#include <yangutil/buffer/YangVideoBuffer.h>
#include <yangutil/buffer/YangVideoEncoderBuffer.h>

#include <yangutil/sys/YangIni.h>
#include <yangutil/sys/YangLoadLib.h>


#ifdef __ANDROID__

#include <media/NdkMediaCodec.h>

class YangEncoderMediacodec: public YangVideoEncoder {
public:
	YangEncoderMediacodec();
	~YangEncoderMediacodec(void);

	int32_t init(YangVideoInfo *pvp,YangVideoEncInfo *penc);

	void setVideoMetaData(YangVideoMeta *pvmd);
	void parseRtmpHeader(uint8_t *p, int32_t pLen, int32_t *pwid, int32_t *phei, int32_t *pfps);
	void sendKeyFrame();
private:
	AMediaCodec* m_mediaCodec;
	int32_t m_sendKeyframe;
	int32_t m_width;
	int32_t m_height;
	int32_t m_yuvLen;
	int32_t encode(YangFrame* pframe, YangEncoderCallback* pcallback);
protected:


	void saveFile(char *fileName, uint8_t *pBuffer, int32_t BufferLen);

private:
	YangSample m_sps;
	YangSample m_pps;
	int32_t metaLen;
	bool metaInit;
	uint8_t temp[50];
	bool m_hasHeader;
};
#endif

#endif
