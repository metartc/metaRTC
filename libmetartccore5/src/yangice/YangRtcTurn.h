//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef SRC_YANGICE_YANGRTCTURN_H_
#define SRC_YANGICE_YANGRTCTURN_H_
#include <yangice/YangRtcStun.h>
typedef struct{
	YangStunPacket packet;
	int32_t (*request)(YangBuffer* stream,void* pudp,char* username,char* ice_pwd);
	int32_t (*allocate)(YangBuffer* stream,void* pudp,char* username,char* ice_pwd);
	int32_t (*refresh)(YangBuffer* stream,void* pudp,char* username,char* ice_pwd);
	int32_t (*permission)(YangBuffer* stream,void* pudp,char* username,char* ice_pwd);
	int32_t (*sendIndication)(YangBuffer* stream,void* pudp,char* username,char* ice_pwd);
	int32_t (*dataIndication)(YangBuffer* stream,void* pudp,char* username,char* ice_pwd);
	int32_t (*channelRequest)(YangBuffer* stream,void* pudp,char* username,char* ice_pwd);
	int32_t (*channelSend)(YangBuffer* stream,void* pudp,char* username,char* ice_pwd);
	int32_t (*decode)(YangStunPacket* pkt,char* buf, const int32_t nb_buf);

}YangRtcTurn;


void yang_create_turn(YangRtcTurn* turn);
void yang_destroy_turn(YangRtcTurn* turn);


#endif /* SRC_YANGICE_YANGRTCTURN_H_ */
