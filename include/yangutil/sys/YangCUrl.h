//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef INCLUDE_YANGUTIL_SYS_YANGCURL_H_
#define INCLUDE_YANGUTIL_SYS_YANGCURL_H_

#include <yangutil/yangtype.h>

#define Yang_Websocket_Ws 0
#define Yang_Websocket_Wss 1
#define Yang_Websocket_Http 2
#define Yang_Websocket_Https 3

typedef struct {
	int32_t netType;
	int32_t port;
	char server[30];
	char app[20];
	char stream[Yang_StreamName_Lenght];
}YangUrlData;

//webrtc://host[:port]/app/stream
int32_t yang_url_parse(YangIpFamilyType familyType,char* purl, YangUrlData* data);
//http://host:port/path ws://host:port/path wss://host:port/path
int32_t yang_ws_url_parse(YangIpFamilyType familyType,char* purl, YangUrlData* data);

int32_t yang_http_url_parse(YangIpFamilyType familyType,char* purl, YangUrlData* data);

#endif /* INCLUDE_YANGUTIL_SYS_YANGCURL_H_ */
