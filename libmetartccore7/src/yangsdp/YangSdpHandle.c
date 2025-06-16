//
// Copyright (c) 2019-2025 yanggaofeng
//
#include <yangsdp/YangSdpHandle.h>
#include <yangsdp/YangRtcSdp.h>
#include <yangutil/sys/YangLog.h>

void yang_parse_sdp(char* str){
	char* tmp;
	YangBuffer buf;
	YangSdp sdp;

	yang_memset(&sdp,0,sizeof(YangSdp));
	yang_create_rtcsdp(&sdp);
	yang_rtcsdp_parse(&sdp,str);
	tmp=(char*)yang_calloc(1024*12,1);

	yang_init_buffer(&buf,tmp,1024*12);
	yang_rtcsdp_encode(&sdp,&buf);

	yang_destroy_rtcsdp(&sdp);
}
