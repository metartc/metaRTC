//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef SRC_YANGIPC_H_
#define SRC_YANGIPC_H_
#include <yangutil/yangavinfotype.h>

#include <yangutil/sys/YangSysMessageI.h>

#include <yangutil/sys/YangSysMessageHandle.h>
#include <yangp2p/YangP2pFactory.h>
class YangIpc :public YangStreamStateNotify,public YangSysMessageI,public YangSysMessageHandleI{
public:
	YangIpc();
	virtual ~YangIpc();
	void init();
	void initPreview();
	void streamStateNotify(int32_t puid,YangStreamOptType opt,bool isConnect);
    void success();
    void failure(int32_t errcode);
    void receiveSysMessage(YangSysMessage *psm, int32_t phandleRet);

    YangContext *m_context;
    bool m_hasAudio;
    YangSysMessageHandle *m_message;
    YangP2pFactory m_p2pfactory;
};

#endif /* SRC_YANGIPC_H_ */
