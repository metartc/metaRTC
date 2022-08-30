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

typedef enum {
    YANG_CONNECTION_STATE_NONE = 0,
    YANG_CONNECTION_STATE_NEW,
    YANG_CONNECTION_STATE_CONNECTING,
    YANG_CONNECTION_STATE_CONNECTED,
    YANG_CONNECTION_STATE_DISCONNECTED,
    YANG_CONNECTION_STATE_FAILED,
    YANG_CONNECTION_STATE_CLOSED
} YANG_RTC_CONNECTION_STATE;

typedef enum YangAudioCodec{
	Yang_AED_AAC,
	Yang_AED_MP3,
	Yang_AED_SPEEX,
	Yang_AED_OPUS,
	Yang_AED_G711
}YangAudioCodec;

typedef enum YangVideoCodec{
	Yang_VED_264,
	Yang_VED_265,
	Yang_VED_AV1,
	Yang_VED_VP9

}YangVideoCodec;

typedef enum YangRequestType {
	Yang_Req_Sendkeyframe,
	Yang_Req_HighLostPacketRate,
	Yang_Req_LowLostPacketRate,
	Yang_Req_Connected,
	Yang_Req_Disconnected
}YangRequestType;

typedef enum YangRtcMessageType{
	YangRTC_Decoder_Error
}YangRtcMessageType;

typedef enum{
	YangIceHost,
	YangIceStun,
	YangIceTurn
}YangIceCandidateType;

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
    int32_t fec;
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
	int32_t serverPort;
	uint32_t stunIp;
	int32_t stunPort;
	uint32_t relayIp;
	int32_t relayPort;
	char username[64];
	char password[64];
	char serverIp[64];
}YangIceServer;

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
	int32_t (*onIceCandidate)(void* context,YangIceServer* iceServer);
	void (*onIceConnectionStateChange)(void* context, YANG_RTC_CONNECTION_STATE connectionState);
}YangIceCallback;

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
	YangIceCallback iceCallback;

	char url[160];
	char remoteIp[64];
	char app[32];
	char stream[128];
}YangStreamConfig;

#endif /* INCLUDE_YANGUTIL_YANGAVCTYPE_H_ */
