//
// Copyright (c) 2019-2026 yanggaofeng
//

#include <yangrtc/YangPushAudio.h>

#include <yangrtc/YangPush.h>
#include <yangrtc/YangRtcConnection.h>

#include <yangutil/sys/YangLog.h>
#include <yangrtp/YangRtpHeader.h>
#include <yangrtp/YangRtpPacket.h>
#include <yangutil/yangrtptype.h>

int32_t yang_push_encodeAudio(YangRtcContext* context, uint32_t audioSsrc,YangPushDataSession *dataSession,uint8_t* tmp) {
	int32_t ret=1;
	YangPacket* pktData=yang_pkt_getPacket(dataSession->pushDataBuffer);
	if(pktData==NULL)
		return ret;

	yang_memcpy(tmp,pktData->payload,pktData->length);

	yang_push_audioPacket(context,audioSsrc,tmp);
	ret=yang_send_avpacket2(context, tmp, pktData->length);
	return ret;
}



