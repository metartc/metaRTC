//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangrtp/YangRtcpBye.h>
#include <yangutil/sys/YangLog.h>

void yang_create_rtcpBye(YangRtcpCommon* comm){
	comm->ssrc = 0;
	comm->header.padding = 0;
	comm->header.type = YangRtcpType_bye;
	comm->header.rc = 0;
	comm->header.version = kRtcpVersion;
}
void yang_destroy_rtcpBye(YangRtcpCommon* comm){

}
int32_t yang_encode_rtcpBye(YangRtcpCommon* comm,YangBuffer* buffer){
	int32_t err = Yang_Ok;

	if(Yang_Ok != (err = yang_encode_header_rtcpCommon(comm,buffer))) {
		return yang_error_wrap(err, "encode header");
	}
	return err;
}
int32_t yang_decode_rtcpBye(YangRtcpCommon* comm,YangBuffer* buffer){

	int32_t err = Yang_Ok;
	comm->data = buffer->head;
	comm->nb_data = yang_buffer_left(buffer);//->left();

	if(Yang_Ok != (err = yang_decode_header_rtcpCommon(comm,buffer))) {
		return yang_error_wrap(err, "decode header");
	}
	return err;
}
