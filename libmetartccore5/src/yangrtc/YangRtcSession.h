//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGWEBRTC_YANGRTCSESSION_H_
#define YANGWEBRTC_YANGRTCSESSION_H_

#include <yangrtc/YangRtcSessionH.h>
#include <yangrtc/YangRtcStun.h>

typedef struct{
	YangRtcSession* session;
	void (*init)(YangRtcSession* session,YangStreamOptType role);
	void (*disconnect)(YangRtcSession *session);
	int32_t (*publishVideo)(YangRtcSession* session,YangFrame* p);
	int32_t (*publishAudio)(YangRtcSession* session,YangFrame *p);
	int32_t (*publishMsg)(YangRtcSession *session, YangFrame *p);
	int32_t (*notify)(YangRtcSession *session, YangRtcMessageType mess);
	int32_t (*isAlive)(YangRtcSession* session);
	void (*startudp)(YangRtcSession *session);
	void (*receive)(YangRtcSession *session, char *data, int32_t size);
}YangRtcConnection;

#ifdef __cplusplus
extern "C"{
#endif

void yang_create_rtcConnection(YangRtcConnection* conn,YangAVInfo* avinfo);
void yang_destroy_rtcConnection(YangRtcConnection* conn);


#ifdef __cplusplus
}
#endif
#endif /* YANGWEBRTC_YANGRTCSESSION_H_ */
