//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGWEBRTC_YANGRTCSESSION_H_
#define YANGWEBRTC_YANGRTCSESSION_H_

#include <yangrtc/YangRtcSession.h>
#include <yangice/YangRtcStun.h>

typedef struct{
	YangRtcSession* session;
	int32_t (*createOffer)(YangRtcSession *session, char **psdp);

	int32_t (*createAnswer)(YangRtcSession* session,char* answer);
	int32_t (*createHttpAnswer)(YangRtcSession* session,char* answer);
	int32_t (*setRemoteDescription)(YangRtcSession* session,char* sdp);

	void (*init)(YangRtcSession* session,YangStreamOptType role);
	void (*close)(YangRtcSession *session);

	int32_t (*on_video)(YangRtcSession* session,YangFrame* p);
	int32_t (*on_audio)(YangRtcSession* session,YangFrame *p);
	int32_t (*on_message)(YangRtcSession *session, YangFrame *p);
	int32_t (*notify)(YangRtcSession *session, YangRtcMessageType mess);
	int32_t (*isAlive)(YangRtcSession* session);
	int32_t (*isConnected)(YangRtcSession* session);
	void (*receive)(YangRtcSession *session, char *data, int32_t size);
	void (*updateCandidateAddress)(YangRtcSession *session,char* ip,int32_t port) ;

}YangRtcConnection;

#ifdef __cplusplus
extern "C"{
#endif

void yang_create_rtcConnection(YangRtcConnection* conn,YangStreamConfig* streamconfig,YangAVInfo* avinfo);
void yang_destroy_rtcConnection(YangRtcConnection* conn);


#ifdef __cplusplus
}
#endif
#endif /* YANGWEBRTC_YANGRTCSESSION_H_ */
