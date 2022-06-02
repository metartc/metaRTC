//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef INCLUDE_YANGUTIL_YANGAVCTYPE_H_
#define INCLUDE_YANGUTIL_YANGAVCTYPE_H_
#include <stdint.h>
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

typedef enum YangRequestType {
	Yang_Req_Sendkeyframe, Yang_Req_Connected, Yang_Req_Disconnected
}YangRequestType;

typedef enum YangRtcMessageType{
	YangRTC_Decoder_Error
}YangRtcMessageType;

typedef struct{
	int32_t mediaType;
	int32_t uid;
	int32_t frametype;
	int32_t nb;
	int64_t pts;
	int64_t dts;
	uint8_t* payload;
}YangFrame;

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

typedef struct{
 void* context;
 int32_t (*sendRtcMessage)(void* context,int puid,YangRtcMessageType mess);
}YangSendRtcMessage;

typedef struct{
	void* context;
	void (*sslAlert)(void* context,int32_t uid,char* type,char* desc);
}YangSslCallback;

typedef struct {
	 void* context;
	 void (*receiveAudio)(void* context,YangFrame *audioFrame);
	 void (*receiveVideo)(void* context,YangFrame *videoFrame);
	 void (*receiveMsg)(void* context,YangFrame *videoFrame);
}YangReceiveCallback;


typedef struct{
	void* context;
	void (*setMediaConfig)(void* context,int32_t puid,YangAudioParam* audio,YangVideoParam* video);
	void (*sendRequest)(void* context,int32_t puid,uint32_t ssrc,YangRequestType req);
}YangRtcCallback;

typedef struct  {
	int32_t localPort;
	int32_t remotePort;
	int32_t uid;
	int32_t isServer;
	YangStreamOptType streamOptType;
	YangSslCallback sslCallback;
	YangRtcCallback rtcCallback;
	YangReceiveCallback recvCallback;
	char url[160];
	char remoteIp[64];
	char app[32];
	char stream[128];
}YangStreamConfig;

#endif /* INCLUDE_YANGUTIL_YANGAVCTYPE_H_ */
