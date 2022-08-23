//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef INCLUDE_YANGRTMP_YANGRTMPHANDLE_H_
#define INCLUDE_YANGRTMP_YANGRTMPHANDLE_H_
#include <stdint.h>
#include <yangstream/YangStreamType.h>
#define RTMP_PACKET_TYPE_AUDIO              0x08
#define RTMP_PACKET_TYPE_VIDEO              0x09
#define RTMP_PACKET_SIZE_LARGE    0
#define RTMP_PACKET_SIZE_MEDIUM   1
#define RTMP_PACKET_SIZE_SMALL    2
#define RTMP_PACKET_SIZE_MINIMUM  3
#define RTMP_MAX_HEADER_SIZE 18
#define RTMP_MAX_BODY_SIZE 512000
#define RTMPPacket_IsReady(a)	((a)->m_nBytesRead == (a)->m_nBodySize)
typedef struct RTMPChunk {
	int32_t c_headerSize;
	int32_t c_chunkSize;
	char *c_chunk;
	char c_header[18];
}RTMPChunk;

typedef struct RTMPPacket {
	uint8_t m_headerType;
	uint8_t m_packetType;
	uint8_t m_hasAbsTimestamp; /* timestamp absolute or relative? */
	int32_t m_nChannel;
	uint32_t m_nTimeStamp; /* timestamp */
	int32_t m_nInfoField2; /* last 4 bytes in a long header */
	uint32_t m_nBodySize;
	uint32_t m_nBytesRead;
	RTMPChunk *m_chunk;
	char *m_body;
}RTMPPacket;
typedef struct{
	void *context;
	void (*initRTMP)(void *context,char *serverIp, char *app, int32_t port);
	int32_t (*isConnect)(void *context);
	int32_t (*getStreamId)(void* context);
	int32_t (*sendPacket)(void *context,RTMPPacket *packet, int32_t queue);
	int32_t (*yangReadPacket)(void *context,RTMPPacket *packet, char *pszbody) ;

	int32_t (*connectRtmpServer)(void *context,YangStreamOptType pisPublish,char *serverIp,char *app,int32_t port);
	int32_t (*HandleStream)(void *context,char *psName,YangStreamOptType pisPublish,int32_t puid) ;
	int32_t (*clientPacket)(void *context,RTMPPacket *packet);
	void (*RTMP_Close)(void *context);
}YangRtmpHandle;
#ifdef __cplusplus
extern "C"{
#endif

void yang_create_rtmp(YangRtmpHandle* rtmp);
void yang_destroy_rtmp(YangRtmpHandle* rtmp);

#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_YANGRTMP_YANGRTMPHANDLE_H_ */
