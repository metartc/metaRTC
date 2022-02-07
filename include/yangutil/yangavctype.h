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



#endif /* INCLUDE_YANGUTIL_YANGAVCTYPE_H_ */
