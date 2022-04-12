//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGMEETING_INCLUDE_YangP2pRtc_H_
#define SRC_YANGMEETING_INCLUDE_YangP2pRtc_H_
#include <yangrtc/YangRtcHandle.h>
#include <string>
#include <yangutil/sys/YangThread.h>
#include <yangutil/sys/YangSysMessageI.h>
#include <yangutil/buffer/YangAudioEncoderBuffer.h>
#include <yangutil/buffer/YangVideoEncoderBuffer.h>
#include <yangutil/buffer/YangVideoDecoderBuffer.h>
#include <yangavutil/video/YangNalu.h>
#include <vector>
using namespace std;

class YangPlayBufferRemove{
public:
	YangPlayBufferRemove(){};
	virtual ~YangPlayBufferRemove(){};
	virtual void removePlayBuffer(int32_t puid,int32_t playcount)=0;
};


class YangP2pRtc: public YangThread ,public YangMediaConfigCallback{
public:
	YangP2pRtc(YangContext *pcontext);
	virtual ~YangP2pRtc();

	int32_t init(int32_t nettype, string server, int32_t localPort,int32_t pport,
			string app,string stream);
	int32_t addPeer(char* sdp,char* answer,char* remoteIp,int32_t localPort,int* phasplay);
	int32_t removePeer(int32_t uid);
	int32_t erasePeer(int32_t uid);
	//int32_t connectServer(int32_t puid);
	int32_t connectMediaServer();
	int32_t disConnectMediaServer();
	//int32_t reconnectMediaServer();
	void setInVideoMetaData(YangVideoMeta *pvmd);
	void setInAudioList(YangAudioEncoderBuffer *pbuf);
	void setInVideoList(YangVideoEncoderBuffer *pbuf);
	void setOutAudioList(YangAudioEncoderBuffer *pbuf);
	void setOutVideoList(YangVideoDecoderBuffer *pbuf);
	YangVideoDecoderBuffer* getOutVideoList();
	void receiveAudio(YangFrame* audioFrame);
	void receiveVideo(YangFrame* videoFrame);
	void setMediaConfig(int32_t puid, YangAudioParam *audio,YangVideoParam *video);
	int32_t stopPublishAudioData();
	int32_t stopPublishVideoData();

	int32_t m_netState;
	int32_t isPublished;
	int32_t m_isStart;

	YangPlayBufferRemove* m_playremove;
	void stop();
protected:
	void run();
	void handleError(int32_t perrCode);
	void startLoop();
	void startLoop_h265();
	int32_t publishVideoData(YangStreamCapture* data);
	int32_t publishAudioData(YangStreamCapture* data);
	void removeStream();
	YangContext *m_context;
	YangVideoMeta *m_vmd;
	YangVideoEncoderBuffer *m_in_videoBuffer;
	YangAudioEncoderBuffer *m_in_audioBuffer;
	bool m_streamInit;
	int32_t m_isConvert;
	int32_t m_isInit;
	int32_t m_audioEncoderType;
	int32_t m_playCount;
	std::vector<YangRtcHandle*> m_pushs;
private:
	int32_t m_transType;
	int32_t notifyState;
	uint32_t m_uidSeq;
	int32_t m_clientUid;
	//bool hasRemoveRtc;
	//int32_t m_removedUid;
	pthread_mutex_t m_mutex;
	vector<int> m_removeList;
	//YangStreamConfig m_streamConf;
	YangH264NaluData m_nalu;
	YangReceiveCallback m_recvCallback;
	YangAudioEncoderBuffer *m_out_audioBuffer;
	YangVideoDecoderBuffer *m_out_videoBuffer;
};

#endif /* SRC_YANGMEETING_INCLUDE_YangP2pRtc_H_ */
