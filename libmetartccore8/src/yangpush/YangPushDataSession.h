//
// Copyright (c) 2019-2026 yanggaofeng
//

#ifndef SRC_YANGPUSH_YANGPUSHDATASESSION_H_
#define SRC_YANGPUSH_YANGPUSHDATASESSION_H_

#include <yangrtc/YangPushData.h>
#include <yangutil/buffer2/YangPacket.h>

#include <yangvideo/YangNalu.h>
#include <yangvideo/YangMeta.h>

#include <yangutil/yangrtptype.h>

typedef struct{
	uint8_t* buffer;
	YangPacketBuffer* pushDataBuffer;
}YangPushDataSession;

typedef struct{
	void* session;
	void (*reset)(void* session);
	int32_t (*on_video)(void* session,YangFrame* videoFrame);
	int32_t (*on_meta)(void* session,YangH2645Conf *conf,uint64_t timestamp);
}YangPushDataVideo;

typedef struct{
	void* session;
	void (*reset)(void* session);
	int32_t (*on_audio)(void* session,YangFrame* audioFrame);
}YangPushDataAudio;

typedef struct{
	YangPacketBuffer* pushDataBuffer;
	YangPushData audioData;
	YangPushDataAudio audioPacket;
}YangPushAudioDataSession;

typedef struct{
	YangVideoCodec codec;
	YangH2645Conf *conf;
	YangPacketBuffer* pushDataBuffer;
	YangPushData videoData;
	YangPushDataVideo videoPacket;
}YangPushVideoDataSession;

#ifdef __cplusplus
extern "C" {
#endif

int32_t yang_create_pushDataAudio(YangPushDataAudio* audioData,YangPushDataSession *pushData);
void yang_destroy_pushDataAudio(YangPushDataAudio* audioData);

int32_t yang_create_pushVideoDataH264(YangPushDataVideo* videoData,YangPushDataSession *pushData);
int32_t yang_create_pushVideoDataH265(YangPushDataVideo* videoData,YangPushDataSession *pushData);
void yang_destroy_pushVideoDataH264(YangPushDataVideo *videoData);
void yang_destroy_pushVideoDataH265(YangPushDataVideo *videoData);

int32_t yang_create_pushData(YangPushData* pushData,YangPacketBuffer* pushDataBuffer);
void yang_destroy_pushData(YangPushData* pushData);

YangPushData* yang_pushAudio_getData(void* audioSession,YangFrame* audioFrame);
YangPushData* yang_pushVideo_getData(void* videoSession,YangFrame* videoFrame);

#ifdef __cplusplus
}
#endif

#endif /* SRC_YANGPUSH_YANGPUSHDATASESSION_H_ */
