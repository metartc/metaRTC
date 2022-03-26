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
	void (*init)(void* handle,void* context,YangStreamConfig* pconf);

	 int32_t (*connectRtcServer)(void* context);
	 int32_t (*disconnectServer)(void* context);
	 int32_t (*getState)(void* context);
	 int32_t (*publishVideo)(void* context,YangStreamCapture* videoFrame);
	 int32_t (*publishAudio)(void* context,YangStreamCapture* audioFrame);
	YangReceiveCallback* recvcb;
	YangStreamConfig *streamconfig;
	YangContextStream* stream;
}YangRtcHandle;
#ifdef __cplusplus
extern "C"{
#endif
void yang_create_rtcstream_handle(YangRtcHandle* handle,YangAVInfo* avinfo,YangContextStream* stream);
void yang_destroy_rtcstream_handle(YangRtcHandle* handle);
#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_YANGRTC_YANGRTCHANDLE_H_ */
