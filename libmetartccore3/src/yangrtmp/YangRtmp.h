//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGRTMP_IMPL_YANGRTMPBASE_H_
#define YANGRTMP_IMPL_YANGRTMPBASE_H_
#include <errno.h>


#include <yangutil/sys/YangAmf.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/yangavinfotype.h>
#include <yangstream/YangStreamType.h>

#include <yangrtmp/YangRtmpHandle.h>



#define RTMP_LIB_VERSION	0x020300	/* 2.3 */
#define RTMP_FEATURE_HTTP	0x01
#define RTMP_FEATURE_ENC	0x02
#define RTMP_FEATURE_SSL	0x04
#define RTMP_FEATURE_MFP	0x08	/* not yet supported */
#define RTMP_FEATURE_WRITE	0x10	/* publish, not play */
#define RTMP_FEATURE_HTTP2	0x20	/* server-side rtmpt */
#define RTMP_PROTOCOL_UNDEFINED	-1
#define RTMP_PROTOCOL_RTMP      0
#define RTMP_PROTOCOL_RTMPE     RTMP_FEATURE_ENC
#define RTMP_PROTOCOL_RTMPT     RTMP_FEATURE_HTTP
#define RTMP_PROTOCOL_RTMPS     RTMP_FEATURE_SSL
#define RTMP_PROTOCOL_RTMPTE    (RTMP_FEATURE_HTTP|RTMP_FEATURE_ENC)
#define RTMP_PROTOCOL_RTMPTS    (RTMP_FEATURE_HTTP|RTMP_FEATURE_SSL)
#define RTMP_PROTOCOL_RTMFP     RTMP_FEATURE_MFP
#define RTMP_DEFAULT_CHUNKSIZE	128
#define	RTMP_CHANNELS	65600
/*      RTMP_PACKET_TYPE_...                0x00 */
#define RTMP_PACKET_TYPE_CHUNK_SIZE         0x01
/*      RTMP_PACKET_TYPE_...                0x02 */
#define RTMP_PACKET_TYPE_BYTES_READ_REPORT  0x03
#define RTMP_PACKET_TYPE_CONTROL            0x04
#define RTMP_PACKET_TYPE_SERVER_BW          0x05
#define RTMP_PACKET_TYPE_CLIENT_BW          0x06

/*      RTMP_PACKET_TYPE_...                0x07 */

/*      RTMP_PACKET_TYPE_...                0x0A */
/*      RTMP_PACKET_TYPE_...                0x0B */
/*      RTMP_PACKET_TYPE_...                0x0C */
/*      RTMP_PACKET_TYPE_...                0x0D */
/*      RTMP_PACKET_TYPE_...                0x0E */
#define RTMP_PACKET_TYPE_FLEX_STREAM_SEND   0x0F
#define RTMP_PACKET_TYPE_FLEX_SHARED_OBJECT 0x10
#define RTMP_PACKET_TYPE_FLEX_MESSAGE       0x11
#define RTMP_PACKET_TYPE_INFO               0x12
#define RTMP_PACKET_TYPE_SHARED_OBJECT      0x13
#define RTMP_PACKET_TYPE_INVOKE             0x14
/*      RTMP_PACKET_TYPE_...                0x15 */
#define RTMP_PACKET_TYPE_FLASH_VIDEO        0x16

/* needs to fit largest number of bytes recv() may return */
#define RTMP_BUFFER_CACHE_SIZE (16*1024)

#define RTMP_SIG_SIZE 1536
#define RTMP_LARGE_HEADER_SIZE 12



#define GetSockError()	errno
#define SetSockError(e)	errno = e
#undef closesocket
#ifdef _WIN32
#define off_t size_t
#else
#define closesocket(s)	close(s)
#endif
#define msleep(n)	yang_usleep(n*1000)
#define SET_RCVTIMEO(tv,s)	struct timeval tv = {s,0}
//#endif



#define SAVC(x)	 const AVal av_##x = AVC(#x)
#define HEX2BIN(a)	(((a)&0x40)?((a)&0xf)+9:((a)&0xf))
#define MAX_IGNORED_FRAMES	50
#define HEADERBUF	(128*1024)
#define OFF(x)	offsetof(struct RTMP,x)


typedef enum {
	OPT_STR = 0, OPT_INT, OPT_BOOL, OPT_CONN
}RTMPOpttype;


typedef struct RTMPSockBuf {
	int32_t sb_socket;
	int32_t sb_size; /* number of unprocessed bytes in buffer */
	char *sb_start; /* pointer into sb_pBuffer of next byte to process */
	char sb_buf[RTMP_BUFFER_CACHE_SIZE]; /* data read from socket */
	int32_t sb_timedout;
	void *sb_ssl;
}RTMPSockBuf;

typedef struct RTMP_LNK {
	char pathChar[30];
	AVal hostname;
	AVal sockshost;
	AVal curPath;
	AVal curPatht;
	char streamType;
	AVal tcUrl;
	AVal swfUrl;
	AVal pageUrl;
	AVal app;
	AVal auth;
	AVal flashVer;
	AVal subscribepath;
	AVal usherToken;
	AVal token;
	AVal pubUser;
	AVal pubPasswd;
	AMFObject extras;
	int32_t edepth;

	int32_t seekTime;
	int32_t stopTime;

#define RTMP_LF_AUTH	0x0001	/* using auth param */
//#define RTMP_LF_LIVE	0x0002	/* stream is live */
#define RTMP_LF_SWFV	0x0004	/* do SWF verification */
#define RTMP_LF_PLST	0x0008	/* send playlist before play */
#define RTMP_LF_BUFX	0x0010	/* toggle stream on BufferEmpty msg */
#define RTMP_LF_FTCU	0x0020	/* free tcUrl on close */
#define RTMP_LF_FAPU	0x0040	/* free app on close */
	int32_t lFlags;

	int32_t swfAge;

	int32_t protocol;
	int32_t timeout; /* connection timeout in seconds */

	int32_t pFlags; /* unused, but kept to avoid breaking ABI */

	unsigned short socksport;
	unsigned short port;
}RTMP_LNK;
typedef struct RTMP_READ {
	char *buf;
	char *bufpos;
	uint32_t  buflen;
	uint32_t timestamp;
	uint8_t dataType;
	uint8_t flags;
#define RTMP_READ_HEADER	0x01
#define RTMP_READ_RESUME	0x02
#define RTMP_READ_NO_IGNORE	0x04
#define RTMP_READ_GOTKF		0x08
#define RTMP_READ_GOTFLVK	0x10
#define RTMP_READ_SEEKING	0x20
	int8_t status;
#define RTMP_READ_COMPLETE	-3
#define RTMP_READ_ERROR	-2
#define RTMP_READ_EOF	-1
#define RTMP_READ_IGNORE	0

	/* if bResume == TRUE */
	uint8_t initialFrameType;
	uint32_t nResumeTS;
	char *metaHeader;
	char *initialFrame;
	uint32_t nMetaHeaderSize;
	uint32_t nInitialFrameSize;
	uint32_t nIgnoredFrameCounter;
	uint32_t nIgnoredFlvFrameCounter;
}RTMP_READ;

typedef struct RTMP_METHOD {
	AVal name;
	int32_t num;
}RTMP_METHOD;

typedef struct YangNetStatus{
	char code[64];
	char description[64];
	char fmsVer[64];
}YangNetStatus;

typedef struct RTMP {
	int32_t m_inChunkSize;
	int32_t m_outChunkSize;
	int32_t m_nBWCheckCounter;
	int32_t m_nBytesIn;
	int32_t m_nBytesInSent;
	int32_t m_nBufferMS;
	int32_t m_stream_id; /* returned in _result from createStream */
	int32_t m_mediaChannel;
	uint32_t m_mediaStamp;
	uint32_t m_pauseStamp;
	int32_t m_pausing;
	int32_t m_nServerBW;
	int32_t m_nClientBW;
	uint8_t m_nClientBW2;
	uint8_t m_bPlaying;
	uint8_t m_bSendEncoding;
	uint8_t m_bSendCounter;
	uint8_t m_bConnected;

	int32_t m_numInvokes;
	int32_t m_numCalls;
	RTMP_METHOD *m_methodCalls; /* remote method calls queue */

	int32_t m_channelsAllocatedIn;
	int32_t m_channelsAllocatedOut;
	RTMPPacket **m_vecChannelsIn;
	RTMPPacket **m_vecChannelsOut;
	int32_t *m_channelTimestamp; /* abs timestamp of last packet */

	double m_fAudioCodecs; /* audioCodecs for the connect packet */
	double m_fVideoCodecs; /* videoCodecs for the connect packet */
	double m_fEncoding; /* AMF0 or AMF3 */

	double m_fDuration; /* duration of stream in seconds */

	int32_t m_isPublish;

	int32_t m_msgCounter; /* RTMPT stuff */
	int32_t m_polling;
	int32_t m_resplen;
	int32_t m_unackd;
	AVal m_clientID;

	RTMP_READ m_read;
	RTMPPacket m_write;
	RTMPSockBuf m_sb;
	RTMP_LNK Link;

	YangNetStatus netStatus;
}RTMP;


#endif /* YANGRTMP_IMPL_YANGRTMPBASE_H_ */
