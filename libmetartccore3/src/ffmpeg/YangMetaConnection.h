//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef INCLUDE_YANGWEBRTC_YANGMETACONNECTION_H_
#define INCLUDE_YANGWEBRTC_YANGMETACONNECTION_H_
#include <stdint.h>
#define yang_free(a) {if( (a)) {free((a)); (a) = NULL;}}
#define YANG_Frametype_Spspps 9
#define YANG_Frametype_I 1
#define YANG_Frametype_P 0
typedef enum  {
	Yang_Stream_Play, Yang_Stream_Publish, Yang_Stream_Both
}YangStreamOptType;
typedef enum YangAudioCodec{
	Yang_AED_AAC,
	Yang_AED_MP3,
	Yang_AED_SPEEX,
	Yang_AED_OPUS
}YangAudioCodec;
typedef enum YangVideoCodec{
	Yang_VED_264,
	Yang_VED_265,
	Yang_VED_AV1,
	Yang_VED_VP9

}YangVideoCodec;

typedef struct {
	enum YangAudioCodec encode;
	int32_t sample;
	int32_t channel;
    int32_t audioClock;
}YangAudioParam;

typedef struct  {
	enum YangVideoCodec encode;
	int32_t videoClock;

}YangVideoParam;
typedef struct  {
	char serverIp[30];
	char localIp[20];
	char app[20];
	char stream[20];
	char url[50];
	int32_t localPort;
	int32_t serverPort;
	int32_t uid;
	YangStreamOptType streamOptType;
}YangStreamConfig;
typedef struct{
	int32_t mediaType;
	int32_t uid;
	int32_t frametype;
	int32_t nb;
	int64_t pts;
	int64_t dts;
	uint8_t* payload;
}YangFrame;

typedef enum YangRequestType {
	Yang_Req_Sendkeyframe, Yang_Req_Connected, Yang_Req_Disconnected
}YangRequestType;

typedef enum YangRtcMessageType{
	YangRTC_Decoder_Input
}YangRtcMessageType;
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
typedef struct{
	void (*receiveAudio)(YangFrame *audioFrame, void *user);
	void (*receiveVideo)(YangFrame *videoFrame, void *user);
	void (*sendRequest)(int32_t puid, uint32_t ssrc, YangRequestType req,void* user);
	void (*setPlayMediaConfig)(	YangAudioParam *remote_audio,YangVideoParam *remote_video,void* user);
} YangMetaRtcCallback;
typedef struct {
	void* context;
	void (*init)(void* context,YangMetaRtcCallback* callback,void* user);
	int32_t (*initParam)(void* context,char* url,YangStreamOptType opt);
	void  (*parseHeader)(YangVideoCodec codec,uint8_t *buf, uint8_t *src, int32_t *hLen);
	int32_t (*connectServer)(void* context);
	int32_t (*disconnectServer)(void* context);
	void (*setExtradata)(void* context,YangVideoCodec codec,uint8_t *extradata,int32_t extradata_size);
	int32_t (*publishAudio)(void* context,YangFrame* audioFrame);
	int32_t (*publishVideo)(void* context,YangFrame* videoFrame);
	int32_t (*getState)(void* context);
	int32_t (*recvvideoNotify)(void* context, YangRtcMessageType mess);
}YangMetaConnection;
void yang_init_metaConnection(YangMetaConnection* metaconn);
void yang_destroy_metaConnection(YangMetaConnection* metaconn);
void yang_init_aec(YangAec* aec);
void yang_destroy_aec(YangAec* aec);
#endif /* INCLUDE_YANGWEBRTC_YANGMETACONNECTION_H_ */
