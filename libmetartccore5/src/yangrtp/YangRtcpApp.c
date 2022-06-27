//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangutil/sys/YangLog.h>
#include <string.h>
#include <yangrtp/YangRtcpApp.h>
void yang_create_rtcpApp(YangRtcpCommon* comm){
    comm->ssrc = 0;
    comm->header.padding = 0;
    comm->header.type = YangRtcpType_app;
    comm->header.rc = 0;
    comm->header.version = kRtcpVersion;
    if(comm->appName==NULL) comm->appName=(char*)calloc(4,1);
}
void yang_destroy_rtcpApp(YangRtcpCommon* comm){
	if(comm==NULL) return;
	yang_free(comm->appName);
}
int32_t yang_encode_rtcpApp(YangRtcpCommon* comm,YangBuffer* buffer){

	 /*
	    @doc: https://tools.ietf.org/html/rfc3550#section-6.7
	    0                   1                   2                   3
	    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |V=2|P| subtype |   PT=APP=204  |             length            |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                           SSRC/CSRC                           |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                          name (ASCII)                         |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                   application-dependent data                ...
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	    */
	    int32_t err = Yang_Ok;

	    if(!yang_buffer_require(buffer,yang_rtcpapp_nb_bytes(comm))) {
	        return yang_error_wrap(ERROR_RTC_RTCP, "requires %d bytes", yang_rtcpapp_nb_bytes(comm));
	    }

	    if(Yang_Ok != (err = yang_encode_header_rtcpCommon(comm,buffer))) {
	        return yang_error_wrap(err, "encode header");
	    }


	    yang_write_bytes(buffer,(char*)comm->appName, sizeof(comm->appName));
	    yang_write_bytes(buffer,(char*)comm->payload, comm->payloadLen);

	    return Yang_Ok;
}
int32_t yang_decode_rtcpApp(YangRtcpCommon* comm,YangBuffer* buffer){
	/*
	    @doc: https://tools.ietf.org/html/rfc3550#section-6.7
	    0                   1                   2                   3
	    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |V=2|P| subtype |   PT=APP=204  |             length            |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                           SSRC/CSRC                           |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                          name (ASCII)                         |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                   application-dependent data                ...
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	    */
	    int32_t err = Yang_Ok;
	    comm->data = buffer->head;
	    comm->nb_data = yang_buffer_left(buffer);//->left();

	    if(Yang_Ok != (err = yang_decode_header_rtcpCommon(comm,buffer))) {
	        return yang_error_wrap(err, "decode header");
	    }

	    if (comm->header.type != YangRtcpType_app || !yang_buffer_require(buffer,4)) {
	        return yang_error_wrap(ERROR_RTC_RTCP, "not rtcp app");
	    }
	    char appname[128]={0};
	    yang_read_bytes(buffer,(char *)appname, sizeof(appname));
	    if(comm->appName==NULL) comm->appName=(char*)calloc(strlen(appname)+1,1);
	    memcpy(comm->appName,appname,yang_min(strlen(appname),4));


	    comm->payloadLen = (comm->header.length + 1) * 4 - 8 - sizeof(appname);
	    if (comm->payloadLen > 0) {
	        yang_read_bytes(buffer,(char *)comm->payload, comm->payloadLen);
	    }

	    return Yang_Ok;
}



bool yang_rtcpapp_isApp(uint8_t *data, int32_t nb_data)
{
    if (!data || nb_data <12) {
        return false;
    }

    YangRtcpHeader *header = (YangRtcpHeader*)data;
    if (header->version == kRtcpVersion
            && header->type == YangRtcpType_app
            && ntohs(header->length) >= 2) {
        return true;
    }

    return false;
}




int32_t yang_rtcpapp_set_payload(YangRtcpCommon* comm,uint8_t* payload, int32_t len)
{
    if(len > (kRtcpPacketSize - 12)) {
        return yang_error_wrap(ERROR_RTC_RTCP, "invalid payload length %d", len);
    }

    comm->payloadLen = (len + 3)/ 4 * 4;;
    memcpy(comm->payload, payload, len);
    if (comm->payloadLen > len) {
        memset(&comm->payload[len], 0, comm->payloadLen - len); //padding
    }
    comm->header.length = comm->payloadLen/4 + 3 - 1;

    return Yang_Ok;
}



uint64_t yang_rtcpapp_nb_bytes(YangRtcpCommon* comm)
{
    return sizeof(YangRtcpHeader) + sizeof(comm->ssrc) + sizeof(comm->appName) + comm->payloadLen;
}


