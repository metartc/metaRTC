//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtc/YangStreamUrl.h>
#include <yangutil/sys/YangCUrl.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangMath.h>
#include <yangutil/yangtype.h>
int32_t yang_stream_parseUrl(char* url,YangStreamConfig* stream,YangAVInfo* avinfo,YangStreamOptType opt){
	int32_t err=Yang_Ok;
	YangUrlData data;
	memset(&data,0,sizeof(YangUrlData));

    if((err=yang_url_parse(url,&data))!=Yang_Ok){
		return yang_error_wrap(err,"stream parse url error");
	}

	stream->localPort = 10000+yang_random()%15000;
	strcpy(stream->remoteIp,data.server);
	stream->remotePort = data.port;
	strcpy(stream->app,data.app);
	strcpy(stream->stream,data.stream);
	stream->uid = 0;
	stream->streamOptType = opt;//Yang_Stream_Play;

	yang_init_avinfo(avinfo);

return err;

}
