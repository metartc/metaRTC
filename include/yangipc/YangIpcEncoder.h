//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef INCLUDE_YANGIPC_YANGIPCENCODER_H_
#define INCLUDE_YANGIPC_YANGIPCENCODER_H_

#include <yangutil/buffer2/YangVideoEncoderBuffer2.h>
#include <yangutil/sys/YangIni.h>
#include <pthread.h>
typedef struct{
	int32_t isConvert;
	int32_t isStart;
	pthread_t threadId;
	YangVideoEncoderBuffer2 *out_videoBuffer;
}YangEncoderSession;
typedef struct{
	YangEncoderSession session;
	int32_t (*init)(YangEncoderSession* session);
	void (*start)(YangEncoderSession* session);
	void (*stop)(YangEncoderSession* session);
	void (*sendMsgToEncoder)(YangEncoderSession* session,YangRequestType request);
}YangEncoderVideo;

void yang_create_videoEncoder(YangEncoderVideo* encoder);
void yang_destroy_videoEncoder(YangEncoderVideo* encoder);



#endif /* INCLUDE_YANGIPC_YANGIPCENCODER_H_ */
