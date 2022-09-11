//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGPLAYER_SRC_YANGRTCRECEIVE_H_
#define SRC_YANGPLAYER_SRC_YANGRTCRECEIVE_H_
#include <yangutil/yangavinfotype.h>
#include <string>
#include <yangutil/sys/YangSysMessageI.h>
#include <yangutil/buffer/YangAudioEncoderBuffer.h>
#include <yangutil/buffer/YangVideoDecoderBuffer.h>
#include <yangavutil/video/YangNalu.h>
#include <yangrtc/YangPeerConnection.h>
#include <yangutil/sys/YangThread.h>



using namespace std;
class YangRtcReceive : public YangThread,public YangMediaConfigCallback{
public:
	YangRtcReceive(YangContext* pcontext,YangSysMessageI* pmessage);
	virtual ~YangRtcReceive();
	void receiveAudio(YangFrame* audioFrame);
	void receiveVideo(YangFrame* videoFrame);

	int32_t publishMsg(YangFrame* msgFrame);
	int32_t receiveMsg(YangFrame* msgFrame);
    int32_t init(int32_t puid,char* localIp, char* server, int32_t pport,char* app,	char* stream);
    void setBuffer(YangAudioEncoderBuffer *al,YangVideoDecoderBuffer *vl);
    void disConnect();
    void play(char* pserverStr,char *streamName);
    void setMediaConfig(int32_t puid, YangAudioParam *audio,YangVideoParam *video);
    YangPeerConnection *m_recv;
  	int32_t m_isReceived; //,isHandled;
	int32_t m_loops; //,isHandleAllInvoke;
	int32_t m_isStart;
	void stop();
protected:
	void run();
	void startLoop();
	YangContext* m_context;
	YangSysMessageI* m_message;



private:
	pthread_mutex_t m_lock;
	pthread_cond_t m_cond_mess;
	//YangStreamConfig m_conf;
	int32_t m_waitState;
	int32_t m_headLen;
	YangAudioEncoderBuffer *m_out_audioBuffer;
	YangVideoDecoderBuffer *m_out_videoBuffer;
	YangRtcInfo m_rtcinfo;
	YangH264NaluData m_nalu;
	//YangReceiveCallback m_recvCallback;

    //uint8_t* m_keyBuf;
};

#endif /* SRC_YANGPLAYER_SRC_YANGRTCRECEIVE_H_ */
