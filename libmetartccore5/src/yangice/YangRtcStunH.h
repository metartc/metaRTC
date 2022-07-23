//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGICE_YANGRTCSTUNH_H_
#define SRC_YANGICE_YANGRTCSTUNH_H_
#include <yangice/YangRtcStun.h>


int32_t yang_stun_encode(YangStunMessageType stunType,YangBuffer* stream,void* pudp,char* username,char* ice_pwd);

#endif /* SRC_YANGICE_YANGRTCSTUNH_H_ */
