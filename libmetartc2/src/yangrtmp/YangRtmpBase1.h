#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "yangutil/sys/YangLog.h"
#include <yangrtmp/YangRtmpBase.h>
#include <yangutil/yangavinfotype.h>

#ifdef _WIN32

#include <winsock2.h>
#include <ws2tcpip.h>
#include <Mmsystem.h>
#ifdef _MSC_VER	/* MSVC */
#define snprintf _snprintf
#define strcasecmp stricmp
#define strncasecmp strnicmp
#define vsnprintf _vsnprintf
#endif

//#define GetSockError()	WSAGetLastError()
//#define SetSockError(e)	WSASetLastError(e)
#define setsockopt(a,b,c,d,e)	(setsockopt)(a,b,c,(const char *)d,(int)e)
//#define EWOULDBLOCK	WSAETIMEDOUT	/* we don't use nonblocking, but we do use timeouts */
#define sleep(n)	Sleep(n*1000)
//#define msleep(n)	Sleep(n)
//#define SET_RCVTIMEO(tv,s)	int32_t tv = s*1000
#else /* !_WIN32 */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/times.h>
#include <netdb.h>
#include <yangutil/yang_unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include "stddef.h"
#define GetSockError()	errno
#define SetSockError(e)	errno = e
#undef closesocket
#define closesocket(s)	close(s)
#define msleep(n)	yang_usleep(n*1000)
#define SET_RCVTIMEO(tv,s)	struct timeval tv = {s,0}
#endif

#undef OSS
#ifdef _WIN32
#define OSS	"WIN"
#elif defined(__sun__)
#define OSS	"SOL"
#elif defined(__APPLE__)
#define OSS	"MAC"
#elif defined(__linux__)
#define OSS	"LNX"
#else
#define OSS	"GNU"
#endif
#define DEF_VERSTR	OSS " 10,0,32,18"

#if 0				/* unused */
SAVC(bgHasStream);

int
SendBGHasStream(RTMP *r, double dId, AVal *playpath)
{
	RTMPPacket packet;
	char pbuf[1024], *pend = pbuf + sizeof(pbuf);
	char *enc;

	packet.m_nChannel = 0x03; /* control channel (invoke) */
	packet.m_headerType = RTMP_PACKET_SIZE_MEDIUM;
	packet.m_packetType = RTMP_PACKET_TYPE_INVOKE;
	packet.m_nTimeStamp = 0;
	packet.m_nInfoField2 = 0;
	packet.m_hasAbsTimestamp = 0;
	packet.m_body = pbuf + RTMP_MAX_HEADER_SIZE;

	enc = packet.m_body;
	enc = AMF_EncodeString(enc, pend, &av_bgHasStream);
	enc = AMF_EncodeNumber(enc, pend, dId);
	*enc++ = AMF_NULL;

	enc = AMF_EncodeString(enc, pend, playpath);
	if (enc == NULL)
	return FALSE;

	packet.m_nBodySize = enc - packet.m_body;

	return RTMP_SendPacket(r, &packet, TRUE);
}
#endif

#ifndef _WIN32
int32_t clk_tck;
#endif

#ifdef _DEBUG
extern FILE *netstackdump;
extern FILE *netstackdump_read;
#endif

// @remark debug info by http://github.com/ossrs/srs

const char DEFAULT_FLASH_VER[] = DEF_VERSTR;
	const AVal RTMP_DefaultFlashVer = { (char *) DEF_VERSTR,	sizeof(DEF_VERSTR) - 1 };


	//const AVal _const_srs_server_ip = AVC("srs_server_ip");
	//	const AVal _const_srs_pid = AVC("srs_pid");
		//const AVal _const_srs_cid = AVC("srs_id");
	SAVC(app);
		SAVC(connect);
		SAVC(flashVer);
		SAVC(swfUrl);
		SAVC(pageUrl);
		SAVC(tcUrl);
		SAVC(fpad);
		SAVC(capabilities);
		SAVC(audioCodecs);
		SAVC(videoCodecs);
		SAVC(videoFunction);
		SAVC(objectEncoding);
		SAVC(secureToken);
		SAVC(secureTokenResponse);
		SAVC(type);
		SAVC(nonprivate);

		SAVC(seek);
		SAVC(createStream);
		SAVC(FCSubscribe);
		SAVC(releaseStream);
		SAVC(FCPublish);
		SAVC(FCUnpublish);
		SAVC(publish);
		SAVC(live);
		SAVC(record);
		SAVC(deleteStream);
		SAVC(closeStream);
		SAVC(pause);
		SAVC(_checkbw);
		SAVC(_result);
		SAVC(ping);
		SAVC(pong);
		SAVC(play);
		SAVC(set_playlist);
		SAVC(0);

		SAVC(onBWDone);
		SAVC(onFCSubscribe);
		SAVC(onFCUnsubscribe);
		SAVC(_onbwcheck);
		SAVC(_onbwdone);
		SAVC(_error);
		SAVC(close);
		SAVC(code);
		SAVC(level);
		SAVC(description);
		SAVC(onStatus);
		SAVC(playlist_ready);
		SAVC(onMetaData);
		SAVC(duration);
		SAVC(video);
		SAVC(audio);
		const AVal av_NetStream_Authenticate_UsherToken =
				AVC("NetStream.Authenticate.UsherToken");
		const AVal av_setDataFrame = AVC("@setDataFrame");
		const AVal av_NetStream_Failed = AVC("NetStream.Failed");
		const AVal av_NetStream_Play_Failed = AVC("NetStream.Play.Failed");
		const AVal av_NetStream_Play_StreamNotFound =
		AVC("NetStream.Play.StreamNotFound");
		const AVal av_NetConnection_Connect_InvalidApp =
		AVC("NetConnection.Connect.InvalidApp");
		const AVal av_NetStream_Play_Start = AVC("NetStream.Play.Start");
		const AVal av_NetStream_Play_Complete = AVC("NetStream.Play.Complete");
		const AVal av_NetStream_Play_Stop = AVC("NetStream.Play.Stop");
		const AVal av_NetStream_Seek_Notify = AVC("NetStream.Seek.Notify");
		const AVal av_NetStream_Pause_Notify = AVC("NetStream.Pause.Notify");
		const AVal av_NetStream_Play_PublishNotify =
		AVC("NetStream.Play.PublishNotify");
		const AVal av_NetStream_Play_UnpublishNotify =
		AVC("NetStream.Play.UnpublishNotify");
		const AVal av_NetStream_Publish_Start = AVC("NetStream.Publish.Start");
		const AVal av_NetStream_Publish_BadName = AVC("NetStream.Publish.BadName");
		const AVal av_NetStream_Unpublish_Success = AVC("NetStream.Unpublish.Success");
		const AVal av_NetConnection_Connect_Rejected =AVC("NetConnection.Connect.Rejected");

		typedef enum {
			RTMPT_OPEN = 0, RTMPT_SEND, RTMPT_IDLE, RTMPT_CLOSE
		} RTMPTCmd;
const int32_t packetSize[] = { 12, 8, 4, 1 };

const char *optinfo[] = { "string", "integer", "boolean", "AMF" };

const char RTMPProtocolStrings[][7] = { "RTMP", "RTMPT", "RTMPE", "RTMPTE",
		"RTMPS", "RTMPTS", "", "", "RTMFP" };

const char RTMPProtocolStringsLower[][7] = { "rtmp", "rtmpt", "rtmpe", "rtmpte",
		"rtmps", "rtmpts", "", "", "rtmfp" };

const char *RTMPT_cmds[] = { "open", "send", "idle", "close" };
const char flvHeader[] = { 'F', 'L', 'V', 0x01, 0x00, /* 0x04 == audio, 0x01 == video */
0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00 };


const AVal truth[] = {
	AVC("1"),
	AVC("on"),
	AVC("yes"),
	AVC("true"), { 0, 0 } };

	struct urlopt {
		AVal name;
		off_t off;
		int32_t otype;
		int32_t omisc;
		char *use;
	} options[] = { { AVC("socks"), OFF(Link.sockshost), OPT_STR, 0,
            (char*)"Use the specified SOCKS proxy" }, { AVC("app"), OFF(Link.app), OPT_STR,
            0,(char*) "Name of target app on server" }, { AVC("tcUrl"), OFF(Link.tcUrl),
            OPT_STR, 0,(char*) "URL to played stream" }, { AVC("pageUrl"), OFF(
            Link.pageUrl), OPT_STR, 0, (char*)"URL of played media's web page" }, {
            AVC("swfUrl"), OFF(Link.swfUrl), OPT_STR, 0, (char*)"URL to player SWF file" },
			{ AVC("flashver"), OFF(Link.flashVer), OPT_STR, 0,
                    (char*)"Flash version string (default " DEF_VERSTR ")" },
			{ AVC("conn"), OFF(Link.extras), OPT_CONN, 0,
                    (char*)"Append arbitrary AMF data to Connect message" }, {
					AVC("playpath"), OFF(Link.curPatht), OPT_STR, 0,
                    (char*)"Path to target media on server" }, { AVC("playlist"), OFF(
					Link.lFlags), OPT_BOOL, RTMP_LF_PLST,
                    (char*)"Set playlist before play command" }, { AVC("live"), OFF(
					Link.lFlags), OPT_BOOL, 0x0002,
                    (char*)"Stream is live, no seeking possible" }, { AVC("subscribe"),
                    OFF(Link.subscribepath), OPT_STR, 0, (char*)"Stream to subscribe to" },
			{ AVC("jtv"), OFF(Link.usherToken), OPT_STR, 0,
                    (char*)"Justin.tv authentication token" }, { AVC("token"), OFF(
                    Link.token), OPT_STR, 0, (char*)"Key for SecureToken response" }, {
					AVC("swfVfy"), OFF(Link.lFlags), OPT_BOOL, RTMP_LF_SWFV,
                    (char*)"Perform SWF Verification" }, { AVC("swfAge"), OFF(Link.swfAge),
                    OPT_INT, 0, (char*)"Number of days to use cached SWF hash" }, {
					AVC("start"), OFF(Link.seekTime), OPT_INT, 0,
                    (char*)"Stream start position in milliseconds" }, { AVC("stop"), OFF(
					Link.stopTime), OPT_INT, 0,
                    (char*)"Stream stop position in milliseconds" }, { AVC("buffer"), OFF(
                    m_nBufferMS), OPT_INT, 0, (char*)"Buffer time in milliseconds" }, {
					AVC("timeout"), OFF(Link.timeout), OPT_INT, 0,
                    (char*)"Session timeout in seconds" }, { AVC("pubUser"), OFF(
                    Link.pubUser), OPT_STR, 0, (char*)"Publisher username" }, {
					AVC("pubPasswd"), OFF(Link.pubPasswd), OPT_STR, 0,
                    (char*)"Publisher password" }, { { NULL, 0 }, 0, 0 } };



int32_t YangRtmpBase::yang_ParseURL( char *url, int32_t *protocol, AVal *host,
		uint32_t  *port, AVal *app) {
	char *p, *end, *col, *ques, *slash;

	yang_debug( "Parsing...");

	*protocol = RTMP_PROTOCOL_RTMP;
	*port = 0;
	//playpath->av_len = 0;
	//playpath->av_val = NULL;
	app->av_len = 0;
	app->av_val = NULL;

	/* Old School Parsing */

	/* look for usual :// pattern */
	p = (char*) strstr(url, "://");
	if (!p) {
		yang_error( "RTMP URL: No :// in url!");
		return FALSE;
	}
	{
		int32_t len = (int) (p - url);

		if (len == 4 && strncasecmp(url, "rtmp", 4) == 0)
			*protocol = RTMP_PROTOCOL_RTMP;
		else if (len == 5 && strncasecmp(url, "rtmpt", 5) == 0)
			*protocol = RTMP_PROTOCOL_RTMPT;
		else if (len == 5 && strncasecmp(url, "rtmps", 5) == 0)
			*protocol = RTMP_PROTOCOL_RTMPS;
		else if (len == 5 && strncasecmp(url, "rtmpe", 5) == 0)
			*protocol = RTMP_PROTOCOL_RTMPE;
		else if (len == 5 && strncasecmp(url, "rtmfp", 5) == 0)
			*protocol = RTMP_PROTOCOL_RTMFP;
		else if (len == 6 && strncasecmp(url, "rtmpte", 6) == 0)
			*protocol = RTMP_PROTOCOL_RTMPTE;
		else if (len == 6 && strncasecmp(url, "rtmpts", 6) == 0)
			*protocol = RTMP_PROTOCOL_RTMPTS;
		else {
			yang_warn( "Unknown protocol!\n");
			goto parsehost;
		}
	}

	yang_debug( "Parsed protocol: %d", *protocol);

	parsehost:
	/* let's get the hostname */
	p += 3;

	/* check for sudden death */
	if (*p == 0) {
		yang_warn( "No hostname in URL!");
		return FALSE;
	}

	end = p + strlen(p);
	col = strchr(p, ':');
	ques = strchr(p, '?');
	slash = strchr(p, '/');

	{
		int32_t hostlen;
		if (slash)
			hostlen = slash - p;
		else
			hostlen = end - p;
		if (col && col - p < hostlen)
			hostlen = col - p;

		if (hostlen < 256) {
			host->av_val = p;
			host->av_len = hostlen;
			yang_debug( "Parsed host    : %.*s", hostlen,
					host->av_val);
		} else {
			yang_warn( "Hostname exceeds 255 characters!");
		}

		p += hostlen;
	}

	/* get the port number if available */
	if (*p == ':') {
		uint32_t  p2;
		p++;
		p2 = atoi(p);
		if (p2 > 65535) {
			yang_warn( "Invalid port number!");
		} else {
			*port = p2;
		}
	}

	if (!slash) {
		yang_warn( "No application or playpath in URL!");
		return TRUE;
	}
	p = slash + 1;

	{
		/* parse application
		 *
		 * rtmp://host[:port]/app[/appinstance][/...]
		 * application = app[/appinstance]
		 */

		char *slash2, *slash3 = NULL, *slash4 = NULL;
		int32_t applen, appnamelen;

		slash2 = strchr(p, '/');
		if (slash2)
			slash3 = strchr(slash2 + 1, '/');
		if (slash3)
			slash4 = strchr(slash3 + 1, '/');

		applen = end - p; /* ondemand, pass all parameters as app */
		appnamelen = applen; /* ondemand length */

		if (ques && strstr(p, "slist=")) { /* whatever it is, the '?' and slist= means we need to use everything as app and parse plapath from slist= */
			appnamelen = ques - p;
		} else if (strncmp(p, "ondemand/", 9) == 0) {
			/* app = ondemand/foobar, only pass app=ondemand */
			applen = 8;
			appnamelen = 8;
		} else { /* app!=ondemand, so app is app[/appinstance] */
			if (slash4)
				appnamelen = slash4 - p;
			else if (slash3)
				appnamelen = slash3 - p;
			else if (slash2)
				appnamelen = slash2 - p;

			applen = appnamelen;
		}

		app->av_val = p;
		app->av_len = applen;
		yang_debug( "Parsed app     : %.*s", applen, p);

		p += appnamelen;
	}

	if (*p == '/')
		p++;

	/**if (end-p) {
	 AVal av = {p, end-p};
	 RTMP_ParsePlaypath(&av, playpath);
	 }**/

	return TRUE;
}
void YangRtmpBase::RTMPPacket_Reset(RTMPPacket *p) {
	p->m_headerType = 0;
	p->m_packetType = 0;
	p->m_nChannel = 0;
	p->m_nTimeStamp = 0;
	p->m_nInfoField2 = 0;
	p->m_hasAbsTimestamp = FALSE;
	p->m_nBodySize = 0;
	p->m_nBytesRead = 0;
}

uint32_t YangRtmpBase::RTMP_GetTime() {
#ifdef _DEBUG
	return 0;
#elif defined(_WIN32)
	return timeGetTime();
#else
	struct tms t;
	if (!clk_tck)
		clk_tck = sysconf(_SC_CLK_TCK);
	return times(&t) * 1000 / clk_tck;
#endif
}

void YangRtmpBase::RTMP_UserInterrupt() {
	RTMP_ctrlC = TRUE;
}

void YangRtmpBase::SocksSetup(AVal *sockshost) {
	if (sockshost->av_len) {
		const char *socksport = strchr(sockshost->av_val, ':');
		char *hostname = strdup(sockshost->av_val);

		if (socksport)
			hostname[socksport - sockshost->av_val] = '\0';
		r->Link.sockshost.av_val = hostname;
		r->Link.sockshost.av_len = strlen(hostname);

		r->Link.socksport = socksport ? atoi(socksport + 1) : 1080;
		yang_debug( "Connecting via SOCKS proxy: %s:%d",
				r->Link.sockshost.av_val, r->Link.socksport);
	} else {
		r->Link.sockshost.av_val = NULL;
		r->Link.sockshost.av_len = 0;
		r->Link.socksport = 0;
	}
}
int32_t YangRtmpBase::RTMPPacket_Alloc(RTMPPacket *p, uint32_t nSize) {
	char *ptr;
	if (nSize > SIZE_MAX - RTMP_MAX_HEADER_SIZE)
		return FALSE;
	ptr = (char*) calloc(1, nSize + RTMP_MAX_HEADER_SIZE);
	if (!ptr)
		return FALSE;
	p->m_body = ptr + RTMP_MAX_HEADER_SIZE;
	p->m_nBytesRead = 0;
	return TRUE;
}

void YangRtmpBase::RTMPPacket_Free(RTMPPacket *p) {
	if (p->m_body) {
		free(p->m_body - RTMP_MAX_HEADER_SIZE);
		p->m_body = NULL;
	}
}

void YangRtmpBase::RTMPPacket_Dump(RTMPPacket *p) {
	yang_debug(
			"RTMP PACKET: packet type: 0x%02x. channel: 0x%02x. info 1: %d info 2: %d. Body size: %u. body: 0x%02x",
			p->m_packetType, p->m_nChannel, p->m_nTimeStamp, p->m_nInfoField2,
			p->m_nBodySize, p->m_body ? (uint8_t) p->m_body[0] : 0);
}

int32_t RTMP_LibVersion() {
	return RTMP_LIB_VERSION;
}

void YangRtmpBase::RTMP_OptUsage() {
	int32_t i;

	yang_error( "Valid RTMP options are:\n");
	for (i = 0; options[i].name.av_len; i++) {
		yang_error( "%10s %-7s  %s\n", options[i].name.av_val,
				optinfo[options[i].otype], options[i].use);
	}
}

int32_t YangRtmpBase::parseAMF(AMFObject *obj, AVal *av, int32_t *depth) {
	AMFObjectProperty prop = { { 0, 0 } };
	int32_t i;
	char *p, *arg = av->av_val;

	if (arg[1] == ':') {
		p = (char *) arg + 2;
		switch (arg[0]) {
		case 'B':
			prop.p_type = AMF_BOOLEAN;
			prop.p_vu.p_number = atoi(p);
			break;
		case 'S':
			prop.p_type = AMF_STRING;
			prop.p_vu.p_aval.av_val = p;
			prop.p_vu.p_aval.av_len = av->av_len - (p - arg);
			break;
		case 'N':
			prop.p_type = AMF_NUMBER;
			prop.p_vu.p_number = strtod(p, NULL);
			break;
		case 'Z':
			prop.p_type = AMF_NULL;
			break;
		case 'O':
			i = atoi(p);
			if (i) {
				prop.p_type = AMF_OBJECT;
			} else {
				(*depth)--;
				return 0;
			}
			break;
		default:
			return -1;
		}
	} else if (arg[2] == ':' && arg[0] == 'N') {
		p = strchr(arg + 3, ':');
		if (!p || !*depth)
			return -1;
		prop.p_name.av_val = (char *) arg + 3;
		prop.p_name.av_len = p - (arg + 3);

		p++;
		switch (arg[1]) {
		case 'B':
			prop.p_type = AMF_BOOLEAN;
			prop.p_vu.p_number = atoi(p);
			break;
		case 'S':
			prop.p_type = AMF_STRING;
			prop.p_vu.p_aval.av_val = p;
			prop.p_vu.p_aval.av_len = av->av_len - (p - arg);
			break;
		case 'N':
			prop.p_type = AMF_NUMBER;
			prop.p_vu.p_number = strtod(p, NULL);
			break;
		case 'O':
			prop.p_type = AMF_OBJECT;
			break;
		default:
			return -1;
		}
	} else
		return -1;

	if (*depth) {
		AMFObject *o2;
		for (i = 0; i < *depth; i++) {
			o2 = &obj->o_props[obj->o_num - 1].p_vu.p_object;
			obj = o2;
		}
	}
	AMF_AddProp(obj, &prop);
	if (prop.p_type == AMF_OBJECT)
		(*depth)++;
	return 0;
}

void AV_erase(RTMP_METHOD *vals, int32_t *num, int32_t i, int32_t freeit) {
	if (freeit)
		free(vals[i].name.av_val);
	(*num)--;
	for (; i < *num; i++) {
		vals[i] = vals[i + 1];
	}
	vals[i].name.av_val = NULL;
	vals[i].name.av_len = 0;
	vals[i].num = 0;
}

void RTMP_DropRequest(RTMP *r, int32_t i, int32_t freeit) {
	AV_erase(r->m_methodCalls, &r->m_numCalls, i, freeit);
}

void AV_queue(RTMP_METHOD **vals, int32_t *num, AVal *av, int32_t txn) {
	char *tmp;
	if (!(*num & 0x0f))
		*vals = (RTMP_METHOD*) realloc(*vals,
				(*num + 16) * sizeof(RTMP_METHOD));
	tmp = (char*) malloc(av->av_len + 1);
	memcpy(tmp, av->av_val, av->av_len);
	tmp[av->av_len] = '\0';
	(*vals)[*num].num = txn;
	(*vals)[*num].name.av_len = av->av_len;
	(*vals)[(*num)++].name.av_val = tmp;
}

void AV_clear(RTMP_METHOD *vals, int32_t num) {
	int32_t i;
	for (i = 0; i < num; i++)
		free(vals[i].name.av_val);
	free(vals);
}

int32_t RTMP_FindFirstMatchingProperty(AMFObject *obj, const AVal *name,
		AMFObjectProperty * p) {
	int32_t n;
	/* this is a small object search to locate the "duration" property */
	for (n = 0; n < obj->o_num; n++) {
		AMFObjectProperty *prop = AMF_GetProp(obj, NULL, n);

		if (AVMATCH(&prop->p_name, name)) {
			memcpy(p, prop, sizeof(*prop));
			return TRUE;
		}

		if (prop->p_type == AMF_OBJECT || prop->p_type == AMF_ECMA_ARRAY) {
			if (RTMP_FindFirstMatchingProperty(&prop->p_vu.p_object, name, p))
				return TRUE;
		}
	}
	return FALSE;
}

/* Like above, but only check if name is a prefix of property */
int32_t RTMP_FindPrefixProperty(AMFObject *obj, const AVal *name,
		AMFObjectProperty * p) {
	int32_t n;
	for (n = 0; n < obj->o_num; n++) {
		AMFObjectProperty *prop = AMF_GetProp(obj, NULL, n);

		if (prop->p_name.av_len > name->av_len
				&& !memcmp(prop->p_name.av_val, name->av_val, name->av_len)) {
			memcpy(p, prop, sizeof(*prop));
			return TRUE;
		}

		if (prop->p_type == AMF_OBJECT) {
			if (RTMP_FindPrefixProperty(&prop->p_vu.p_object, name, p))
				return TRUE;
		}
	}
	return FALSE;
}

int32_t YangRtmpBase::DumpMetaData(AMFObject *obj) {
	AMFObjectProperty *prop;
	int32_t n, len;
	for (n = 0; n < obj->o_num; n++) {
		char str[256] = "";
		prop = AMF_GetProp(obj, NULL, n);
		switch (prop->p_type) {
		case AMF_OBJECT:
		case AMF_ECMA_ARRAY:
		case AMF_STRICT_ARRAY:
			if (prop->p_name.av_len)
				yang_info( "%.*s:", prop->p_name.av_len,
						prop->p_name.av_val);
			DumpMetaData(&prop->p_vu.p_object);
			break;
		case AMF_NUMBER:
			snprintf(str, 255, "%.2f", prop->p_vu.p_number);
			break;
		case AMF_BOOLEAN:
			snprintf(str, 255, "%s",
					prop->p_vu.p_number != 0. ? "TRUE" : "FALSE");
			break;
		case AMF_STRING:
			len = snprintf(str, 255, "%.*s", prop->p_vu.p_aval.av_len,
					prop->p_vu.p_aval.av_val);
			if (len >= 1 && str[len - 1] == '\n')
				str[len - 1] = '\0';
			break;
		case AMF_DATE:
			snprintf(str, 255, "timestamp:%.2f", prop->p_vu.p_number);
			break;
		default:
			snprintf(str, 255, "INVALID TYPE 0x%02x",
					(uint8_t) prop->p_type);
		}
		if (str[0] && prop->p_name.av_len) {
			yang_info( "  %-22.*s%s", prop->p_name.av_len,
					prop->p_name.av_val, str);
		}
	}
	return FALSE;
}

int32_t YangRtmpBase::HandleMetadata(char *body, uint32_t  len) {
	/* allright we get some info here, so parse it and print32_t it */
	/* also keep duration or filesize to make a nice progress bar */

	AMFObject obj;
	AVal metastring;
	int32_t ret = FALSE;

	int32_t nRes = AMF_Decode(&obj, body, len, FALSE);
	if (nRes < 0) {
		yang_error( "%s, error decoding meta data packet",
				__FUNCTION__);
		return FALSE;
	}

	AMF_Dump(&obj);
	AMFProp_GetString(AMF_GetProp(&obj, NULL, 0), &metastring);

	if (AVMATCH(&metastring, &av_onMetaData)) {
		AMFObjectProperty prop;
		/* Show metadata */
		yang_info( "Metadata:");
		DumpMetaData(&obj);
		if (RTMP_FindFirstMatchingProperty(&obj, &av_duration, &prop)) {
			r->m_fDuration = prop.p_vu.p_number;
			/*RTMP_Log(RTMP_LOGDEBUG, "Set duration: %.2f", m_fDuration); */
		}
		/* Search for audio or video tags */
		if (RTMP_FindPrefixProperty(&obj, &av_video, &prop))
			r->m_read.dataType |= 1;
		if (RTMP_FindPrefixProperty(&obj, &av_audio, &prop))
			r->m_read.dataType |= 4;
		ret = TRUE;
	}
	AMF_Reset(&obj);
	return ret;
}

void YangRtmpBase::HandleChangeChunkSize(RTMP *r, const RTMPPacket *packet) {
	if (packet->m_nBodySize >= 4) {
		r->m_inChunkSize = AMF_DecodeInt32(packet->m_body);
		yang_debug( "%s, received: chunk size change to %d",
				__FUNCTION__, r->m_inChunkSize);
	}
}

void HandleAudio(RTMP *r, const RTMPPacket *packet) {
}

void HandleVideo(RTMP *r, const RTMPPacket *packet) {
}

void YangRtmpBase::HandleServerBW(const RTMPPacket *packet) {
	r->m_nServerBW = AMF_DecodeInt32(packet->m_body);
	yang_debug( "%s: server BW = %d", __FUNCTION__, r->m_nServerBW);
}

void YangRtmpBase::HandleClientBW(const RTMPPacket *packet) {
	r->m_nClientBW = AMF_DecodeInt32(packet->m_body);
	if (packet->m_nBodySize > 4)
		r->m_nClientBW2 = packet->m_body[4];
	else
		r->m_nClientBW2 = -1;
	yang_debug( "%s: client BW = %d %d", __FUNCTION__,
			r->m_nClientBW, r->m_nClientBW2);
}

int32_t DecodeInt32LE(const char *data) {
	uint8_t *c = (uint8_t *) data;
	uint32_t  val;

	val = (c[3] << 24) | (c[2] << 16) | (c[1] << 8) | c[0];
	return val;
}

int32_t EncodeInt32LE(char *output, int32_t nVal) {
	output[0] = nVal;
	nVal >>= 8;
	output[1] = nVal;
	nVal >>= 8;
	output[2] = nVal;
	nVal >>= 8;
	output[3] = nVal;
	return 4;
}

int32_t YangRtmpBase::RTMPSockBuf_Fill( RTMPSockBuf *sb) {
	if(!r) return 0;
	int32_t nBytes;

	if (!sb->sb_size)
		sb->sb_start = sb->sb_buf;

	while (1) {
		nBytes = sizeof(sb->sb_buf) - 1 - sb->sb_size- (sb->sb_start - sb->sb_buf);
		{
			nBytes = recv(sb->sb_socket, sb->sb_start + sb->sb_size, nBytes, 0);
		}
		if (nBytes != -1) {
			sb->sb_size += nBytes;
		} else {
			int32_t sockerr = GetSockError();
			yang_error("%s, recv returned %d. GetSockError(): %d (%s)",__FUNCTION__, nBytes, sockerr, strerror(sockerr));
			if (sockerr == EINTR && !RTMP_ctrlC)
				continue;

			if (sockerr == EWOULDBLOCK || sockerr == EAGAIN) {
				sb->sb_timedout = TRUE;
				nBytes = 0;
			}
		}
		break;
	}

	return nBytes;
}
#ifdef _WIN32
#define MSG_NOSIGNAL 0
#endif
int32_t YangRtmpBase::RTMPSockBuf_Send(RTMPSockBuf *sb, const char *buf, int32_t len) {
        return send(sb->sb_socket, buf, len, MSG_NOSIGNAL);
}

int32_t YangRtmpBase::RTMPSockBuf_Close(RTMPSockBuf *sb) {
	if (sb->sb_socket != -1)
		return closesocket(sb->sb_socket);
	return 0;
}

void DecodeTEA(AVal *key, AVal *text) {
	uint32_t *v, k[4] = { 0 }, u;
	uint32_t z, y, sum = 0, e, DELTA = 0x9e3779b9;
	int32_t p, q;
	int32_t i, n;
	uint8_t *ptr, *out;

	/* prep key: pack 1st 16 chars into 4 LittleEndian ints */
	ptr = (uint8_t *) key->av_val;
	u = 0;
	n = 0;
	v = k;
	p = key->av_len > 16 ? 16 : key->av_len;
	for (i = 0; i < p; i++) {
		u |= ptr[i] << (n * 8);
		if (n == 3) {
			*v++ = u;
			u = 0;
			n = 0;
		} else {
			n++;
		}
	}
	/* any trailing chars */
	if (u)
		*v = u;

	/* prep text: hex2bin, multiples of 4 */
	n = (text->av_len + 7) / 8;
	out = (uint8_t*) malloc(n * 8);
	ptr = (uint8_t *) text->av_val;
	v = (uint32_t *) out;
	for (i = 0; i < n; i++) {
		u = (HEX2BIN(ptr[0]) << 4) + HEX2BIN(ptr[1]);
		u |= ((HEX2BIN(ptr[2]) << 4) + HEX2BIN(ptr[3])) << 8;
		u |= ((HEX2BIN(ptr[4]) << 4) + HEX2BIN(ptr[5])) << 16;
		u |= ((HEX2BIN(ptr[6]) << 4) + HEX2BIN(ptr[7])) << 24;
		*v++ = u;
		ptr += 8;
	}
	v = (uint32_t *) out;

	/* http://www.movable-type.co.uk/scripts/tea-block.html */
#define MX (((z>>5)^(y<<2)) + ((y>>3)^(z<<4))) ^ ((sum^y) + (k[(p&3)^e]^z));
	z = v[n - 1];
	y = v[0];
	q = 6 + 52 / n;
	sum = q * DELTA;
	while (sum != 0) {
		e = sum >> 2 & 3;
		for (p = n - 1; p > 0; p--)
			z = v[p - 1], y = v[p] -= MX
		;
		z = v[n - 1];
		y = v[0] -= MX
		;
		sum -= DELTA;
	}

	text->av_len /= 2;
	memcpy(text->av_val, out, text->av_len);
	free(out);
}
/**
int32_t HTTP_Post(RTMP *r, RTMPTCmd cmd, const char *buf, int32_t len) {
	char hbuf[512];
	int32_t hlen = snprintf(hbuf, sizeof(hbuf), "POST /%s%s/%d HTTP/1.1\r\n"
			"Host: %.*s:%d\r\n"
			"Accept: *\r\n"
			"User-Agent: Shockwave Flash\r\n"
			"Connection: Keep-Alive\r\n"
			"Cache-Control: no-cache\r\n"
			"Content-type: application/x-fcs\r\n"
			"Content-length: %d\r\n\r\n", RTMPT_cmds[cmd],
			r->m_clientID.av_val ? r->m_clientID.av_val : "", r->m_msgCounter,
			r->Link.hostname.av_len, r->Link.hostname.av_val, r->Link.port,
			len);
	RTMPSockBuf_Send(&r->m_sb, hbuf, hlen);
	hlen = RTMPSockBuf_Send(&r->m_sb, buf, len);
	r->m_msgCounter++;
	r->m_unackd++;
	return hlen;
}

int32_t HTTP_read(YangRtmpLib *ylr, RTMP *r, int32_t fill) {
	char *ptr;
	int32_t hlen;

	restart: if (fill)
		RTMPSockBuf_Fill(ylr, &r->m_sb);
	if (r->m_sb.sb_size < 13) {
		if (fill)
			goto restart;
		return -2;
	}
	if (strncmp(r->m_sb.sb_start, "HTTP/1.1 200 ", 13))
		return -1;
	r->m_sb.sb_start[r->m_sb.sb_size] = '\0';
	if (!strstr(r->m_sb.sb_start, "\r\n\r\n")) {
		if (fill)
			goto restart;
		return -2;
	}

	ptr = r->m_sb.sb_start + sizeof("HTTP/1.1 200");
	while ((ptr = strstr(ptr, "Content-"))) {
		if (!strncasecmp(ptr + 8, "length:", 7))
			break;
		ptr += 8;
	}
	if (!ptr)
		return -1;
	hlen = atoi(ptr + 16);
	ptr = strstr(ptr + 16, "\r\n\r\n");
	if (!ptr)
		return -1;
	ptr += 4;
	if (ptr + (r->m_clientID.av_val ? 1 : hlen)
			> r->m_sb.sb_start + r->m_sb.sb_size) {
		if (fill)
			goto restart;
		return -2;
	}
	r->m_sb.sb_size -= ptr - r->m_sb.sb_start;
	r->m_sb.sb_start = ptr;
	r->m_unackd--;

	if (!r->m_clientID.av_val) {
		r->m_clientID.av_len = hlen;
		r->m_clientID.av_val = (char*) malloc(hlen + 1);
		if (!r->m_clientID.av_val)
			return -1;
		r->m_clientID.av_val[0] = '/';
		memcpy(r->m_clientID.av_val + 1, ptr, hlen - 1);
		r->m_clientID.av_val[hlen] = 0;
		r->m_sb.sb_size = 0;
	} else {
		r->m_polling = *ptr++;
		r->m_resplen = hlen - 1;
		r->m_sb.sb_start++;
		r->m_sb.sb_size--;
	}
	return 0;
}
**/
int32_t YangRtmpBase::RTMP_IsConnected() {
	return r->m_sb.sb_socket != -1;
}
double RTMP_GetDuration(RTMP *r) {
	return r->m_fDuration;
}

int32_t RTMP_Socket(RTMP *r) {
	return r->m_sb.sb_socket;
}

int32_t RTMP_IsTimedout(RTMP *r) {
	return r->m_sb.sb_timedout;
}

void RTMP_UpdateBufferMS(YangRtmpBase *ylr, RTMP *r) {
	ylr->RTMP_SendCtrl(3, r->m_stream_id, r->m_nBufferMS);
}
void YangRtmpBase::CloseInternal(int32_t reconnect) {
	int32_t i;
if(!r) return;
	if (RTMP_IsConnected()) {
		if (r->m_stream_id > 0&&(m_errState>4||m_errState==0)) {
			i = r->m_stream_id;
			r->m_stream_id = 0;
			if (r->m_isPublish)		SendFCUnpublish();
			SendDeleteStream(i);
		}
		if (r->m_clientID.av_val) {
			//HTTP_Post(r, RTMPT_CLOSE, "", 1);
			free(r->m_clientID.av_val);
			r->m_clientID.av_val = NULL;
			r->m_clientID.av_len = 0;
		}
		RTMPSockBuf_Close(&r->m_sb);
	}

	r->m_stream_id = -1;
	r->m_sb.sb_socket = -1;
	r->m_nBWCheckCounter = 0;
	r->m_nBytesIn = 0;
	r->m_nBytesInSent = 0;

	if (r->m_read.flags & RTMP_READ_HEADER) {
		free(r->m_read.buf);
		r->m_read.buf = NULL;
	}
	r->m_read.dataType = 0;
	r->m_read.flags = 0;
	r->m_read.status = 0;
	r->m_read.nResumeTS = 0;
	r->m_read.nIgnoredFrameCounter = 0;
	r->m_read.nIgnoredFlvFrameCounter = 0;

	r->m_write.m_nBytesRead = 0;
	RTMPPacket_Free(&r->m_write);
    if(r&&r->m_vecChannelsIn){
        //for (i = 0; i < r->m_channelsAllocatedIn; i++) {
         //   if (r->m_vecChannelsIn[i]) {
          //      RTMPPacket_Free(r->m_vecChannelsIn[i]);
          //  }
       // }
        free(r->m_vecChannelsIn);
    }
	r->m_vecChannelsIn = NULL;
	free(r->m_channelTimestamp);
	r->m_channelTimestamp = NULL;
	r->m_channelsAllocatedIn = 0;
    if(r&&r->m_vecChannelsOut){
        //for (i = 0; i < r->m_channelsAllocatedOut; i++) {
        //    if (r->m_vecChannelsOut[i]) {
         //       RTMPPacket_Free(r->m_vecChannelsOut[i]);
          //  }
      //  }
        free(r->m_vecChannelsOut);
    }
	r->m_vecChannelsOut = NULL;
	r->m_channelsAllocatedOut = 0;
	AV_clear(r->m_methodCalls, r->m_numCalls);
	r->m_methodCalls = NULL;
	r->m_numCalls = 0;
	r->m_numInvokes = 0;

	r->m_bPlaying = FALSE;
	r->m_sb.sb_size = 0;

	r->m_msgCounter = 0;
	r->m_resplen = 0;
	r->m_unackd = 0;

	if ((r->Link.lFlags & RTMP_LF_FTCU) && !reconnect) {
		free(r->Link.tcUrl.av_val);
		r->Link.tcUrl.av_val = NULL;
		r->Link.lFlags ^= RTMP_LF_FTCU;
	}
	if ((r->Link.lFlags & RTMP_LF_FAPU) && !reconnect) {
		free(r->Link.app.av_val);
		r->Link.app.av_val = NULL;
		r->Link.lFlags ^= RTMP_LF_FAPU;
	}

	//if (!reconnect) {
	//free(r->Link.playpath0.av_val);
	//r->Link.playpath0.av_val = NULL;
	//	}

}

void YangRtmpBase::RTMP_SetupStream(int32_t protocol, AVal *host, uint32_t  port,
		AVal *sockshost, AVal *playpath, AVal *tcUrl, AVal *swfUrl,
		AVal *pageUrl, AVal *app, AVal *auth, AVal *swfSHA256Hash,
		uint32_t swfSize, AVal *flashVer, AVal *subscribepath, AVal *usherToken,
		int32_t dStart, int32_t dStop, int32_t bLiveStream,  int64_t timeout) {
	yang_debug( "Protocol : %s", RTMPProtocolStrings[protocol & 7]);
	yang_debug( "Hostname : %.*s", host->av_len, host->av_val);
	yang_debug( "Port     : %d", port);
	yang_debug( "Playpath : %s", playpath->av_val);

	if (tcUrl && tcUrl->av_val)
		yang_debug( "tcUrl    : %s", tcUrl->av_val);
	if (swfUrl && swfUrl->av_val)
		yang_debug( "swfUrl   : %s", swfUrl->av_val);
	if (pageUrl && pageUrl->av_val)
		yang_debug( "pageUrl  : %s", pageUrl->av_val);
	if (app && app->av_val)
		yang_debug( "app      : %.*s", app->av_len, app->av_val);
	if (auth && auth->av_val)
		yang_debug( "auth     : %s", auth->av_val);
	if (subscribepath && subscribepath->av_val)
		yang_debug( "subscribepath : %s", subscribepath->av_val);
	if (usherToken && usherToken->av_val)
		yang_debug( "NetStream.Authenticate.UsherToken : %s",
				usherToken->av_val);
	if (flashVer && flashVer->av_val)
		yang_debug( "flashVer : %s", flashVer->av_val);
	if (dStart > 0)
		yang_debug( "StartTime     : %d msec", dStart);
	if (dStop > 0)
		yang_debug( "StopTime      : %d msec", dStop);

	yang_debug( "live     : %s", bLiveStream ? "yes" : "no");
	yang_debug( "timeout  : %ld sec", timeout);

	SocksSetup(sockshost);

	if (tcUrl && tcUrl->av_len)
		r->Link.tcUrl = *tcUrl;
	if (swfUrl && swfUrl->av_len)
		r->Link.swfUrl = *swfUrl;
	if (pageUrl && pageUrl->av_len)
		r->Link.pageUrl = *pageUrl;
	if (app && app->av_len)
		r->Link.app = *app;
	if (auth && auth->av_len) {
		r->Link.auth = *auth;
		r->Link.lFlags |= RTMP_LF_AUTH;
	}
	if (flashVer && flashVer->av_len)
		r->Link.flashVer = *flashVer;
	else
		r->Link.flashVer = RTMP_DefaultFlashVer;
	if (subscribepath && subscribepath->av_len)
		r->Link.subscribepath = *subscribepath;
	if (usherToken && usherToken->av_len)
		r->Link.usherToken = *usherToken;
	r->Link.seekTime = dStart;
	r->Link.stopTime = dStop;
	//if (bLiveStream)		r->Link.lFlags |= RTMP_LF_LIVE;
	r->Link.timeout = timeout;

	r->Link.protocol = protocol;
	r->Link.hostname = *host;
	r->Link.port = port;
	//	r->Link.playpatht = *playpath;

	if (r->Link.port == 0)
		r->Link.port = 1935;
}
void RTMP_ParsePlaypath(AVal *in, AVal *out) {
	int32_t addMP4 = 0;
	int32_t addMP3 = 0;
	int32_t subExt = 0;
	const char *playpath = in->av_val;
	const char *temp, *q, *ext = NULL;
	const char *ppstart = playpath;
	char *streamname, *destptr, *p;

	int32_t pplen = in->av_len;

	out->av_val = NULL;
	out->av_len = 0;

	if ((*ppstart == '?') &&
	    (temp=strstr(ppstart, "slist=")) != 0) {
		ppstart = temp+6;
		pplen = strlen(ppstart);

		temp = strchr(ppstart, '&');
		if (temp) {
			pplen = temp-ppstart;
		}
	}

	q = strchr(ppstart, '?');
	if (pplen >= 4) {
		if (q)
			ext = q-4;
		else
			ext = &ppstart[pplen-4];
		if ((strncmp(ext, ".f4v", 4) == 0) ||
		    (strncmp(ext, ".mp4", 4) == 0)) {
			addMP4 = 1;
			subExt = 1;
		/* Only remove .flv from rtmp URL, not slist params */
		} else if ((ppstart == playpath) &&
		    (strncmp(ext, ".flv", 4) == 0)) {
			subExt = 1;
		} else if (strncmp(ext, ".mp3", 4) == 0) {
			addMP3 = 1;
			subExt = 1;
		}
	}

	streamname = (char *)malloc((pplen+4+1)*sizeof(char));
	if (!streamname)
		return;

	destptr = streamname;
	if (addMP4) {
		if (strncmp(ppstart, "mp4:", 4)) {
			strcpy(destptr, "mp4:");
			destptr += 4;
		} else {
			subExt = 0;
		}
	} else if (addMP3) {
		if (strncmp(ppstart, "mp3:", 4)) {
			strcpy(destptr, "mp3:");
			destptr += 4;
		} else {
			subExt = 0;
		}
	}

 	for (p=(char *)ppstart; pplen >0;) {
		/* skip extension */
		if (subExt && p == ext) {
			p += 4;
			pplen -= 4;
			continue;
		}
		if (*p == '%') {
			uint32_t  c;
			sscanf(p+1, "%02x", &c);
			*destptr++ = c;
			pplen -= 3;
			p += 3;
		} else {
			*destptr++ = *p++;
			pplen--;
		}
	}
	*destptr = '\0';

	out->av_val = streamname;
	out->av_len = destptr - streamname;
}
int32_t YangRtmpBase::RTMP_ParseURL(char *url, int32_t *protocol, AVal *host, uint32_t  *port,
	AVal *playpath, AVal *app)
{
	char *p, *end, *col, *ques, *slash;

	yang_debug( "Parsing...");

	*protocol = RTMP_PROTOCOL_RTMP;
	*port = 0;
	playpath->av_len = 0;
	playpath->av_val = NULL;
	app->av_len = 0;
	app->av_val = NULL;

	/* Old School Parsing */

	/* look for usual :// pattern */
	p = strstr(url, "://");
	if(!p) {
		yang_error( "RTMP URL: No :// in url!");
		return FALSE;
	}
	{
	int32_t len = (int)(p-url);

	if(len == 4 && strncasecmp(url, "rtmp", 4)==0)
		*protocol = RTMP_PROTOCOL_RTMP;
	else if(len == 5 && strncasecmp(url, "rtmpt", 5)==0)
		*protocol = RTMP_PROTOCOL_RTMPT;
	else if(len == 5 && strncasecmp(url, "rtmps", 5)==0)
	        *protocol = RTMP_PROTOCOL_RTMPS;
	else if(len == 5 && strncasecmp(url, "rtmpe", 5)==0)
	        *protocol = RTMP_PROTOCOL_RTMPE;
	else if(len == 5 && strncasecmp(url, "rtmfp", 5)==0)
	        *protocol = RTMP_PROTOCOL_RTMFP;
	else if(len == 6 && strncasecmp(url, "rtmpte", 6)==0)
	        *protocol = RTMP_PROTOCOL_RTMPTE;
	else if(len == 6 && strncasecmp(url, "rtmpts", 6)==0)
	        *protocol = RTMP_PROTOCOL_RTMPTS;
	else {
		yang_warn( "Unknown protocol!\n");
		goto parsehost;
	}
	}

	yang_debug( "Parsed protocol: %d", *protocol);

parsehost:
	/* let's get the hostname */
	p+=3;

	/* check for sudden death */
	if(*p==0) {
		yang_warn( "No hostname in URL!");
		return FALSE;
	}

	end   = p + strlen(p);
	col   = strchr(p, ':');
	ques  = strchr(p, '?');
	slash = strchr(p, '/');

	{
	int32_t hostlen;
	if(slash)
		hostlen = slash - p;
	else
		hostlen = end - p;
	if(col && col -p < hostlen)
		hostlen = col - p;

	if(hostlen < 256) {
		host->av_val = p;
		host->av_len = hostlen;
		yang_debug( "Parsed host    : %.*s", hostlen, host->av_val);
	} else {
		yang_warn( "Hostname exceeds 255 characters!");
	}

	p+=hostlen;
	}

	/* get the port number if available */
	if(*p == ':') {
		uint32_t  p2;
		p++;
		p2 = atoi(p);
		if(p2 > 65535) {
			yang_warn( "Invalid port number!");
		} else {
			*port = p2;
		}
	}

	if(!slash) {
		yang_warn( "No application or playpath in URL!");
		return TRUE;
	}
	p = slash+1;

	{
	/* parse application
	 *
	 * rtmp://host[:port]/app[/appinstance][/...]
	 * application = app[/appinstance]
	 */

	char *slash2, *slash3 = NULL, *slash4 = NULL;
	int32_t applen, appnamelen;

	slash2 = strchr(p, '/');
	if(slash2)
		slash3 = strchr(slash2+1, '/');
	if(slash3)
		slash4 = strchr(slash3+1, '/');

	applen = end-p; /* ondemand, pass all parameters as app */
	appnamelen = applen; /* ondemand length */

	if(ques && strstr(p, "slist=")) { /* whatever it is, the '?' and slist= means we need to use everything as app and parse plapath from slist= */
		appnamelen = ques-p;
	}
	else if(strncmp(p, "ondemand/", 9)==0) {
                /* app = ondemand/foobar, only pass app=ondemand */
                applen = 8;
                appnamelen = 8;
        }
	else { /* app!=ondemand, so app is app[/appinstance] */
		if(slash4)
			appnamelen = slash4-p;
		else if(slash3)
			appnamelen = slash3-p;
		else if(slash2)
			appnamelen = slash2-p;

		applen = appnamelen;
	}

	app->av_val = p;
	app->av_len = applen;
	yang_debug( "Parsed app     : %.*s", applen, p);

	p += appnamelen;
	}

	if (*p == '/')
		p++;

	if (end-p) {
		AVal av;
		av.av_val=p;
		av.av_len=end-p;//{p, end-p};
		RTMP_ParsePlaypath(&av, playpath);
	}

	return TRUE;
}
int32_t YangRtmpBase::RTMP_ParseURL1(char *url, int32_t *protocol, AVal *host, uint32_t  *port,AVal *app)
{
	char *p, *end, *col, *ques, *slash;

	yang_debug( "Parsing...");

	*protocol = RTMP_PROTOCOL_RTMP;
	*port = 0;
//	playpath->av_len = 0;
	//playpath->av_val = NULL;
	app->av_len = 0;
	app->av_val = NULL;

	/* Old School Parsing */

	/* look for usual :// pattern */
	p = strstr(url, "://");
	if(!p) {
		yang_error( "RTMP URL: No :// in url!");
		return FALSE;
	}
	{
	int32_t len = (int)(p-url);

	if(len == 4 && strncasecmp(url, "rtmp", 4)==0)
		*protocol = RTMP_PROTOCOL_RTMP;
	else if(len == 5 && strncasecmp(url, "rtmpt", 5)==0)
		*protocol = RTMP_PROTOCOL_RTMPT;
	else if(len == 5 && strncasecmp(url, "rtmps", 5)==0)
	        *protocol = RTMP_PROTOCOL_RTMPS;
	else if(len == 5 && strncasecmp(url, "rtmpe", 5)==0)
	        *protocol = RTMP_PROTOCOL_RTMPE;
	else if(len == 5 && strncasecmp(url, "rtmfp", 5)==0)
	        *protocol = RTMP_PROTOCOL_RTMFP;
	else if(len == 6 && strncasecmp(url, "rtmpte", 6)==0)
	        *protocol = RTMP_PROTOCOL_RTMPTE;
	else if(len == 6 && strncasecmp(url, "rtmpts", 6)==0)
	        *protocol = RTMP_PROTOCOL_RTMPTS;
	else {
		yang_warn( "Unknown protocol!\n");
		goto parsehost;
	}
	}

	yang_debug( "Parsed protocol: %d", *protocol);

parsehost:
	/* let's get the hostname */
	p+=3;

	/* check for sudden death */
	if(*p==0) {
		yang_warn( "No hostname in URL!");
		return FALSE;
	}

	end   = p + strlen(p);
	col   = strchr(p, ':');
	ques  = strchr(p, '?');
	slash = strchr(p, '/');

	{
	int32_t hostlen;
	if(slash)
		hostlen = slash - p;
	else
		hostlen = end - p;
	if(col && col -p < hostlen)
		hostlen = col - p;

	if(hostlen < 256) {
		host->av_val = p;
		host->av_len = hostlen;
		yang_debug( "Parsed host    : %.*s", hostlen, host->av_val);
	} else {
		yang_warn( "Hostname exceeds 255 characters!");
	}

	p+=hostlen;
	}

	/* get the port number if available */
	if(*p == ':') {
		uint32_t  p2;
		p++;
		p2 = atoi(p);
		if(p2 > 65535) {
			yang_warn( "Invalid port number!");
		} else {
			*port = p2;
		}
	}

	if(!slash) {
		yang_warn( "No application or playpath in URL!");
		return TRUE;
	}
	p = slash+1;

	{
	/* parse application
	 *
	 * rtmp://host[:port]/app[/appinstance][/...]
	 * application = app[/appinstance]
	 */

	char *slash2, *slash3 = NULL, *slash4 = NULL;
	int32_t applen, appnamelen;

	slash2 = strchr(p, '/');
	if(slash2)
		slash3 = strchr(slash2+1, '/');
	if(slash3)
		slash4 = strchr(slash3+1, '/');

	applen = end-p; /* ondemand, pass all parameters as app */
	appnamelen = applen; /* ondemand length */

	if(ques && strstr(p, "slist=")) { /* whatever it is, the '?' and slist= means we need to use everything as app and parse plapath from slist= */
		appnamelen = ques-p;
	}
	else if(strncmp(p, "ondemand/", 9)==0) {
                /* app = ondemand/foobar, only pass app=ondemand */
                applen = 8;
                appnamelen = 8;
        }
	else { /* app!=ondemand, so app is app[/appinstance] */
		if(slash4)
			appnamelen = slash4-p;
		else if(slash3)
			appnamelen = slash3-p;
		else if(slash2)
			appnamelen = slash2-p;

		applen = appnamelen;
	}

	app->av_val = p;
	app->av_len = applen;
	yang_debug( "Parsed app     : %.*s", applen, p);

	p += appnamelen;
	}

	if (*p == '/')
		p++;

	if (end-p) {
		AVal av;
		av.av_val=p;
		av.av_len=end-p;//{p, end-p};
		//RTMP_ParsePlaypath(&av, playpath);
	}

	return TRUE;
}
int32_t YangRtmpBase::yang_SetupURL(char *url,int32_t pport) {
	//	r->Link.count=1;
	//	r->Link.curIndex=0;
	AVal opt, arg;
	char *p1, *p2, *ptr = strchr(url, ' ');
	int32_t ret=0, len=0;
	uint32_t  port = 0;//pport;

	if (ptr) *ptr = '\0';
	len = strlen(url);
	while (ptr) {
		*ptr++ = '\0';
		p1 = ptr;
		p2 = strchr(p1, '=');
		if (!p2)
			break;
		opt.av_val = p1;
		opt.av_len = p2 - p1;
		*p2++ = '\0';
		arg.av_val = p2;
		ptr = strchr(p2, ' ');
		if (ptr) {
			*ptr = '\0';
			arg.av_len = ptr - p2;
			/* skip repeated spaces */
			while (ptr[1] == ' ')
				*ptr++ = '\0';
		} else {
			arg.av_len = strlen(p2);
		}

		/* unescape */
		port = arg.av_len;
		for (p1 = p2; port > 0;) {
			if (*p1 == '\\') {
				uint32_t  c;
				if (port < 3)
					return FALSE;
				sscanf(p1 + 1, "%02x", &c);
				*p2++ = c;
				port -= 3;
				p1 += 3;
			} else {
				*p2++ = *p1++;
				port--;
			}
		}
		arg.av_len = p2 - arg.av_val;

		ret = RTMP_SetOpt(&opt, &arg);
		if (!ret)
			return ret;
	}

	SocksSetup( &r->Link.sockshost);

	if (r->Link.port == 0)
		r->Link.port = 1935;
	return TRUE;
}

void YangRtmpBase::yang_handleconn(AMFObject *pob) {
	char nstr[128], vstr[128];

	for (int32_t n = 0; n < pob->o_num; n++) {
		if (pob->o_props[n].p_type == AMF_OBJECT) {
			AMFObject *ob1 = &pob->o_props[n].p_vu.p_object;
			for (int32_t m = 0; m < ob1->o_num; m++) {
				memset(nstr,0,sizeof(nstr));
				memset(vstr,0,sizeof(vstr));
				if (ob1->o_props[m].p_type == AMF_STRING) {
					memcpy(nstr,ob1->o_props[m].p_name.av_val,ob1->o_props[m].p_name.av_len);
					memcpy(vstr,ob1->o_props[m].p_vu.p_aval.av_val,ob1->o_props[m].p_vu.p_aval.av_len);
					if(strcmp("code",nstr)==0) {memset(r->netStatus.code,0,sizeof(r->netStatus.code));strcpy(r->netStatus.code,vstr);}
					if(strcmp("description",nstr)==0) {memset(r->netStatus.description,0,sizeof(r->netStatus.description));strcpy(r->netStatus.description,vstr);}
					if(strcmp("fmsVer",nstr)==0) {memset(r->netStatus.fmsVer,0,sizeof(r->netStatus.fmsVer));strcpy(r->netStatus.fmsVer,vstr);}
					//snprintf(nstr, 255, "%s", ob1->o_props[m].p_name.av_val);
					//snprintf(vstr, 255, "%s",ob1->o_props[m].p_vu.p_aval.av_val);
				}
				//printf("\n\name=%s,value=%s\n",nstr,vstr);

			}
			ob1 = NULL;
		}
	}
}



void YangRtmpBase::initRTMP(char *serverIp, char *app, int32_t port) {
	if (r == NULL) {
		r = (RTMP*) calloc(1, sizeof(RTMP));
		RTMP_Init();
		char url[128];
		memset(pathChar, 0, sizeof(pathChar));
		r->Link.curPath.av_val = pathChar;
		r->Link.curPath.av_len = 0;
		r->Link.port = port;
		//initAVStreams();
		/**char app[30];
		 memset(app,0,30);
		 sprintf(app,"%s","live");**/
		memset(url, 0, sizeof(url));
		sprintf(url, "rtmp://%s/%s", serverIp, app);
		//printf("\nurl========================%s\n",url);
		int32_t tsize = strlen(url);
		r->Link.tcUrl.av_val = (char*) malloc(tsize + 1);
		r->Link.tcUrl.av_len = tsize;
		memcpy(r->Link.tcUrl.av_val, url, tsize);
		r->Link.tcUrl.av_val[tsize] = '\0';

		tsize = strlen(app);
		r->Link.app.av_val = (char*) malloc(tsize + 1);
		r->Link.app.av_len = tsize;
		memcpy(r->Link.app.av_val, app, tsize);
		r->Link.app.av_val[tsize] = '\0';

		tsize = strlen(serverIp);
		r->Link.hostname.av_val = (char*) malloc(tsize + 1);
		r->Link.hostname.av_len = tsize;
		memcpy(r->Link.hostname.av_val, serverIp, tsize);
		r->Link.hostname.av_val[tsize] = '\0';

		r->Link.protocol = 0;
		//printf("url================%s",url);
		yang_SetupURL(url, port);
		//r->Link.curVideoPlayIndex = 0;
		//r->Link.curAudioPlayIndex = 0;
		//	r->Link.audioPlayCount = 0;
		//r->Link.videoPlayCount = 0;
		//	r->Link.curVideoPubIndex = 0;
		//	r->Link.curAudioPubIndex = 0;
		//	r->Link.audioPubCount = 0;
		//	r->Link.videoPubCount = 0;
	}
	//if(pflag==YangRtmpRead) r->Link.lFlags|= RTMP_LF_LIVE;
	//int32_t rtmpRet= RTMP_Connect( NULL);
}
void YangRtmpBase::initRTMP(char *serverIp, char *app, int32_t pmeetingId,int32_t port) {
	if (r == NULL) {
		r = (RTMP*) calloc(1, sizeof(RTMP));
		RTMP_Init();
		char url[128];

		//initAVStreams();
		memset(pathChar, 0, 30);
		r->Link.curPath.av_val = pathChar;
		r->Link.curPath.av_len = 0;

		r->Link.port = port;

		memset(url, 0, sizeof(url));
		sprintf(url, "%s/%d", app, pmeetingId);
		int32_t tsize = strlen(url);
		r->Link.app.av_val = (char*) malloc(tsize + 1);
		r->Link.app.av_len = tsize;
		memcpy(r->Link.app.av_val, url, tsize);
		r->Link.app.av_val[tsize] = '\0';

		memset(url, 0, sizeof(url));
		sprintf(url, "rtmp://%s/%s/%d", serverIp, app, pmeetingId);
		//printf("\nurl===%s", url);
		tsize = strlen(url);
		r->Link.tcUrl.av_val = (char*) malloc(tsize + 1);
		r->Link.tcUrl.av_len = tsize;
		memcpy(r->Link.tcUrl.av_val, url, tsize);
		r->Link.tcUrl.av_val[tsize] = '\0';

		tsize = strlen(serverIp);
		r->Link.hostname.av_val = (char*) malloc(tsize + 1);
		r->Link.hostname.av_len = tsize;
		memcpy(r->Link.hostname.av_val, serverIp, tsize);
		r->Link.hostname.av_val[tsize] = '\0';

		r->Link.protocol = 0;

		yang_SetupURL(url, port);
		//r->Link.curVideoPlayIndex = 0;
		//r->Link.curAudioPlayIndex = 0;
		//r->Link.audioPlayCount = 0;
		//r->Link.videoPlayCount = 0;
		//r->Link.curVideoPubIndex = 0;
		//r->Link.curAudioPubIndex = 0;
		//r->Link.audioPubCount = 0;
		//r->Link.videoPubCount = 0;
	}
	//if(pflag==YangRtmpRead) r->Link.lFlags|= RTMP_LF_LIVE;
	//int32_t rtmpRet= RTMP_Connect( NULL);
}
int32_t YangRtmpBase::RTMP_SetupURL(char *url) {
	AVal opt, arg;
	char *p1, *p2, *ptr = strchr(url, ' ');
	int32_t ret, len;
	uint32_t  port = 0;

	if (ptr)
		*ptr = '\0';

	len = strlen(url);
	//ret = RTMP_ParseURL(url, &r->Link.protocol, &r->Link.hostname, &port,&r->Link.pubVideoPath[0], &r->Link.app);
	ret = RTMP_ParseURL1(url, &r->Link.protocol, &r->Link.hostname, &port,
			&r->Link.app);
	if (!ret)
		return ret;
	r->Link.port = port;
	// r->Link.playpath = r->Link.playpath0;

	while (ptr) {
		*ptr++ = '\0';
		p1 = ptr;
		p2 = strchr(p1, '=');
		if (!p2)
			break;
		opt.av_val = p1;
		opt.av_len = p2 - p1;
		*p2++ = '\0';
		arg.av_val = p2;
		ptr = strchr(p2, ' ');
		if (ptr) {
			*ptr = '\0';
			arg.av_len = ptr - p2;
			/* skip repeated spaces */
			while (ptr[1] == ' ')
				*ptr++ = '\0';
		} else {
			arg.av_len = strlen(p2);
		}

		/* unescape */
		port = arg.av_len;
		for (p1 = p2; port > 0;) {
			if (*p1 == '\\') {
				uint32_t  c;
				if (port < 3)
					return FALSE;
				sscanf(p1 + 1, "%02x", &c);
				*p2++ = c;
				port -= 3;
				p1 += 3;
			} else {
				*p2++ = *p1++;
				port--;
			}
		}
		arg.av_len = p2 - arg.av_val;

		ret = RTMP_SetOpt(&opt, &arg);
		if (!ret)
			return ret;
	}

	if (!r->Link.tcUrl.av_len) {
		r->Link.tcUrl.av_val = url;
		if (r->Link.app.av_len) {
			if (r->Link.app.av_val < url + len) {
				/* if app is part of original url, just use it */
				r->Link.tcUrl.av_len = r->Link.app.av_len
						+ (r->Link.app.av_val - url);
			} else {
				len = r->Link.hostname.av_len + r->Link.app.av_len
						+ sizeof("rtmpte://:65535/");
				r->Link.tcUrl.av_val = (char*) malloc(len);
				r->Link.tcUrl.av_len = snprintf(r->Link.tcUrl.av_val, len,
						"%s://%.*s:%d/%.*s",
						RTMPProtocolStringsLower[r->Link.protocol],
						r->Link.hostname.av_len, r->Link.hostname.av_val,
						r->Link.port, r->Link.app.av_len, r->Link.app.av_val);
				r->Link.lFlags |= RTMP_LF_FTCU;
			}
		} else {
			r->Link.tcUrl.av_len = strlen(url);
		}
	}

#ifdef CRYPTO
	if ((r->Link.lFlags & RTMP_LF_SWFV) && r->Link.swfUrl.av_len)
	RTMP_HashSWF(r->Link.swfUrl.av_val, &r->Link.SWFSize,
			(uint8_t *)r->Link.SWFHash, r->Link.swfAge);
#endif

	SocksSetup(&r->Link.sockshost);

	if (r->Link.port == 0) {
		if (r->Link.protocol & RTMP_FEATURE_SSL)
			r->Link.port = 443;
		else if (r->Link.protocol & RTMP_FEATURE_HTTP)
			r->Link.port = 80;
		else
			r->Link.port = 1935;
	}
	return TRUE;
}

//void YangLibRtmp::RTMP_EnableWrite() {
//	r->Link.protocol |= RTMP_FEATURE_WRITE;
//}

void YangRtmpBase::setBufferMS(int32_t size) {
	r->m_nBufferMS = size;
}
/**
int32_t YangRtmpLib::RTMP_GetNextMediaPacket(RTMPPacket *packet) {
	int32_t bHasMediaPacket = 0;

	while (!bHasMediaPacket && RTMP_IsConnected(r) && readPacket(packet)) {
		if (!RTMPPacket_IsReady(packet) || !packet->m_nBodySize) {
			continue;
		}

		bHasMediaPacket = clientPacket(packet);

		if (!bHasMediaPacket) {
			RTMPPacket_Free(packet);
		} else if (r->m_pausing == 3) {
			if (packet->m_nTimeStamp <= r->m_mediaStamp) {
				bHasMediaPacket = 0;
#ifdef _DEBUG
				yang_debug(
						"Skipped type: %02X, size: %d, TS: %d ms, abs TS: %d, pause: %d ms",
						packet->m_packetType, packet->m_nBodySize,
						packet->m_nTimeStamp, packet->m_hasAbsTimestamp,
						r->m_mediaStamp);
#endif
				RTMPPacket_Free(packet);
				continue;
			}
			r->m_pausing = 0;
		}
	}

	if (bHasMediaPacket)
		r->m_bPlaying = TRUE;
	else if (r->m_sb.sb_timedout && !r->m_pausing)
		r->m_pauseStamp =
				r->m_mediaChannel < r->m_channelsAllocatedIn ?
						r->m_channelTimestamp[r->m_mediaChannel] : 0;

	return bHasMediaPacket;
}
**/
/**
 AVal YangRtmpLib::getPlayPath1() {
 if (r->m_isPublish) {
 if (r->Link.streamType == YangAudioStream) {
 return r->Link.pubAudioPath[r->Link.curAudioPubIndex];
 } else
 return r->Link.pubVideoPath[r->Link.curVideoPubIndex];
 } else {
 if (r->Link.streamType == YangAudioStream) {
 return r->Link.playAudioPath[r->Link.curAudioPlayIndex];
 } else
 return r->Link.playVideoPath[r->Link.curVideoPlayIndex];
 }
 }**/
