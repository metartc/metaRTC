//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtp/YangRtcpSli.h>
#include <yangrtp/YangRtpConstant.h>

#include <yangutil/sys/YangLog.h>



void yang_create_rtcpSli(YangRtcpCommon* comm,uint32_t sender_ssrc){
	if(comm->sli==NULL) comm->sli=(YangRtcpSli*)yang_calloc(sizeof(YangRtcpSli),1);
	comm->sli->first = 0;
	comm->sli->number = 0;
	comm->sli->picture = 0;

	comm->header.padding = 0;
	comm->header.type = YangRtcpType_psfb;
	comm->header.rc = kSLI;
	comm->header.version = kRtcpVersion;
	comm->ssrc = sender_ssrc;
}
void yang_destroy_rtcpSli(YangRtcpCommon* comm){
	if(comm==NULL) return;
	yang_free(comm->sli);
}
int32_t yang_encode_rtcpSli(YangRtcpCommon* comm,YangBuffer* buffer){
	 int32_t err = Yang_Ok;

	    return err;
}
int32_t yang_decode_rtcpSli(YangRtcpCommon* comm,YangBuffer* buffer){
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


	    @doc: https://tools.ietf.org/html/rfc4585#section-6.3.2
	    0                   1                   2                   3
	    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |            First        |        Number           | PictureID |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   */

	    int32_t err = Yang_Ok;
	    comm->data = buffer->head;
	    comm->nb_data = yang_buffer_left(buffer);

	    if(Yang_Ok != (err = yang_decode_header_rtcpCommon(comm,buffer))) {
	 	        return yang_error_wrap(err, "decode rtcp sli header");
	 	    }

	    comm->mediaSsrc = yang_read_4bytes(buffer);
	    int32_t len = (comm->header.length + 1) * 4 - 12;
	    yang_buffer_skip(buffer,len);
	    return err;
}

