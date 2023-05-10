//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef INCLUDE_YANGRTC_YANGMETACONNECTION_H_
#define INCLUDE_YANGRTC_YANGMETACONNECTION_H_
#include <yangutil/yangavctype.h>
typedef struct{
	void *session;
	void (*init)(void* session,int32_t sample,int32_t channel,int32_t echopath);
	void (*closeAec)(void* session);

	void (*echoCapture)(void* session,short *rec, short *out);
	void (*preprocessRun)(void* session,short *pcm);
	void (*echoStateReset)(void* session);
	void (*echoPlayback)(void* session,short *play);
	void (*echoCancellation)(void* session,const short *rec, const short *play,
			short *out);
}YangAec;

typedef struct {
	void* session;
	void (*init)(void* session,YangStreamConfig* stream,void* user);
	int32_t (*initParam)(void* session,char* url,YangStreamDirection opt);
	void  (*parseHeader)(YangVideoCodec codec,uint8_t *buf, uint8_t *src, int32_t *hLen);
	int32_t (*connectSfuServer)(void* session,int32_t mediaServer);
	int32_t (*close)(void* session);
	void (*setExtradata)(void* session,YangVideoCodec codec,uint8_t *extradata,int32_t extradata_size);
	int32_t (*on_audio)(void* session,YangFrame* audioFrame);
	int32_t (*on_video)(void* session,YangFrame* videoFrame);
	int32_t (*on_message)(void* session,YangFrame* videoFrame);
	int32_t (*isConnected)(void* session);
	int32_t (*recvvideoNotify)(void* session, YangRtcMessageType mess);
}YangMetaConnection;
void yang_create_metaConnection(YangMetaConnection* metaconn);
void yang_destroy_metaConnection(YangMetaConnection* metaconn);
void yang_create_aec(YangAec* aec);
void yang_destroy_aec(YangAec* aec);
#endif /* INCLUDE_YANGRTC_YANGMETACONNECTION_H_ */
