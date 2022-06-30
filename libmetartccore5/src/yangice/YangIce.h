//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGICE_YANGICE_H_
#define SRC_YANGICE_YANGICE_H_
#include <yangice/YangRtcStun.h>
#include <yangrtc/YangRtcUdp.h>
#include <yangutil/yangavtype.h>
#include <yangutil/yangavinfotype.h>
typedef struct{
	YangIceServer server;
	YangRtcStun stun;
	int32_t (*requestStun)(YangIceServer* server,int32_t localport);
}YangIce;

void yang_create_ice(YangIce* ice,YangAVInfo* avinfo);
void yang_destroy_ice(YangIce* ice);

#endif /* SRC_YANGICE_YANGICE_H_ */
