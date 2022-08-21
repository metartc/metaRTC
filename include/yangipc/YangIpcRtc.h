//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGMEETING_INCLUDE_YangP2pRtc_H_
#define SRC_YANGMEETING_INCLUDE_YangP2pRtc_H_

#include <yangrtc/YangPeerConnection.h>
#include <yangstream/YangStreamCapture.h>

#include <yangutil/buffer2/YangVideoEncoderBuffer2.h>

#include <yangavutil/video/YangNalu.h>

#include <yangutil/sys/YangVector.h>

yang_vector_declare2(YangPeerConnection)

yang_vector_declare(yangint32)


typedef struct{
	YangAVInfo* avinfo;
	YangRtcCallback rtcCallback;

	YangPeerConnectionVector2 pushs;
	yangint32Vector removeList;

	int32_t isStart;
	int32_t isConvert;
	int32_t isInit;
	uint32_t uidSeq;
	int32_t clientUid;
	uint32_t playCount;
	pthread_t threadId;

	pthread_mutex_t mutex;
	YangH264NaluData nalu;

	YangVideoEncoderBuffer2 *in_videoBuffer;

}YangIpcRtcSession;

typedef struct{
	YangIpcRtcSession session;
	void (*start)(YangIpcRtcSession* session);
	void (*stop)(YangIpcRtcSession* session);
	int32_t (*addPeer)(YangIpcRtcSession* session,char* sdp,char* answer,char* remoteIp,int32_t localPort,int* phasplay);
	int32_t (*removePeer)(YangIpcRtcSession* session,int32_t uid);
	void (*checkAlive)(YangIpcRtcSession* session);
    int32_t (*publishMsg)(YangIpcRtcSession* session,YangFrame* msgFrame);
}YangIpcRtc;

void yang_create_p2prtc(YangIpcRtc* rtc,YangAVInfo* avinfo);
void yang_destroy_p2prtc(YangIpcRtc* rtc);


#endif /* SRC_YANGMEETING_INCLUDE_YangP2pRtc_H_ */
