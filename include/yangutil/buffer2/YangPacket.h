//
// Copyright (c) 2019-2026 yanggaofeng
//

#ifndef INCLUDE_YANGUTIL_BUFFER2_YANGPACKET_H_
#define INCLUDE_YANGUTIL_BUFFER2_YANGPACKET_H_
#include <yangutil/yangtype.h>
#include <yangutil/yangavinfo.h>

typedef struct{
	uint16_t putIndex;
	uint16_t getIndex;
	int16_t  vsize;
	uint16_t capacity;
	YangPacket* packetData;
}YangPacketBuffer;

YangPacket* yang_pkt_getPacket(YangPacketBuffer* dataBuffer);
YangPacket* yang_pkt_getPutPacket(YangPacketBuffer* dataBuffer);
YangPacket* yang_pkt_getPacketBySeq(YangPacketBuffer* dataBuffer,uint16_t seq);
int32_t yang_pkt_checkBuffer(YangPacketBuffer* dataBuffer,uint32_t capacity);

YangPacketBuffer* yang_create_packetBuffer(int32_t rtpCount,int32_t payloadSize);
void yang_destroy_packetBuffer(YangPacketBuffer* pushDataBuffer);

#endif /* INCLUDE_YANGUTIL_BUFFER2_YANGPACKET_H_ */
