//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangutil/sys/YangLog.h>
#include <yangrtp/YangRtcpApp.h>
void yang_create_rtcpApp(YangRtcpCommon* comm){
    comm->ssrc = 0;
    comm->header.padding = 0;
    comm->header.type = YangRtcpType_app;
    comm->header.rc = 0;
    comm->header.version = kRtcpVersion;
    if(comm->appName==NULL) comm->appName=(char*)yang_calloc(4,1);
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
	    if(comm->appName==NULL) comm->appName=(char*)yang_calloc(yang_strlen(appname)+1,1);
	    yang_memcpy(comm->appName,appname,yang_min(yang_strlen(appname),4));


	    comm->payloadLen = (comm->header.length + 1) * 4 - 8 - sizeof(appname);
	    if (comm->payloadLen > 0) {
	        yang_read_bytes(buffer,(char *)comm->payload, comm->payloadLen);
	    }

	    return Yang_Ok;
}



yangbool yang_rtcpapp_isApp(uint8_t *data, int32_t nb_data)
{
    if (!data || nb_data <12) {
        return yangfalse;
    }
    YangRtcpHeader header;
    //YangRtcpHeader *header = (YangRtcpHeader*)data;
    if(yang_decode_rtcpHeader(&header,(char*)data)!=Yang_Ok) return yangfalse;
    if (header.version == kRtcpVersion
            && header.type == YangRtcpType_app
            && header.length >= 2) {
        return yangtrue;
    }

    return yangfalse;
}




int32_t yang_rtcpapp_set_payload(YangRtcpCommon* comm,uint8_t* payload, int32_t len)
{
    if(len > (kRtcpPacketSize - 12)) {
        return yang_error_wrap(ERROR_RTC_RTCP, "invalid payload length %d", len);
    }

    comm->payloadLen = (len + 3)/ 4 * 4;;
    yang_memcpy(comm->payload, payload, len);
    if (comm->payloadLen > len) {
        yang_memset(&comm->payload[len], 0, comm->payloadLen - len); //padding
    }
    comm->header.length = comm->payloadLen/4 + 3 - 1;

    return Yang_Ok;
}



uint64_t yang_rtcpapp_nb_bytes(YangRtcpCommon* comm)
{
    return Yang_RtcpHeader_Length + sizeof(comm->ssrc) + sizeof(comm->appName) + comm->payloadLen;
}


