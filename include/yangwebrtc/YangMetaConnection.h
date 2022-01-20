/*
 * YangMetaConnection.h
 *
 *  Created on: 2022年1月16日
 *      Author: yang
 */

#ifndef INCLUDE_YANGWEBRTC_YANGMETACONNECTION_H_
#define INCLUDE_YANGWEBRTC_YANGMETACONNECTION_H_
#include <yangutil/yangavctype.h>
typedef struct{
	void (*receiveAudio)(YangFrame *audioFrame, void *user);
	void (*receiveVideo)(YangFrame *videoFrame, void *user);
	void (*sendRequest)(int32_t puid, uint32_t ssrc, YangRequestType req,void* user);
	void (*setPlayMediaConfig)(	YangAudioParam *remote_audio,YangVideoParam *remote_video,void* user);
} YangMetaRtcCallback;
typedef struct {
	void* context;
	void (*init)(void* context,YangMetaRtcCallback* callback,void* user);
	int32_t (*initParam)(void* context,char* url,YangStreamOptType opt);
	void  (*parseHeader)(YangVideoCodec codec,uint8_t *buf, uint8_t *src, int32_t *hLen);
	int32_t (*connectServer)(void* context);
	int32_t (*disconnectServer)(void* context);
	int32_t (*publishAudio)(void* context,YangFrame* audioFrame);
	int32_t (*publishVideo)(void* context,YangFrame* videoFrame);
	int32_t (*getState)(void* context);
	int32_t (*recvvideo_notify)(void* context, YangRtcMessageType mess);
}YangMetaConnection;
void yang_init_metaConnection(YangMetaConnection* metaconn);
void yang_destroy_metaConnection(YangMetaConnection* metaconn);
#endif /* INCLUDE_YANGWEBRTC_YANGMETACONNECTION_H_ */
