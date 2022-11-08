//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangsdp/YangAnswerSdp.h>
#include <yangutil/yangtype.h>
#include <yangsdp/YangSdp.h>
char* g_http_response="HTTP/1.1 200 OK\r\nAllow: OPTIONS, GET, HEAD, POST\r\nAccess-Control-Allow-Origin: *\r\nAccess-Control-Allow-Methods: POST\r\nConnection: Close\r\nContent-Length: %d\r\nContent-Type: application/json\r\nServer: metaRTC3\r\n\r\n%s";
const char* g_http_content2="{\"code\":0,\"server\":\"metaRTC\",\"sdp\":\"%s\",\"sessionid\":\"%s\"}";
const char* g_http_content="{\"code\":0,\"id\":\"metaRTC_%d\",\"sdp\":\"%s\",\"type\":\"answer\"}";
int32_t g_yang_sdp_answerId=0;
int32_t yang_sdp_getAnswerSdp(YangRtcSession* session,char* answer){
    char* sdp=NULL;
	char* sdpstr=(char*)calloc(1024*12,1);

    yang_sdp_genLocalSdp(session,session->context.streamConfig->localPort, &sdp,session->context.streamConfig->streamOptType);

	sprintf(sdpstr,g_http_content,g_yang_sdp_answerId++,sdp);
	sprintf(answer,g_http_response,strlen(sdpstr),sdpstr);
	yang_free(sdp);

	yang_free(sdpstr);
	return Yang_Ok;
}

int32_t yang_sdp_getAnswerSdp2(YangRtcSession* session,char* answer){
    char* sdp=NULL;//(char*)calloc(1024*12,1);
	char* sdpstr=(char*)calloc(1024*12,1);
	char* sessionid=(char*)calloc(64,1);
    yang_sdp_genLocalSdp(session,session->context.streamConfig->localPort, &sdp,session->context.streamConfig->streamOptType);
	sprintf(sessionid,"%s:%s",session->local_ufrag,session->remote_ufrag);
	sprintf(sdpstr,g_http_content2,sdp,sessionid);
	sprintf(answer,g_http_response,strlen(sdpstr),sdpstr);
	yang_free(sdp);
	yang_free(sessionid);
	yang_free(sdpstr);
	return Yang_Ok;
}
