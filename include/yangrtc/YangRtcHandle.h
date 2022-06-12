//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef INCLUDE_YANGRTC_YANGRTCHANDLE_H_
#define INCLUDE_YANGRTC_YANGRTCHANDLE_H_
#include <yangstream/YangStreamCapture.h>
#include <yangstream/YangStreamType.h>
#include <yangutil/yangavinfotype.h>

typedef struct{
	void* context;
	void (*init)(void* context);
	int32_t (*getAnswerSdp)(void* context,char* answer);
	int32_t (*startRtc)(void* context,char* sdp);
	int32_t (*stopRtc)(void* context);
	int32_t (*connectRtcServer)(void* context);
	int32_t (*disconnectServer)(void* context);
	int32_t (*getState)(void* context);
	int32_t (*isAlive)(void* context);
	int32_t (*publishVideo)(void* context,YangStreamCapture* videoFrame);
	int32_t (*publishAudio)(void* context,YangStreamCapture* audioFrame);
	int32_t (*publishMsg)(void* context,YangFrame* msgFrame);
	int32_t (*sendRtcMessage)(void* context,YangRtcMessageType mess);

	YangStreamConfig streamconfig;
}YangRtcHandle;
#ifdef __cplusplus
extern "C"{
#endif
void yang_create_rtcstream_handle(YangRtcHandle* handle,YangAVInfo* avinfo);
void yang_destroy_rtcstream_handle(YangRtcHandle* handle);
#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_YANGRTC_YANGRTCHANDLE_H_ */
