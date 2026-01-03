//
// Copyright (c) 2019-2026 yanggaofeng
//
#ifndef YANGRTP_YangWhipConnection_H_
#define YANGRTP_YangWhipConnection_H_

#include <yangutil/yangavinfo.h>


#ifdef __cplusplus
extern "C"{
#endif

int32_t yang_whip_connectWhipWhepServer(YangPeer* peer,char* url);
int32_t yang_whip_connectSfuServer(YangPeer* peer,char* url,int32_t mediaServer);

#ifdef __cplusplus
}
#endif
#endif
