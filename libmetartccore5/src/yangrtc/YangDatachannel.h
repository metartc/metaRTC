//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef SRC_YANGRTC_YANGDATACHANNEL_H_
#define SRC_YANGRTC_YANGDATACHANNEL_H_
#include <yangutil/yangtype.h>
#include <yangrtc/YangRtcContextH.h>
#if Yang_Enable_Datachannel
typedef struct{
	void *context;
	void (*on_message)(void *context,char* data,int32_t nb);
	void (*send_message)(void *context,YangFrame* msgFrame);
}YangDatachannel;

void yang_create_datachannel(YangDatachannel *datachannel,YangRtcContext* ctx);
void yang_destroy_datachannel(YangDatachannel *datachannel);
#endif
#endif /* SRC_YANGRTC_YANGDATACHANNEL_H_ */
