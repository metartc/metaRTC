//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGENCODER_YANGENCODERGPU_H_
#define SRC_YANGENCODER_YANGENCODERGPU_H_
#include <yangencoder/YangVideoEncoder.h>
#include <yangutil/buffer/YangVideoBuffer.h>
#include <yangutil/buffer/YangVideoEncoderBuffer.h>
#include <yangutil/sys/YangIni.h>
#include "codec/YangH264EncoderGpu.h"
class YangEncoderGpu: public YangVideoEncoder {
public:
    YangEncoderGpu();
	virtual ~YangEncoderGpu();
		//static void initX265Param(YangVideoInfo *pvp,YangVideoEncInfo *penc,x265_param *param);
        int32_t init(YangContext* pcontext,YangVideoInfo *pvp);
		void setVideoMetaData(YangVideoMeta *pvmd);
		bool isSupported();
		void parseRtmpHeader(uint8_t *p, int32_t pLen, int32_t *pwid, int32_t *phei, int32_t *pfps);
        void sendMsgToEncoder(YangRtcEncoderMessage *msg);
	private:

		int32_t m_sendKeyframe;
		//int32_t encode(uint8_t *src,int32_t buflen, uint8_t *dest, int32_t *destLen, int32_t *frametype);
		int32_t encode(YangFrame* pframe, YangEncoderCallback* pcallback);
	protected:
        YangH264EncoderGpu* m_encoder;
        YangFrame m_videoFrame;


};

#endif /* SRC_YANGENCODER_YANGENCODERGPU_H_ */
