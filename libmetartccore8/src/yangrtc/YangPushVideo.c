//
// Copyright (c) 2019-2026 yanggaofeng
//

#include <yangrtc/YangPushVideo.h>
#include <yangrtc/YangRtcConnection.h>

#include <yangrtc/YangPushData.h>

#include <yangrtp/YangRtpHeader.h>
#include <yangrtp/YangRtpPacket.h>


#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangEndian.h>

#include <yangrtc/YangPush.h>


int32_t yang_push_encodeVideo(YangRtcContext *context, uint32_t videoSSrc,YangPushDataSession *dataSession) {
	int32_t err = 0;

	YangPacket *pktData=NULL;
	uint8_t* buffer=dataSession->buffer;

	pktData=yang_pkt_getPacket(dataSession->pushDataBuffer);
	while(pktData){

		yang_memcpy(buffer,pktData->payload,pktData->length);
		yang_push_videoPacket(context,videoSSrc,buffer,pktData->length);

		context->stats.on_pub_videoRtp(&context->stats.sendStats,pktData->length);
		if((err=yang_send_avpacket2(context, buffer, pktData->length))!=Yang_Ok)
			break;

		pktData=yang_pkt_getPacket(dataSession->pushDataBuffer);

	}
	return err;
}
