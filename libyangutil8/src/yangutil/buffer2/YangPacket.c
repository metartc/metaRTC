//
// Copyright (c) 2019-2026 yanggaofeng
//

#include <yangutil/buffer2/YangPacket.h>


YangPacket* yang_pkt_getPacket(YangPacketBuffer* dataBuffer){
	YangPacket* data=NULL;
	if(dataBuffer==NULL || dataBuffer->vsize < 1)
		return NULL;

	data = &dataBuffer->packetData[dataBuffer->getIndex++];

	if(dataBuffer->getIndex>=dataBuffer->capacity)
		dataBuffer->getIndex=0;

	dataBuffer->vsize--;
	return data;
}

YangPacket* yang_pkt_getPutPacket(YangPacketBuffer* dataBuffer){
	YangPacket* data=NULL;
	if(dataBuffer==NULL)
		return NULL;

	data = &dataBuffer->packetData[dataBuffer->putIndex++];

	if(dataBuffer->putIndex>=dataBuffer->capacity)
		dataBuffer->putIndex=0;

	dataBuffer->vsize++;
	return data;
}


YangPacket* yang_pkt_getPacketBySeq(YangPacketBuffer* dataSession,uint16_t sid){
	int32_t i;
	if(dataSession==NULL)
		return NULL;

	for(i=0;i<dataSession->capacity;i++){
		if(dataSession->packetData[i].sequence==sid)
			return &dataSession->packetData[i];
	}

	return NULL;
}

static int32_t yang_resetPacketSize(YangPacketBuffer* dataSession,uint32_t capacity){
	int32_t i;
	uint32_t minus=0;
	uint32_t preCapacity=0;
	YangPacket* pkts=NULL;
	uint32_t payloadSize=0;
	if(dataSession==NULL)
		return 1;

	if(capacity<=dataSession->capacity)
		return Yang_Ok;

	payloadSize=dataSession->packetData[0].capacity;
	preCapacity=dataSession->capacity;
	minus=capacity-preCapacity;

	pkts=(YangPacket*)yang_malloc(sizeof(YangPacket)*capacity);

	yang_memcpy((char*)pkts,(char*)dataSession->packetData,preCapacity*sizeof(YangPacket));

	for(i=0;i<minus;i++){
		pkts[preCapacity+i].sequence=0;
		pkts[preCapacity+i].length=0;
		pkts[preCapacity+i].capacity=payloadSize;
		pkts[preCapacity+i].payload=(uint8_t*)yang_malloc(payloadSize);
	}

	dataSession->capacity=capacity;
	yang_free(dataSession->packetData);
	dataSession->packetData=pkts;
	return Yang_Ok;
}

int32_t yang_pkt_checkBuffer(YangPacketBuffer* dataSession,uint32_t capacity){
	if(capacity<=dataSession->capacity)
		return Yang_Ok;

	return yang_resetPacketSize(dataSession,capacity);
}

YangPacketBuffer* yang_create_packetBuffer(int32_t rtpCount,int32_t payloadSize){
	int32_t i;
	YangPacketBuffer* packetBuffer=(YangPacketBuffer*)yang_calloc(sizeof(YangPacketBuffer),1);
	packetBuffer->capacity=rtpCount;
	packetBuffer->putIndex=0;
	packetBuffer->getIndex=0;
	packetBuffer->packetData=(YangPacket*)yang_calloc(sizeof(YangPacket),rtpCount);
	for(i=0;i<rtpCount;i++){
		packetBuffer->packetData[i].sequence=0;
		packetBuffer->packetData[i].length=0;
		packetBuffer->packetData[i].capacity=payloadSize;
		packetBuffer->packetData[i].payload=(uint8_t*)yang_malloc(payloadSize);
	}
	return packetBuffer;
}

void yang_destroy_packetBuffer(YangPacketBuffer* packetBuffer){
	int32_t i;

	if(packetBuffer==NULL)
		return;

	if(packetBuffer->packetData){
		for(i=0;i<packetBuffer->capacity;i++){
			yang_free(packetBuffer->packetData[i].payload);
		}
	}
	yang_free(packetBuffer->packetData);

}

