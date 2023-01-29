//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtp/YangRtcpCommon.h>

#include <yangrtp/YangRtcpRR.h>
#include <yangrtp/YangRtcpSR.h>
#include <yangrtp/YangRtcpSli.h>
#include <yangrtp/YangRtcpNack.h>
#include <yangrtp/YangRtcpTWCC.h>
#include <yangrtp/YangRtcpRpsi.h>

#include <yangutil/sys/YangLog.h>

int32_t yang_decode_rtcpCommon(YangRtcpCommon* comm,YangBuffer *buffer){
	if(comm==NULL||buffer==NULL) return 1;
	if (!yang_buffer_require(buffer,Yang_RtcpHeader_Length + 4)) {
	        return yang_error_wrap(ERROR_RTC_RTCP, "require %d", Yang_RtcpHeader_Length + 4);
	    }

    yang_decode_rtcpHeader(&comm->header,buffer->head);
    yang_buffer_skip(buffer,Yang_RtcpHeader_Length);

    int32_t payload_len = comm->header.length * 4;
    if (payload_len > yang_buffer_left(buffer)) {
        return yang_error_wrap(ERROR_RTC_RTCP,
                "payload require payload len=%u, buffer left=%u", payload_len, yang_buffer_left(buffer));
    }
    comm->ssrc = yang_read_4bytes(buffer);
    comm->payloadLen = payload_len-4;
    yang_read_bytes(buffer,(char*)comm->payload,comm->payloadLen);

	return Yang_Ok;
}

int32_t yang_encode_rtcpCommon(YangRtcpCommon* comm,YangBuffer *buffer){
	 return yang_error_wrap(ERROR_RTC_RTCP, "not implement");
}


int32_t yang_decode_header_rtcpCommon(YangRtcpCommon* comm,YangBuffer *buffer)
{
	if(comm==NULL||buffer==NULL) return 1;
    if (!yang_buffer_require(buffer,Yang_RtcpHeader_Length + 4)) {
        return yang_error_wrap(ERROR_RTC_RTCP, "require %d", Yang_RtcpHeader_Length + 4);
    }


    yang_decode_rtcpHeader(&comm->header,buffer->head);
    yang_buffer_skip(buffer,Yang_RtcpHeader_Length);

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
    if(! yang_buffer_require(buffer,Yang_RtcpHeader_Length + 4)) {
        return yang_error_wrap(ERROR_RTC_RTCP, "require %d", Yang_RtcpHeader_Length + 4);
    }

    yang_encode_rtcpHeader(&comm->header,buffer);

    yang_write_4bytes(buffer,comm->ssrc);

    return Yang_Ok;
}
int32_t yang_encode_header_rtcpHeader(YangRtcpHeader* header,uint32_t ssrc,YangBuffer *buffer)
{
	if(header==NULL||buffer==NULL) return 1;
    if(! yang_buffer_require(buffer,Yang_RtcpHeader_Length + 4)) {
        return yang_error_wrap(ERROR_RTC_RTCP, "require %d", Yang_RtcpHeader_Length + 4);
    }

    yang_encode_rtcpHeader(header,buffer);

    yang_write_4bytes(buffer,ssrc);
    return Yang_Ok;
}

uint64_t yang_get_rtcpCommon_nb_bytes(YangRtcpCommon* comm)
{
    return Yang_RtcpHeader_Length + 4 + comm->payloadLen;
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
	yang_destroy_rtcpSli(comm);
	comm->data=NULL;
}

