//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGENCODER_INCLUDE_YANGVideoENCODER_H_
#define YANGENCODER_INCLUDE_YANGVideoENCODER_H_
#include <yangutil/yangavinfotype.h>
#include "stdint.h"
#include "YangEncoder.h"
typedef struct{
	YangRequestType request;
	int32_t requestValue;
}YangRtcEncoderMessage;
class YangVideoEncoder{
public:
	YangVideoEncoder();
	virtual ~YangVideoEncoder();
    virtual  int32_t init(YangContext* pcontext,YangVideoInfo* pvideoInfo)=0;
	virtual int32_t encode(YangFrame* pframe, YangEncoderCallback* pcallback)=0;
	virtual void setVideoMetaData(YangVideoMeta *pvmd)=0;
	virtual void sendMsgToEncoder(YangRtcEncoderMessage *msg)=0;
protected:
    int32_t m_isInit;
    YangVideoInfo m_videoInfo;
    YangVideoEncInfo m_enc;
    uint8_t* m_vbuffer;
   void setVideoPara(YangVideoInfo *pap,YangVideoEncInfo *penc);
};

#endif /* YANGENCODER_INCLUDE_YANGENCODER_H_ */
