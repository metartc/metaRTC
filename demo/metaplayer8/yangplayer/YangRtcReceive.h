//
// Copyright (c) 2019-2026 yanggaofeng
//
#ifndef SRC_YANGPLAYER_SRC_YANGRTCRECEIVE_H_
#define SRC_YANGPLAYER_SRC_YANGRTCRECEIVE_H_

#include <yangutil/buffer/YangAudioEncoderBuffer.h>
#include <yangutil/buffer/YangVideoDecoderBuffer.h>
#include <yangutil/sys/YangSysMessageI.h>
#include <yangutil/sys/YangThread2.h>
#include <yangutil/yangavinfotype.h>

#include <yangrtc/YangPeerConnection.h>
#include <yangvideo/YangNalu.h>

using namespace std;
class YangRtcReceive : public YangThread,public YangMediaConfigCallback{
public:
	YangRtcReceive(YangContext* pcontext,YangSysMessageI* pmessage);
	virtual ~YangRtcReceive();
	void receiveAudio(YangFrame* audioFrame);
	void receiveVideo(YangFrame* videoFrame);

	int32_t publishMsg(YangFrame* msgFrame);
	int32_t receiveMsg(YangFrame* msgFrame);

    int32_t init(int32_t puid,char* url);
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
	yang_thread_mutex_t m_lock;
	yang_thread_cond_t m_cond_mess;
	int32_t m_waitState;
	int32_t m_headLen;
	YangAudioEncoderBuffer *m_out_audioBuffer;
	YangVideoDecoderBuffer *m_out_videoBuffer;
	YangRtcInfo m_rtcinfo;

    char* m_url;

};

#endif /* SRC_YANGPLAYER_SRC_YANGRTCRECEIVE_H_ */
