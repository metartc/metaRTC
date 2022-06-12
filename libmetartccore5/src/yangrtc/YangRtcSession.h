//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGWEBRTC_YANGRTCSESSION_H_
#define YANGWEBRTC_YANGRTCSESSION_H_

#include <yangrtc/YangRtcSessionH.h>
#include <yangrtc/YangRtcStun.h>
#ifdef __cplusplus
extern "C"{
#endif
int32_t yang_stun_createRequestStunPacket(YangRtcSession* session,char* ice_pwd);
int32_t yang_stun_createResponseStunPacket(YangStunPacket* request,YangRtcSession* session);
void yang_create_rtcsession(YangRtcSession* session,YangAVInfo* avinfo);
void yang_destroy_rtcsession(YangRtcSession* session);

void yang_rtcsession_init(YangRtcSession* session,YangStreamOptType role);

void yang_rtcsession_setStun(YangRtcSession* session,char* data,int32_t nb);
void yang_rtcsession_startudp(YangRtcSession* session);

void yang_rtcsession_disconnect(YangRtcSession* session);
void yang_rtcsession_setSsrc(YangRtcSession* session,uint32_t audioSsrc,uint32_t videoSsrc);
void yang_rtcsession_startStunTimer(YangRtcSession* session);
void yang_rtcsession_startTimers(YangRtcSession* session) ;
void yang_rtcsession_receive(YangRtcSession* session,char *data, int32_t size);
int32_t yang_rtcsession_publishVideo(YangRtcSession* session,YangFrame* p);
int32_t yang_rtcsession_publishAudio(YangRtcSession* session,YangFrame *p);
int32_t yang_rtcsession_publishMsg(YangRtcSession *session, YangFrame *p);

int32_t yang_rtcsession_isAlive(YangRtcSession* session);

int32_t yang_send_avpacket(YangRtcSession* session,YangRtpPacket *pkt,YangBuffer* pbuf);

int32_t yang_recvvideo_notify(YangRtcSession *session, YangRtcMessageType mess);
#ifdef __cplusplus
}
#endif
#endif /* YANGWEBRTC_YANGRTCSESSION_H_ */
