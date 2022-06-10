//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef INCLUDE_YANGRTC_YANGMETACONNECTION_H_
#define INCLUDE_YANGRTC_YANGMETACONNECTION_H_
#include <yangutil/yangavctype.h>
typedef struct{
	void *context;
	void (*init)(void* context,int32_t sample,int32_t channel,int32_t echopath);
	void (*closeAec)(void* context);

	void (*echoCapture)(void* context,short *rec, short *out);
	void (*preprocessRun)(void* context,short *pcm);
	void (*echoStateReset)(void* context);
	void (*echoPlayback)(void* context,short *play);
	void (*echoCancellation)(void* context,const short *rec, const short *play,
			short *out);
}YangAec;

typedef struct {
	void* context;
	void (*init)(void* context,YangStreamConfig* stream,void* user);
	int32_t (*initParam)(void* context,char* url,YangStreamOptType opt);
	void  (*parseHeader)(YangVideoCodec codec,uint8_t *buf, uint8_t *src, int32_t *hLen);
	int32_t (*connectServer)(void* context,int32_t mediaServer);
	int32_t (*disconnectServer)(void* context);
	void (*setExtradata)(void* context,YangVideoCodec codec,uint8_t *extradata,int32_t extradata_size);
	int32_t (*publishAudio)(void* context,YangFrame* audioFrame);
	int32_t (*publishVideo)(void* context,YangFrame* videoFrame);
	int32_t (*getState)(void* context);
	int32_t (*recvvideoNotify)(void* context, YangRtcMessageType mess);
}YangMetaConnection;
void yang_create_metaConnection(YangMetaConnection* metaconn);
void yang_destroy_metaConnection(YangMetaConnection* metaconn);
void yang_create_aec(YangAec* aec);
void yang_destroy_aec(YangAec* aec);
#endif /* INCLUDE_YANGRTC_YANGMETACONNECTION_H_ */
