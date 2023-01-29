//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtp/YangRtcp.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangEndian.h>

int32_t yang_decode_rtcpHeader(YangRtcpHeader* header,char* buf){
	if(header==NULL||buf==NULL) return 1;
	uint8_t first=(uint8_t)buf[0];
	header->rc=first&0x1F;
	header->padding=first&0x20;
	header->version=(first>>6)&0x3;
	header->type=(uint8_t)buf[1];
	header->length=yang_get_be16((uint8_t*)buf+2);
	return Yang_Ok;
}

int32_t yang_encode_rtcpHeader(YangRtcpHeader* header,YangBuffer* buffer){
	if(header==NULL||buffer==NULL) return 1;
	uint8_t first=header->version<<6|header->padding<<5|header->rc;
	yang_write_1bytes(buffer,first);
	yang_write_1bytes(buffer,header->type);
	yang_write_2bytes(buffer,header->length);
	return Yang_Ok;
}
