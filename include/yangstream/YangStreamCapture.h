//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef INCLUDE_YANGSTREAM_YANGSTREAMCAPTURE_H_
#define INCLUDE_YANGSTREAM_YANGSTREAMCAPTURE_H_
#include <stdint.h>
#include <yangutil/yangavtype.h>

typedef struct{
	void* context;
	uint8_t* (*getVideoData)(void* pcontext);
	int32_t (*getVideoLen)(void* pcontext);
	int32_t (*getVideoFrametype)(void* pcontext);
	int64_t (*getVideoTimestamp)(void* pcontext);

	uint8_t* (*getAudioData)(void* pcontext);
	int32_t (*getAudioLen)(void* pcontext);
	int64_t (*getAudioTimestamp)(void* pcontext);
	YangAudioCodec (*getAudioType)(void* pcontext);

	void (*initVideo)(void* pcontext,int32_t transtype);
	void (*setVideoData)(void* pcontext,YangFrame *videoFrame, YangVideoCodec videoType);
	void (*setVideoMeta)(void* pcontext,uint8_t *p, int32_t plen, YangVideoCodec videoType);
	void (*setMetaTimestamp)(void* pcontext,int64_t timestamp);
	void (*setVideoFrametype)(void* pcontext,int32_t frametype);

	void (*setAudioData)(void* pcontext,YangFrame *audioFrame);
	void (*setAudioMetaData)(void* pcontext,uint8_t *p, int32_t plen);
	void (*setAudioFrametype)(void* pcontext,int32_t frametype);
	void (*initAudio)(void* pcontext,int32_t transType,int32_t sample,int32_t channel,YangAudioCodec audioType);

	YangFrame* (*getVideoFrame)(void* pcontext);
	YangFrame* (*getAudioFrame)(void* pcontext);


}YangStreamCapture;
#ifdef __cplusplus
extern "C"{
#endif
void yang_create_streamCapture(YangStreamCapture* stream);
void yang_destroy_streamCapture(YangStreamCapture* stream);
#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_YANGSTREAM_YANGSTREAMCAPTURE_H_ */
