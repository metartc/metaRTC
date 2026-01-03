//
// Copyright (c) 2019-2026 yanggaofeng
//

#ifndef YANGUTIL_YANGAVINFOTYPE_H_
#define YANGUTIL_YANGAVINFOTYPE_H_
#include <yangutil/yangtype.h>
#include <yangutil/yangavinfo.h>



typedef struct{
	void* context;
	void (*receiveData)(void* context,YangFrame* msgFrame);
}YangChannelDataRecvI;
typedef struct{
	void* context;
	void (*sendData)(void* context,YangFrame* msgFrame);
}YangChannelDataSendI;
#ifdef __cplusplus
#include <yangstream/YangStreamManager.h>
#include <yangstream/YangSynBufferManager.h>
class YangContext {
public:
	YangContext();
	virtual ~YangContext();
	void init(char *filename);
	void init();

	virtual void initExt(void *filename);
	virtual void initExt();
public:

	YangAVInfo avinfo;
	YangRtcCallback rtcCallback;
	YangSendRtcMessage sendRtcMessage;
	YangChannelDataRecvI channeldataRecv;
	YangChannelDataSendI channeldataSend;
#if Yang_OS_ANDROID
	void* nativeWindow;
#endif

#if Yang_Enable_Vr
        char bgFilename[256];
#endif

        YangSynBufferManager synMgr;
        YangStreamManager* streams;
};

extern "C"{
void yang_init_avinfo(YangAVInfo* avinfo);
}
#else
void yang_init_avinfo(YangAVInfo* avinfo);
#endif




#ifdef __cplusplus
extern "C"{
#include <yangutil/yangframebuffer.h>
}
#else
#include <yangutil/yangframebuffer.h>
#endif




#endif /* YANGUTIL_YANGTYPE_H_ */
