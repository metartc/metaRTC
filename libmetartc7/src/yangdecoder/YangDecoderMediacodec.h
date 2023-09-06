//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGDECODERYANGVIDEODECODERAndroid_H_
#define YANGDECODERYANGVIDEODECODERAndroid_H_
#include <yangdecoder/YangAudioDecoderHandles.h>
#include <yangdecoder/YangVideoDecoder.h>
#include <yangavutil/video/YangYuvConvert.h>
#include <yangutil/sys/YangLoadLib.h>
#include <yangutil/yangavinfotype.h>


#if Yang_OS_ANDROID
#include <media/NdkMediaCodec.h>

class YangDecoderMediacodec: public YangVideoDecoder {
public:
	YangDecoderMediacodec(YangVideoInfo *pcontext,YangVideoCodec pencdectype);
	virtual ~YangDecoderMediacodec();
	void parseRtmpHeader(uint8_t *p, int32_t pLen, int32_t *pwid, int32_t *phei, int32_t *pfps);
	void init();
	int32_t decode(YangFrame* videoFrame,YangYuvType yuvtype,YangDecoderCallback* pcallback);
	void decode_close();
private:
	YangVideoCodec m_encDecType;
	YangVideoInfo *m_context;
	int32_t m_bitDepth;
	int32_t ret;
	yangbool m_isNv12;

	AMediaCodec* m_mediaCodec;
	uint8_t* m_nv12Buffer;

	int32_t m_width;
	int32_t m_height;
	int32_t m_yuvLen;
	int32_t yLen;
	int32_t uLen;
	int32_t tryagainCount;

	YangYuvConvert m_yuv;
};
#endif
#endif /* YANGDECODER_SRC_YANGH2645VIDEODECODERFFMPEG_H_ */
