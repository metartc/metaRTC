//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtp/YangRtcpXr.h>
#include <yangrtp/YangRtpConstant.h>

#include <yangutil/sys/YangLog.h>

void yang_create_rtcpXr(YangRtcpCommon* comm){
	if(comm==NULL) return;
	   comm->header.padding = 0;
	    comm->header.type = YangRtcpType_xr;
	    comm->header.rc = 0;
	    comm->header.version = kRtcpVersion;
	    comm->ssrc = 0;
}
int32_t yang_decode_rtcpXr(YangRtcpCommon* comm,YangBuffer *buffer){
	if(comm==NULL||buffer==NULL) return 1;
	/*
	    @doc: https://tools.ietf.org/html/rfc3611#section-2
	    0                   1                   2                   3
	    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |V=2|P|reserved |   PT=XR=207   |             length            |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                              SSRC                             |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   :                         report blocks                         :
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   */

    int32_t err = Yang_Ok;
    comm->data = buffer->head;
    comm->nb_data = yang_buffer_left(buffer);

    if(Yang_Ok != (err = yang_decode_header_rtcpCommon(comm,buffer))) {
        return yang_error_wrap(err, "decode rtcp xr header");
    }

    int32_t len = (comm->header.length + 1) * 4 - 8;
    yang_buffer_skip(buffer,len);
    return err;
}
int32_t yang_encode_rtcpXr(YangRtcpCommon* comm,YangBuffer *buffer){
	return yang_error_wrap(ERROR_RTC_RTCP, "not support");
}

uint64_t yang_get_rtcpXr_nb_bytes(YangRtcpCommon* comm){
	return kRtcpPacketSize;
}

