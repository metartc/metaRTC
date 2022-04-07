



//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGPUSH_YANGPUSHHANDLEIMPL_H_
#define YANGPUSH_YANGPUSHHANDLEIMPL_H_
#include <yangp2p/YangP2pPublish.h>
#include <yangp2p/YangP2pHandle.h>
#include <yangp2p/YangP2pRtc.h>
#include <yangutil/sys/YangUrl.h>
#include <yangplayer/YangPlayerBase.h>
#include <yangp2p/YangP2pServer.h>
class YangP2pHandleImpl :public YangP2pHandle{
public:
	YangP2pHandleImpl(bool hasAudio,YangContext* pcontext,YangSysMessageI* pmessage);
	virtual ~YangP2pHandleImpl();
	void init();
	void startCapture();
	int32_t connectRtc(char* url);
    int32_t startRtc(char* remoteIp,char* sdp,char* response);
    int32_t initRtc();
    int32_t initRtc2(bool hasPlay);
	YangVideoBuffer* getPreVideoBuffer();
	YangVideoBuffer* getPlayVideoBuffer();
	void disconnect();
	void receiveTcp(char *data, int32_t nb_data);


private:
    void startCamera();

    void stopCamera();

	void stopPublish();

	void switchToCamera(bool pisinit);


private:
	bool m_hasAudio;
	int m_videoState;
	bool m_isInit;
	bool m_isInitRtc;

	YangPlayerDecoder *m_ydb;

	YangP2pPublish* m_cap;
	YangP2pRtc* m_pub;

	YangContext* m_context;
	YangUrlData m_url;
	YangSysMessageI* m_message;
	YangP2pServer* m_p2pServer;


};

#endif /* YANGPUSH_YANGPUSHHANDLEIMPL_H_ */
