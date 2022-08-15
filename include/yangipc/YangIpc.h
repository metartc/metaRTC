//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef SRC_YANGP2P_YANGIPC_H_
#define SRC_YANGP2P_YANGIPC_H_

#include <yangutil/yangavinfotype.h>
#ifdef __cplusplus
extern "C"{
#endif
#include <yangp2p/YangP2pServer.h>
#include <yangipc/YangIpcPublish.h>
typedef struct{
	int32_t hasAudio;
	int32_t isInitRtc;
	YangIpcPublish* publish;

	YangAVInfo avinfo;
	YangP2pServer p2pServer;

	YangVideoEncoderBuffer2* videoBuffer;
}YangIpcSession;

typedef struct {
	YangIpcSession session;

	void (*init)(YangIpcSession* session);
	void (*start)(YangIpcSession* session);
	void (*checkAlive)(YangIpcSession* session);
}YangIpc;

void yang_create_ipc(YangIpc* ipc);
void yang_destroy_ipc(YangIpc* ipc);


#ifdef __cplusplus
}
#endif
#endif /* SRC_YANGP2P_YANGIPC_H_ */
