//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef INCLUDE_YANGSTREAM_YANGSTREAMMANAGER1_H_
#define INCLUDE_YANGSTREAM_YANGSTREAMMANAGER1_H_
#include <yangstream/YangStreamType.h>
#include <yangstream/YangSynBuffer.h>
#include <yangstream/YangSynBufferManager.h>
#include <vector>

class YangStreamStateNotify{
public:
	YangStreamStateNotify(){};
	virtual ~YangStreamStateNotify(){};
    virtual void streamStateNotify(int32_t puid,YangRtcDirection opt,bool isConnect)=0;
};

class YangStreamManager {
public:
	YangStreamManager(YangSynBufferManager* synMgr);
	virtual ~YangStreamManager();

public:
	YangStreamStateNotify* m_streamState;


	int32_t getPlayOnlineCount();
	int32_t getPushOnlineCount();
	void connectNotify(int32_t puid,YangRtcDirection opt,bool isConnect);

public:
	void setMediaConfig(int32_t puid,YangAudioParam* audio,YangVideoParam* video);
	void sendRequest(int32_t puid,uint32_t ssrc,YangRequestType req);

	void setSendRequestCallback(YangSendRequestCallback* request);
	void setDecoderMediaConfigCallback(YangMediaConfigCallback* dec);
	void setRenderMediaConfigCallback(YangMediaConfigCallback* render);

	int32_t getAudioClock();
	int32_t getVideoClock();
private:
	YangSendRequestCallback* m_sendRequestCb;
	YangMediaConfigCallback* m_mediaConfig_dec;
	YangMediaConfigCallback* m_mediaConfig_render;
	YangSynBufferManager* m_synMgr;
	int32_t m_videoClock;
	int32_t m_audioClock;
private:

	std::vector<int32_t> m_onlinePlayUsers;
	std::vector<int32_t> m_onlinePushUsers;
};

#endif /* INCLUDE_YANGSTREAM_YANGSTREAMMANAGER_H_ */
