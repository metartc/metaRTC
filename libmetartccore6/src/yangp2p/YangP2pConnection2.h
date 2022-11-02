//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGRTP_YangP2pConnection_H_
#define YANGRTP_YangP2pConnection_H_

#include <yangrtc/YangRtcConnection.h>
int32_t yang_icep2p_connectRtcServer(YangRtcConnection* conn);
int32_t yang_icep2p_startRtc(YangRtcConnection* conn,char* sdp);
int32_t yang_icep2p_getRemoteSdp(YangRtcSession* session,char* p2p);
#endif
