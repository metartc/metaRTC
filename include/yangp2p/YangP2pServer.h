//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef INCLUDE_YANGP2P_YANGP2PSERVER_H_
#define INCLUDE_YANGP2P_YANGP2PSERVER_H_
#include <yangp2p/YangP2pServerH.h>
#ifdef __cplusplus
extern "C"{
#endif
int32_t yang_create_p2pserver(YangP2pServer* p2p,int32_t listenPort);
void yang_destroy_p2pserver(YangP2pServer* p2p);
void yang_start_p2pserver(YangP2pServer* p2p);
void yang_stop_p2pserver(YangP2pServer* p2p);
int32_t yang_p2pserver_sendData(YangP2pServer* p2p,char* p,int32_t nb);
#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_YANGP2P_YANGP2PSERVER_H_ */
