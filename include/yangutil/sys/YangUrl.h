#ifndef INCLUDE_YANGUTIL_SYS_YANGURL_H_
#define INCLUDE_YANGUTIL_SYS_YANGURL_H_
#include <string>
#include <stdint.h>

#define Yang_Websocket_Ws 0
#define Yang_Websocket_Wss 1
#define Yang_Websocket_Http 2
#define Yang_Websocket_Https 3
struct YangUrlData{
	int32_t netType;
	int32_t port;
	std::string server;
	std::string app;
	std::string stream;
};
//webrtc://host[:port]/app/stream
int32_t yang_srs_url_parse(char* purl,YangUrlData* data);
//http://host:port/path ws://host:port/path wss://host:port/path
int32_t yang_ws_url_parse(char* purl,YangUrlData* data);
#endif /* INCLUDE_YANGUTIL_SYS_YANGURL_H_ */
