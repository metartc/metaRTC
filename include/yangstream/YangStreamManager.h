#ifndef INCLUDE_YANGSTREAM_YANGSTREAMMANAGER1_H_
#define INCLUDE_YANGSTREAM_YANGSTREAMMANAGER1_H_
#include <yangstream/YangStreamType.h>
#include <yangstream/YangSynBuffer.h>
#include <vector>
#include <map>

class YangStreamManager {
public:
	YangStreamManager();
	virtual ~YangStreamManager();

public:
	std::vector<YangSynBuffer*>* m_playBuffers;
	YangSynBuffer* m_playBuffer;
	int getIndex(int puid);
	YangSynBuffer* getSynBuffer(int puid);
public:
	void setMediaConfig(int32_t puid,YangAudioParam* audio,YangVideoParam* video);
	void sendRequest(int32_t puid,uint32_t ssrc,YangRequestType req);
	void sendRequest(int32_t puid,YangRtcMessageType msg);
	void setRtcMessageNotify(int puid,YangRtcMessageNotify* rtcmsg);
	void setSendRequestCallback(YangSendRequestCallback* pli);
	void setDecoderMediaConfigCallback(YangMediaConfigCallback* dec);
	void setRenderMediaConfigCallback(YangMediaConfigCallback* render);


	int32_t getAudioClock();
	int32_t getVideoClock();
private:
	YangSendRequestCallback* m_sendPli;
	YangMediaConfigCallback* m_mediaConfig_dec;
	YangMediaConfigCallback* m_mediaConfig_render;


	int32_t m_videoClock;
	int32_t m_audioClock;
private:
	YangRtcMessageNotify* m_rtcMsg;
	map<int32_t,YangRtcMessageNotify*> *m_rtcMsgMap;



};

#endif /* INCLUDE_YANGSTREAM_YANGSTREAMMANAGER_H_ */
