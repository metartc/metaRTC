//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGRTP_YangP2pConnection_H_
#define YANGRTP_YangP2pConnection_H_

#include <yangrtc/YangRtcConnection.h>
int32_t yang_p2p_connectRtcServer(char* ip,int32_t remotePort,char* localSdp,char* remoteSdp);
#endif
