//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtp/YangRtcpRpsi.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangutil/sys/YangLog.h>

void yang_create_rtcpRpsi(YangRtcpCommon* comm){
	if(comm==NULL) return;
	if(comm->rpsi==NULL) comm->rpsi=(YangRtcpRpsi*)yang_malloc(sizeof(YangRtcpRpsi));
	 	comm->rpsi->pb = 0;
	    comm->rpsi->payload_type = 0;
	    comm->rpsi->native_rpsi = NULL;
	    comm->rpsi->nb_native_rpsi = 0;

	    comm->header.padding = 0;
	    comm->header.type = YangRtcpType_psfb;
	    comm->header.rc = kRPSI;
	    comm->header.version = kRtcpVersion;
	    comm->ssrc = 0;
}
void yang_destroy_rtcpRpsi(YangRtcpCommon* comm){
	if(comm==NULL) return;
	yang_free(comm->rpsi);
}
int32_t yang_decode_rtcpRpsi(YangRtcpCommon* comm,YangBuffer *buffer){
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


	    @doc: https://tools.ietf.org/html/rfc4585#section-6.3.3
	    0                   1                   2                   3
	    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |      PB       |0| Payload Type|    Native RPSI bit string     |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |   defined per codec          ...                | Padding (0) |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   */

	    int32_t err = Yang_Ok;
	    comm->data = buffer->head;
	    comm->nb_data = yang_buffer_left(buffer);

	    if(Yang_Ok != (err = yang_decode_header_rtcpCommon(comm,buffer))) {
	        return yang_error_wrap(err, "decode header");
	    }

        comm->rpsi->mediaSsrc = yang_read_4bytes(buffer);
	    int32_t len = (comm->header.length + 1) * 4 - 12;
	    yang_buffer_skip(buffer,len);
	    return err;
}
int32_t yang_encode_rtcpRpsi(YangRtcpCommon* comm,YangBuffer *buffer){
	int32_t err = Yang_Ok;

	    return err;
}
uint64_t yang_rtcpRpsi_nb_bytes(){
	return kRtcpPacketSize;
}

