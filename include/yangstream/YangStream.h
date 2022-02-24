//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef INCLUDE_YANGSTREAM_YANGSTREAM_H_
#define INCLUDE_YANGSTREAM_YANGSTREAM_H_
#include <stdint.h>
#include <yangstream/YangStreamCapture.h>
#include <yangutil/yangavinfotype.h>
#include "YangStreamType.h"
typedef struct{
	int32_t transtype;
	int32_t streamInit;
	int32_t uid;
	int32_t audioStream;
	int32_t videoStream;
	int32_t netState;
	void* context;
	YangAVInfo* avinfo;
	YangContextStream* stream;
	YangReceiveCallback *data;
	YangStreamConfig streamconfig;
}YangStreamContext;
typedef struct {
	YangStreamContext* context;
	//void (*init)(YangStreamContext* context,YangStreamConfig *pconf);
	int32_t (*connectServer)(YangStreamContext* context);
	int32_t (*disConnectServer)(YangStreamContext* context);
	int32_t (*reconnect)(YangStreamContext* context);
	int32_t (*receiveData)(YangStreamContext* context,int32_t *plen);
	int32_t (*publishVideoData)(YangStreamContext* context,YangStreamCapture *videoFrame);
	int32_t (*publishAudioData)(YangStreamContext* context,YangStreamCapture *audioFrame);
	int32_t (*getConnectState)(YangStreamContext* context);
	int32_t (*isconnected)(YangStreamContext* context);


}YangStreamHandle;
#ifdef __cplusplus
extern "C"{
#endif
void yang_create_stream(int32_t transType,YangStreamHandle* streamHandle,int32_t puid,YangStreamConfig* streamconfig,YangAVInfo* pcontext,YangContextStream* stream,YangReceiveCallback* callback) ;
void yang_destroy_stream(YangStreamHandle* stream);
#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_YANGSTREAM_YANGSTREAM_H_ */
