//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtc/YangRtcTwcc.h>

#include <yangrtp/YangRtcp.h>
#include <yangrtp/YangRtcpTWCC.h>

#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangTime.h>
#include <yangutil/sys/YangEndian.h>

#include <yangutil/yangavtype.h>
#include <yangutil/buffer/YangBuffer.h>

void yang_insert_twcc_local(YangTwccSession* twcc,uint16_t sn){
	if(twcc==NULL) return;
	int32_t ind=YANG_GET_RECV_BUFFER_INDEX(sn);
	twcc->twccPackets[ind].isUse=1;
	twcc->twccPackets[ind].sn=sn;
	twcc->twccPackets[ind].localTs=yang_get_micro_time();
	twcc->sn=sn;;
	twcc->packetCount++;
}

void yang_insert_twcc_remote(YangTwccSession* twcc,uint16_t sn,int64_t ts){
	if(twcc==NULL) return;
	int32_t ind=YANG_GET_RECV_BUFFER_INDEX(sn);
	twcc->twccPackets[ind].remoteTs=ts;
	twcc->lastReportedSn=sn;
}

#define YANG_TWCC_IS_2BIT(packetChunk) (((packetChunk) >> 14u) & 1u)
#define YANG_IS_TWCC_RUNLEN(packetChunk)   ((((packetChunk) >> 15u) & 1u) == 0)
#define YANG_TWCC_STATUSVECTOR_SSIZE(packetChunk)   (YANG_TWCC_IS_2BIT(packetChunk) ? 2u : 1u)
#define YANG_TWCC_STATUSVECTOR_SMASK(packetChunk)   (YANG_TWCC_IS_2BIT(packetChunk) ? 2u : 1u)
#define YANG_TWCC_STATUSVECTOR_STATUS(packetChunk, i)                                                                                                     \
    (((packetChunk) >> (14u - (i) *YANG_TWCC_STATUSVECTOR_SSIZE(packetChunk))) & YANG_TWCC_STATUSVECTOR_SMASK(packetChunk))

int32_t yang_twcc_decode(YangTwccSession* twcc,YangRtcpCommon* comm){
	int32_t err=Yang_Ok;
	YangBuffer stream;
	YangBuffer* buffer=&stream;
	yang_init_buffer(buffer,(char*) comm->payload, comm->payloadLen);

	uint32_t mediaSsrc=yang_read_4bytes(buffer);
	(void)mediaSsrc;
	uint16_t baseSn=(uint16_t)yang_read_2bytes(buffer);
	uint16_t pkcount=(uint16_t)yang_read_2bytes(buffer);
	int32_t referenceTime=yang_read_3bytes(buffer);
	int32_t fb_pkt_count=yang_read_1bytes(buffer);
	(void)fb_pkt_count;
	uint8_t* pchunk=(uint8_t*)buffer->head;

	uint16_t remainingPackets = pkcount;

    uint16_t packetSn=baseSn;
    uint32_t packetChunk=0;

    referenceTime=referenceTime*kTwccFbTimeMultiplier;

    while (remainingPackets > 0) {
        packetChunk = yang_read_2bytes(buffer);
        if (YANG_IS_TWCC_RUNLEN(packetChunk)) {
            remainingPackets -= packetChunk & 0x1fffu;
        } else {
            remainingPackets -= yang_min(YANG_TWCC_IS_2BIT(packetChunk) ? 7 : 14, remainingPackets);
        }

    }
    int16_t recvDelta=0;
    packetSn = baseSn;
    remainingPackets = pkcount;
    uint32_t i=0;
    uint8_t statusSymbol=0;

    while (remainingPackets > 0) {
           packetChunk = (uint32_t)yang_get_be16(pchunk);
           pchunk+=kTwccFbChunkBytes;
           statusSymbol = ((packetChunk) >> 13u) & 3u;
           if (YANG_IS_TWCC_RUNLEN(packetChunk)) {
               for (i = 0; i < (packetChunk & 0x1fffu); i++) {
                   recvDelta = INT16_MIN;
                   switch (statusSymbol) {
                       case 1:
                           recvDelta = (int16_t) yang_read_1bytes(buffer);
                           break;
                       case 2:
                           recvDelta = yang_read_2bytes(buffer);
                           break;
                       case 0:
                    	   yang_insert_twcc_remote(twcc,packetSn,-1);
                    	   break;
                   }
                   if (recvDelta != INT16_MIN) {
                       referenceTime += recvDelta*kTwccFbDeltaUnit;
                  	   yang_insert_twcc_remote(twcc,packetSn,referenceTime);
                   }
                   packetSn++;
                   remainingPackets--;
               }
           } else {
               int32_t statuses = yang_min(YANG_TWCC_IS_2BIT(packetChunk) ? 7 : 14, remainingPackets);

               for (i = 0; i < statuses; i++) {
            	   statusSymbol=YANG_TWCC_STATUSVECTOR_STATUS(packetChunk, i);

                   recvDelta = INT16_MIN;
                   switch (statusSymbol) {
                       case 1:
                           recvDelta = (int16_t) yang_read_1bytes(buffer);
                           break;
                       case 2:
                           recvDelta = yang_read_2bytes(buffer);
                           break;
                       case 0:
                    	   yang_insert_twcc_remote(twcc,packetSn,-1);
                           break;
                   }
                   if (recvDelta != INT16_MIN) {
                	   referenceTime += recvDelta*kTwccFbDeltaUnit;
                	   yang_insert_twcc_remote(twcc,packetSn,referenceTime);
                   }
                   packetSn++;
                   remainingPackets--;
               }
           }

       }

	return err;
}

void yang_create_rtctwcc(YangRtcTwcc* twcc){
	twcc->insertLocal=yang_insert_twcc_local;
	twcc->insertRemote=yang_insert_twcc_remote;
	twcc->decode=yang_twcc_decode;
}
void yang_destroy_rtctwcc(YangRtcTwcc* twcc){

}

