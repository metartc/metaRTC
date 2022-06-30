//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef __YangOpenH264Encoder__
#define __YangOpenH264Encoder__
#include <yangencoder/YangVideoEncoder.h>
#include <yangutil/buffer/YangVideoBuffer.h>
#include <yangutil/buffer/YangVideoEncoderBuffer.h>
#include <yangutil/sys/YangIni.h>

#include <wels/codec_api.h>



class YangOpenH264Encoder: public YangVideoEncoder {
public:
	YangOpenH264Encoder();
	~YangOpenH264Encoder(void);

	int32_t init(YangContext* pcontext,YangVideoInfo* pvideoInfo);

	void setVideoMetaData(YangVideoMeta *pvmd);
	void parseRtmpHeader(uint8_t *p, int32_t pLen, int32_t *pwid, int32_t *phei, int32_t *pfps);
	void sendMsgToEncoder(YangRtcEncoderMessage *msg);
private:

	int32_t m_sendKeyframe;
	int32_t encode(YangFrame* pframe, YangEncoderCallback* pcallback);
protected:

	ISVCEncoder *m_264Handle;


	SSourcePicture m_pic;
	SFrameBSInfo m_einfo;
	int32_t m_yuvLen;
	int32_t m_vlen;


private:

	bool m_hasHeader;

};
#endif
