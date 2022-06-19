//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtp/YangRtcpCommon.h>
#include <yangrtp/YangRtcpNack.h>
#include <yangrtp/YangRtcpTWCC.h>
#include <yangrtp/YangRtcpRpsi.h>
#include <yangrtp/YangRtcpRR.h>
#include <yangrtp/YangRtcpSR.h>
#include <yangutil/yangtype.h>
#include <yangutil/sys/YangLog.h>
#include <stdio.h>
#include <string.h>



int32_t yang_decode_rtcpCommon(YangRtcpCommon* comm,YangBuffer *buffer){
	if(comm==NULL||buffer==NULL) return 1;
	if (!yang_buffer_require(buffer,sizeof(YangRtcpHeader) + 4)) {
	        return yang_error_wrap(ERROR_RTC_RTCP, "require %d", sizeof(YangRtcpHeader) + 4);
	    }

    yang_read_bytes(buffer,(char*)(&comm->header), sizeof(YangRtcpHeader));
    comm->header.length = ntohs(comm->header.length);

    int32_t payload_len = comm->header.length * 4;
    if (payload_len > yang_buffer_left(buffer)) {
        return yang_error_wrap(ERROR_RTC_RTCP,
                "payload require payload len=%u, buffer left=%u", payload_len, yang_buffer_left(buffer));
    }
    comm->ssrc = yang_read_4bytes(buffer);

	    return Yang_Ok;
}
int32_t yang_encode_rtcpCommon(YangRtcpCommon* comm,YangBuffer *buffer){
	 return yang_error_wrap(ERROR_RTC_RTCP, "not implement");
}


int32_t yang_decode_header_rtcpCommon(YangRtcpCommon* comm,YangBuffer *buffer)
{
	if(comm==NULL||buffer==NULL) return 1;
    if (!yang_buffer_require(buffer,sizeof(YangRtcpHeader) + 4)) {
        return yang_error_wrap(ERROR_RTC_RTCP, "require %d", sizeof(YangRtcpHeader) + 4);
    }

    yang_read_bytes(buffer,(char*)(&comm->header), sizeof(YangRtcpHeader));
    comm->header.length = ntohs(comm->header.length);

    int32_t payload_len = comm->header.length * 4;
    if (payload_len > yang_buffer_left(buffer)) {
        return yang_error_wrap(ERROR_RTC_RTCP,
                "header require payload len=%u, buffer left=%u", payload_len, yang_buffer_left(buffer));
    }
    comm->ssrc = yang_read_4bytes(buffer);

    return Yang_Ok;
}

int32_t yang_encode_header_rtcpCommon(YangRtcpCommon* comm,YangBuffer *buffer)
{
	if(comm==NULL||buffer==NULL) return 1;
    if(! yang_buffer_require(buffer,sizeof(YangRtcpHeader) + 4)) {
        return yang_error_wrap(ERROR_RTC_RTCP, "require %d", sizeof(YangRtcpHeader) + 4);
    }
    comm->header.length = htons(comm->header.length);
    yang_write_bytes(buffer,(char*)(&comm->header), sizeof(YangRtcpHeader));
    yang_write_4bytes(buffer,comm->ssrc);

    return Yang_Ok;
}
int32_t yang_encode_header_rtcpHeader(YangRtcpHeader* header,uint32_t ssrc,YangBuffer *buffer)
{
	if(header==NULL||buffer==NULL) return 1;
    if(! yang_buffer_require(buffer,sizeof(YangRtcpHeader) + 4)) {
        return yang_error_wrap(ERROR_RTC_RTCP, "require %d", sizeof(YangRtcpHeader) + 4);
    }
    header->length = htons(header->length);
    yang_write_bytes(buffer,(char*)(header), sizeof(YangRtcpHeader));
    yang_write_4bytes(buffer,ssrc);
    return Yang_Ok;
}

uint64_t yang_get_rtcpCommon_nb_bytes(YangRtcpCommon* comm)
{
    return sizeof(YangRtcpHeader) + 4 + comm->payloadLen;
}
void yang_create_rtcpCommon(YangRtcpCommon* comm){
	if(comm==NULL) return;

}
void yang_destroy_rtcpCommon(YangRtcpCommon* comm){
	if(comm==NULL) return;
	yang_destroy_rtcpSR(comm);
	yang_destroy_rtcpRR(comm);
	yang_destroy_rtcpNack(comm);
	yang_destroy_rtcpRpsi(comm);
	yang_destroy_rtcptwcc(comm);
	comm->data=NULL;
}

