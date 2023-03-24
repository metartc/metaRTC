//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtp/YangRtcpPsfbCommon.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangutil/sys/YangLog.h>

void yang_create_rtcpPsfb(YangRtcpCommon* comm){
	if(comm==NULL) return;
	   comm->header.padding = 0;
	    comm->header.type = YangRtcpType_psfb;
	    comm->header.rc = 1;
	    comm->header.version = kRtcpVersion;
}
int32_t yang_decode_rtcpPsfb(YangRtcpCommon* comm,YangBuffer *buffer){
	if(comm==NULL||buffer==NULL) return 1;
	/*
	    @doc: https://tools.ietf.org/html/rfc4585#section-6.1
	        0                   1                   2                   3
	    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |V=2|P|   FMT   |       PT      |          length               |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                  SSRC of packet sender                        |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                  SSRC of media source                         |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   :            Feedback Control Information (FCI)                 :
	   :                                                               :
	   */

	    int32_t err = Yang_Ok;
	    comm->data = buffer->head;
	    comm->nb_data = yang_buffer_left(buffer);

	    if(Yang_Ok != (err = yang_decode_header_rtcpCommon(comm,buffer))) {
	        return yang_error_wrap(err, "decode header");
	    }

	    comm->mediaSsrc = yang_read_4bytes(buffer);
	    int32_t len = (comm->header.length + 1) * 4 - 12;
	    yang_buffer_skip(buffer,len);
	    return err;
}
int32_t yang_encode_rtcpPsfb(YangRtcpCommon* comm,YangBuffer *buffer){
	 return yang_error_wrap(ERROR_RTC_RTCP, "not support");
}
uint64_t yang_rtcpPsfb_nb_bytes(YangRtcpCommon* comm,YangBuffer *buffer){
	 return kRtcpPacketSize;
}

