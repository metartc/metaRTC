//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangrtp/YangRtcpPli.h>
#include <yangrtp/YangRtpConstant.h>

#include <yangutil/sys/YangLog.h>

void yang_create_rtcpPli(YangRtcpCommon* comm){
	if(comm==NULL) return;
	comm->header.padding = 0;
	comm->header.type = YangRtcpType_psfb;
	comm->header.rc = kPLI;
	comm->header.version = kRtcpVersion;
	comm->ssrc = 0;
}
void yang_destroy_rtcpPli(YangRtcpCommon* comm){

}
int32_t yang_encode_rtcpPli(YangRtcpCommon* comm,YangBuffer* buffer){
	if(comm==NULL) return 1;
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
	    if(!yang_buffer_require(buffer,yang_get_rtcpCommon_nb_bytes(comm))) {
		        return yang_error_wrap(ERROR_RTC_RTCP, "requires %d bytes", yang_get_rtcpCommon_nb_bytes(comm));
		    }

		    comm->header.length = 2;
		    if(Yang_Ok != (err = yang_encode_header_rtcpCommon(comm,buffer))) {
		        return yang_error_wrap(err, "encode header");
		    }

		    yang_write_4bytes(buffer,comm->mediaSsrc);

	    return err;
}
int32_t yang_decode_rtcpPli(YangRtcpCommon* comm,YangBuffer* buffer){
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
	        return yang_error_wrap(err, "decode rtcp pli header");
	    }

	    comm->mediaSsrc = yang_read_4bytes(buffer);
	    return err;
}
uint64_t yang_rtcpPli_nb_bytes(YangRtcpCommon* comm){
	return 12;
}

