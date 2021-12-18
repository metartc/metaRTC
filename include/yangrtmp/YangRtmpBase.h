/*
 * YangLibRtmp.h
 *
 *  Created on: 2019年9月2日
 *      Author: yang
 */

#ifndef YANGRTMP_IMPL_YANGRTMPBASE_H_
#define YANGRTMP_IMPL_YANGRTMPBASE_H_
#include <errno.h>
#include <vector>

#include "yangutil/sys/YangAmf.h"
#include "yangutil/sys/YangLog.h"
#include <yangstream/YangStreamType.h>
#include <yangutil/yangavinfotype.h>
using namespace std;


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
#define RTMP_PACKET_TYPE_AUDIO              0x08
#define RTMP_PACKET_TYPE_VIDEO              0x09
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
#define RTMP_PACKET_SIZE_LARGE    0
#define RTMP_PACKET_SIZE_MEDIUM   1
#define RTMP_PACKET_SIZE_SMALL    2
#define RTMP_PACKET_SIZE_MINIMUM  3
/* needs to fit largest number of bytes recv() may return */
#define RTMP_BUFFER_CACHE_SIZE (16*1024)
#define RTMP_MAX_HEADER_SIZE 18
#define RTMP_MAX_BODY_SIZE 512000
#define RTMP_SIG_SIZE 1536
#define RTMP_LARGE_HEADER_SIZE 12

#define RTMPPacket_IsReady(a)	((a)->m_nBytesRead == (a)->m_nBodySize)

#define GetSockError()	errno
#define SetSockError(e)	errno = e
#undef closesocket
#ifdef _WIN32

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


enum {
	OPT_STR = 0, OPT_INT, OPT_BOOL, OPT_CONN
};
struct RTMPChunk {
	int32_t c_headerSize;
	int32_t c_chunkSize;
	char *c_chunk;
	char c_header[RTMP_MAX_HEADER_SIZE];
};

struct RTMPPacket {
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
};

struct RTMPSockBuf {
	int32_t sb_socket;
	int32_t sb_size; /* number of unprocessed bytes in buffer */
	char *sb_start; /* pointer into sb_pBuffer of next byte to process */
	char sb_buf[RTMP_BUFFER_CACHE_SIZE]; /* data read from socket */
	int32_t sb_timedout;
	void *sb_ssl;
};

struct RTMP_LNK {
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
};
struct RTMP_READ {
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
};

struct RTMP_METHOD {
	AVal name;
	int32_t num;
};

struct YangNetStatus{
	char code[64];
	char description[64];
	char fmsVer[64];
};

struct RTMP {
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
};

class YangRtmpBase {
public:
	YangRtmpBase();
	virtual ~YangRtmpBase();
	//YangLog *m_log;
	RTMP *r;
	int32_t m_errState;
	//virtual void netStatusHandler(char *code);
	void setPublishState(int32_t p_pubstate);
	int32_t connectServer();
	int32_t HandleStream(char *psName,YangStreamOptType isPublish,int32_t puid);
	int32_t connectRtmpServer(YangStreamOptType pisPublish,char *serverIp,char *app,int32_t port);
	void initRTMP(char *serverIp,char *app,int32_t port);
	void initRTMP(char *serverIp,char *app,int32_t meetingId,int32_t port);
	int32_t initConnect(RTMPPacket *cp);
	void setBufferMS(int32_t size);
	int32_t readPacket(RTMPPacket *packet);
	int32_t yangReadPacket(RTMPPacket *packet,char* pszbody);
	int32_t sendPacket(RTMPPacket *packet, int32_t queue);
	int32_t yangSendPacket(RTMPPacket *packet, int32_t queue);
	int32_t clientPacket(RTMPPacket *packet);
	int32_t clientPacket_conn(RTMPPacket *packet);
	void freePacket(RTMPPacket *packet);
	int32_t RTMP_SetupURL( char *url);
	int32_t RTMP_SetOpt(const AVal *opt, AVal *arg);
	int32_t RTMP_Connect0(struct sockaddr *svc);
	int32_t RTMP_Connect1(RTMPPacket *cp);
	int32_t RTMP_Serve();
	int32_t RTMP_SendChunk(RTMPChunk *chunk);
	int32_t RTMP_Socket();
	int32_t RTMP_ToggleStream();
	void RTMP_DeleteStream();
	void RTMP_Init();
	void RTMP_Close();
	void deleteR();
	void RTMP_Free();
	int32_t RTMP_LibVersion(void);
	void RTMP_UserInterrupt(void);
	int32_t RTMP_SendCtrl(short nType, uint32_t  nObject, uint32_t  nTime);
	int32_t RTMP_SendCtrlPong(short nType, char *p,int32_t len);
	int32_t RTMP_SendPause(int32_t DoPause, int32_t dTime);
	int32_t RTMP_Pause(int32_t DoPause);
	int32_t RTMP_SendCreateStream();
	int32_t RTMP_SendSeek(int32_t dTime);
	int32_t RTMP_SendServerBW();
	int32_t RTMP_SendClientBW();
	void RTMP_DropRequest(int32_t i, int32_t freeit);
	int32_t RTMP_Read(char *buf, int32_t size);
	int32_t RTMP_Write(const char *buf, int32_t size);
	int32_t RTMP_HashSWF(const char *url, uint32_t  *size, uint8_t *hash,int32_t age);
	int32_t SendFCUnpublish();
	int32_t SendDeleteStream(double dStreamId);
	int32_t SendCloseStream(double dStreamId);
	int32_t RTMP_ctrlC;
	char pathChar[30];
	//int32_t m_streamState;
	//int32_t m_isHandleStream;
protected:
	void yang_handleconn(AMFObject *pob);
	uint32_t RTMP_GetTime();
	void RTMP_OptUsage();
	//void RTMP_UserInterrupt();
	//void RTMPPacket_Reset(RTMPPacket *p);
	int32_t RTMPSockBuf_Close(RTMPSockBuf *sb);
	int32_t RTMPSockBuf_Send(RTMPSockBuf *sb, const char *buf, int32_t len) ;
	void HandleClientBW(const RTMPPacket *packet);
	void HandleServerBW(const RTMPPacket *packet);
	void HandleChangeChunkSize(RTMP *r, const RTMPPacket *packet);
	int32_t HandleMetadata(char *body, uint32_t  len);
	int32_t DumpMetaData(AMFObject *obj);
	void RTMPPacket_Dump(RTMPPacket *p);
	void RTMPPacket_Reset(RTMPPacket *p);
	void SocksSetup(AVal *sockshost);
	int32_t RTMPPacket_Alloc(RTMPPacket *p, uint32_t nSize);
	void RTMPPacket_Free(RTMPPacket *p);
	int32_t RTMP_IsConnected();
	void CloseInternal(int32_t reconnect);
	int32_t add_addr_info(struct sockaddr_in *service, AVal *host, int32_t port);
	void handleError(int32_t perrCode);
	int32_t SocksNegotiate();
	int32_t ReadN(char *buffer, int32_t n);
	int32_t WriteN(const char *buffer, int32_t n);
	int32_t SendConnectPacket(RTMPPacket *cp);
	int32_t SendFCSubscribe(AVal *subscribepath);
	int32_t SendUsherToken(AVal *usherToken);
	// void CloseInternal( int32_t reconnect);
	int32_t yang_ParseURL( char *url, int32_t *protocol, AVal *host,
			uint32_t  *port, AVal *app);
	void RTMP_SetupStream(int32_t protocol, AVal *host, uint32_t  port,
			AVal *sockshost, AVal *playpath, AVal *tcUrl, AVal *swfUrl,
			AVal *pageUrl, AVal *app, AVal *auth, AVal *swfSHA256Hash,
			uint32_t swfSize, AVal *flashVer, AVal *subscribepath, AVal *usherToken,
			int32_t dStart, int32_t dStop, int32_t bLiveStream,  int64_t timeout);
	int32_t RTMP_ParseURL(char *url, int32_t *protocol, AVal *host, uint32_t  *port,
		AVal *playpath, AVal *app);
	int32_t RTMP_ParseURL1(char *url, int32_t *protocol, AVal *host, uint32_t  *port,AVal *app);
	int32_t parseAMF(AMFObject *obj, AVal *av, int32_t *depth);
	int32_t yang_SetupURL(char *url,int32_t pport);
	int32_t SendReleaseStream();
	int32_t SendPublish();
	int32_t SendFCPublish();
	int32_t SendPong(double txn);
	int32_t SendCheckBWResult(double txn);
	int32_t SendCheckBW();
	int32_t SendBytesReceived();
	int32_t HandleInvoke(const char *body, uint32_t  nBodySize);
	int32_t SendSecureTokenResponse(AVal *resp);
	int32_t SendPlay();
	int32_t SendPlaylist();
	void HandleCtrl(const RTMPPacket *packet);
	int32_t HandShake(int32_t FP9HandShake);
	int32_t SHandShake();
	AVal* getPlayPath();
	//AVal getPlayPath1();
private:
	//void initAVStreams();
	int32_t RTMPSockBuf_Fill( RTMPSockBuf *sb);
private:

};

#endif /* YANGRTMP_IMPL_YANGRTMPBASE_H_ */
