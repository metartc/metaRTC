//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGRTP_YangP2pConnection_H_
#define YANGRTP_YangP2pConnection_H_

#include <yangrtc/YangRtcSessionH.h>
int32_t yang_p2p_connectRtcServer(YangRtcSession* session);
int32_t yang_p2p_startRtc(YangRtcSession* session,char* sdp);
int32_t yang_p2p_getRemoteSdp(YangRtcSession* session,char* p2p);
#endif
