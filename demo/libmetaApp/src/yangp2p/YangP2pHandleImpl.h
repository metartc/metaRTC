//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGPUSH_YANGPUSHHANDLEIMPL_H_
#define YANGPUSH_YANGPUSHHANDLEIMPL_H_
#include <yangp2p/YangP2pPublish.h>
#include <yangp2p/YangP2pHandle.h>
#include <yangp2p/YangP2pRtc.h>
#include <yangutil/sys/YangUrl.h>
#include <yangp2p/YangP2pServer.h>
#include <yangp2p/YangP2pDecoder.h>
#include <yangp2p/YangP2pFactoryI.h>
class YangP2pHandleImpl :public YangP2pHandle,public YangP2pRtcI{
public:
	YangP2pHandleImpl(bool hasAudio,YangContext* pcontext,YangSysMessageI* pmessage,YangP2pFactoryI* factory);
	virtual ~YangP2pHandleImpl();
	void init();

	int32_t connectRtc(char* url);
    int32_t startRtc(char* remoteIp,char* sdp,char* response);
    int32_t initRtc();
    int32_t initRtc2(bool hasPlay);
	YangVideoBuffer* getPreVideoBuffer();
	vector<YangVideoBuffer*>* getPlayVideoBuffer();
	void disconnect();
	void receiveTcp(char *data, int32_t nb_data);

	void removePlayBuffer(int32_t puid,int32_t playcount);
	void sendKeyframe();



private:
    void startCamera();
    void stopCamera();
	void stopPublish();
	void initPlayList();


private:
	bool m_hasAudio;
	int m_videoState;
	bool m_isInit;
	bool m_isInitRtc;
	YangP2pFactoryI* m_factory;
	YangP2pDecoder *m_decoder;

	YangP2pPublish* m_cap;
	YangP2pRtc* m_pub;

	YangContext* m_context;
	YangUrlData m_url;
	YangSysMessageI* m_message;
	YangP2pServer* m_p2pServer;

	YangVideoDecoderBuffer* m_outVideoBuffer;
	YangAudioEncoderBuffer* m_outAudioBuffer;


};

#endif /* YANGPUSH_YANGPUSHHANDLEIMPL_H_ */
