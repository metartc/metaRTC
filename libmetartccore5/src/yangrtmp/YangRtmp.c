//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtmp/YangRtmp2.h>
char *NetStatusEvent[4]={"NetConnection.Connect.Success","NetConnection.Connect.Failed","NetConnection.Connect.Closed","NetConnection.Connect.Rejected"};

int32_t sendPacket(RTMP* r,RTMPPacket *packet, int32_t queue);
int32_t yangSendPacket(RTMP* r,RTMPPacket *packet, int32_t queue);
int32_t WriteN(RTMP* r,const char *buffer, int32_t n) ;
int32_t SHandShake(RTMP* r);
int32_t ReadN(RTMP* r,char *buffer, int32_t n);
int32_t RTMP_Connect0(RTMP* r,struct sockaddr *service);
int32_t RTMP_Connect1(RTMP* r,RTMPPacket *cp);
int32_t SendConnectPacket(RTMP* r,RTMPPacket *cp) ;
int32_t HandleInvoke(RTMP* r,const char *body, uint32_t  nBodySize);


void RTMP_Close(RTMP* r) {
	if (r) {
		CloseInternal(r,0);
	}

}


AVal* getPlayPath(RTMP* r) {
	return &r->Link.curPath;
}
int32_t SendFCPublish(RTMP* r) {
	RTMPPacket packet;
	char pbuf[1024], *pend = pbuf + sizeof(pbuf);
	char *enc;

	packet.m_nChannel = 0x03; /* control channel (invoke) */
	packet.m_headerType = RTMP_PACKET_SIZE_LARGE;	//RTMP_PACKET_SIZE_MEDIUM;
	packet.m_packetType = RTMP_PACKET_TYPE_INVOKE;
	packet.m_nTimeStamp = 0;
	packet.m_nInfoField2 = 0;
	packet.m_hasAbsTimestamp = 0;
	packet.m_body = pbuf + RTMP_MAX_HEADER_SIZE;

	enc = packet.m_body;
	enc = AMF_EncodeString(enc, pend, &av_FCPublish);
	enc = AMF_EncodeNumber(enc, pend, ++r->m_numInvokes);
	*enc++ = AMF_NULL;
	enc = AMF_EncodeString(enc, pend, getPlayPath(r));
	if (!enc)
		return FALSE;

	packet.m_nBodySize = enc - packet.m_body;

	return sendPacket(r,&packet, FALSE);
}

int32_t SendFCUnpublish(RTMP* r) {
	RTMPPacket packet;
	char pbuf[1024], *pend = pbuf + sizeof(pbuf);
	char *enc;

	packet.m_nChannel = 0x03; /* control channel (invoke) */
	packet.m_headerType = RTMP_PACKET_SIZE_LARGE;	//RTMP_PACKET_SIZE_MEDIUM;
	packet.m_packetType = RTMP_PACKET_TYPE_INVOKE;
	packet.m_nTimeStamp = 0;
	packet.m_nInfoField2 = 0;
	packet.m_hasAbsTimestamp = 0;
	packet.m_body = pbuf + RTMP_MAX_HEADER_SIZE;

	enc = packet.m_body;
	enc = AMF_EncodeString(enc, pend, &av_FCUnpublish);
	enc = AMF_EncodeNumber(enc, pend, ++r->m_numInvokes);
	*enc++ = AMF_NULL;
	enc = AMF_EncodeString(enc, pend, getPlayPath(r));
	if (!enc)
		return FALSE;

	packet.m_nBodySize = enc - packet.m_body;

	return sendPacket(r,&packet, FALSE);
}
int32_t SendPublish(RTMP* r) {

	RTMPPacket packet;
	char pbuf[1024], *pend = pbuf + sizeof(pbuf);
	char *enc;

	packet.m_nChannel = 0x04; /* source channel (invoke) */
	packet.m_headerType = RTMP_PACKET_SIZE_LARGE;
	packet.m_packetType = RTMP_PACKET_TYPE_INVOKE;
	packet.m_nTimeStamp = 0;
	packet.m_nInfoField2 = r->m_stream_id;
	packet.m_hasAbsTimestamp = 0;
	packet.m_body = pbuf + RTMP_MAX_HEADER_SIZE;

	enc = packet.m_body;
	enc = AMF_EncodeString(enc, pend, &av_publish);
	enc = AMF_EncodeNumber(enc, pend, ++r->m_numInvokes);
	*enc++ = AMF_NULL;
	enc = AMF_EncodeString(enc, pend, getPlayPath(r));
	if (!enc)
		return FALSE;


	enc = AMF_EncodeString(enc, pend, &av_live);
	if (!enc)
		return FALSE;

	packet.m_nBodySize = enc - packet.m_body;

	return sendPacket(r,&packet, TRUE);
}

int32_t RTMP_SendPause(RTMP* r,int32_t DoPause, int32_t iTime) {
	RTMPPacket packet;
	char pbuf[256], *pend = pbuf + sizeof(pbuf);
	char *enc;

	packet.m_nChannel = 0x08; /* video channel */
	packet.m_headerType = RTMP_PACKET_SIZE_MEDIUM;
	packet.m_packetType = RTMP_PACKET_TYPE_INVOKE;
	packet.m_nTimeStamp = 0;
	packet.m_nInfoField2 = 0;
	packet.m_hasAbsTimestamp = 0;
	packet.m_body = pbuf + RTMP_MAX_HEADER_SIZE;

	enc = packet.m_body;
	enc = AMF_EncodeString(enc, pend, &av_pause);
	enc = AMF_EncodeNumber(enc, pend, ++r->m_numInvokes);
	*enc++ = AMF_NULL;
	enc = AMF_EncodeBoolean(enc, pend, DoPause);
	enc = AMF_EncodeNumber(enc, pend, (double) iTime);

	packet.m_nBodySize = enc - packet.m_body;

	yang_debug( "%s, %d, pauseTime=%d", __FUNCTION__, DoPause,	iTime);
	return sendPacket(r,&packet, TRUE);
}

int32_t RTMP_Pause(RTMP* r,int32_t DoPause) {
	if (DoPause)	r->m_pauseStamp =r->m_mediaChannel < r->m_channelsAllocatedIn ?r->m_channelTimestamp[r->m_mediaChannel] : 0;
	return RTMP_SendPause(r,DoPause, r->m_pauseStamp);
}

int32_t SendBytesReceived(RTMP* r) {
	RTMPPacket packet;
	char pbuf[256], *pend = pbuf + sizeof(pbuf);

	packet.m_nChannel = 0x02; /* control channel (invoke) */
	packet.m_headerType = RTMP_PACKET_SIZE_LARGE;	//RTMP_PACKET_SIZE_MEDIUM;
	packet.m_packetType = RTMP_PACKET_TYPE_BYTES_READ_REPORT;
	packet.m_nTimeStamp = 0;
	packet.m_nInfoField2 = 0;
	packet.m_hasAbsTimestamp = 0;
	packet.m_body = pbuf + RTMP_MAX_HEADER_SIZE;

	packet.m_nBodySize = 4;

	AMF_EncodeInt32(packet.m_body, pend, r->m_nBytesIn); /* hard coded for now */
	r->m_nBytesInSent = r->m_nBytesIn;

	return yangSendPacket(r,&packet, FALSE);
}
int32_t SendPlay(RTMP* r) {
	RTMPPacket packet;
	char pbuf[1024], *pend = pbuf + sizeof(pbuf);
	char *enc;

	packet.m_nChannel = 0x08; /* we make 8 our stream channel */
	packet.m_headerType = RTMP_PACKET_SIZE_LARGE;
	packet.m_packetType = RTMP_PACKET_TYPE_INVOKE;
	packet.m_nTimeStamp = 0;
	packet.m_nInfoField2 = r->m_stream_id; /*0x01000000; */
	packet.m_hasAbsTimestamp = 0;
	packet.m_body = pbuf + RTMP_MAX_HEADER_SIZE;

	enc = packet.m_body;
	enc = AMF_EncodeString(enc, pend, &av_play);
	enc = AMF_EncodeNumber(enc, pend, ++r->m_numInvokes);
	*enc++ = AMF_NULL;

	yang_debug( "%s, seekTime=%d, stopTime=%d, sending play: %s",
			__FUNCTION__, r->Link.seekTime, r->Link.stopTime,
			getPlayPath(r)->av_val);
	enc = AMF_EncodeString(enc, pend, getPlayPath(r));
	if (!enc)
		return FALSE;

	/* Optional parameters start and len.
	 *
	 * start: -2, -1, 0, positive number
	 *  -2: looks for a live stream, then a recorded stream,
	 *      if not found any open a live stream
	 *  -1: plays a live stream
	 * >=0: plays a recorded streams from 'start' milliseconds
	 */
	//if (r->Link.lFlags & RTMP_LF_LIVE)
	enc = AMF_EncodeNumber(enc, pend, -1000.0);
	/**else {
	 if (r->Link.seekTime > 0.0)
	 enc = AMF_EncodeNumber(enc, pend, r->Link.seekTime); // resume from here
	 else
	 enc = AMF_EncodeNumber(enc, pend, 0.0); //-2000.0); recorded as default, -2000.0 is not reliable since that freezes the player if the stream is not found
	 }**/
	if (!enc)
		return FALSE;

	/* len: -1, 0, positive number
	 *  -1: plays live or recorded stream to the end (default)
	 *   0: plays a frame 'start' ms away from the beginning
	 *  >0: plays a live or recoded stream for 'len' milliseconds
	 */
	/*enc += EncodeNumber(enc, -1.0); *//* len */
	if (r->Link.stopTime) {
		enc = AMF_EncodeNumber(enc, pend, r->Link.stopTime - r->Link.seekTime);
		if (!enc)
			return FALSE;
	}

	packet.m_nBodySize = enc - packet.m_body;

	return sendPacket(r,&packet, TRUE);
}

int32_t SendPlaylist(RTMP* r) {
	RTMPPacket packet;
	char pbuf[1024], *pend = pbuf + sizeof(pbuf);
	char *enc;

	packet.m_nChannel = 0x08; /* we make 8 our stream channel */
	packet.m_headerType = RTMP_PACKET_SIZE_LARGE;
	packet.m_packetType = RTMP_PACKET_TYPE_INVOKE;
	packet.m_nTimeStamp = 0;
	packet.m_nInfoField2 = r->m_stream_id; /*0x01000000; */
	packet.m_hasAbsTimestamp = 0;
	packet.m_body = pbuf + RTMP_MAX_HEADER_SIZE;

	enc = packet.m_body;
	enc = AMF_EncodeString(enc, pend, &av_set_playlist);
	enc = AMF_EncodeNumber(enc, pend, 0);
	*enc++ = AMF_NULL;
	*enc++ = AMF_ECMA_ARRAY;
	*enc++ = 0;
	*enc++ = 0;
	*enc++ = 0;
	*enc++ = AMF_OBJECT;
	enc = AMF_EncodeNamedString(enc, pend, &av_0, getPlayPath(r));
	if (!enc)
		return FALSE;
	if (enc + 3 >= pend)
		return FALSE;
	*enc++ = 0;
	*enc++ = 0;
	*enc++ = AMF_OBJECT_END;

	packet.m_nBodySize = enc - packet.m_body;

	return sendPacket(r,&packet, TRUE);
}

int32_t SendSecureTokenResponse(RTMP* r,AVal *resp) {
	RTMPPacket packet;
	char pbuf[1024], *pend = pbuf + sizeof(pbuf);
	char *enc;

	packet.m_nChannel = 0x03; /* control channel (invoke) */
	packet.m_headerType = RTMP_PACKET_SIZE_LARGE;	//RTMP_PACKET_SIZE_MEDIUM;
	packet.m_packetType = RTMP_PACKET_TYPE_INVOKE;
	packet.m_nTimeStamp = 0;
	packet.m_nInfoField2 = 0;
	packet.m_hasAbsTimestamp = 0;
	packet.m_body = pbuf + RTMP_MAX_HEADER_SIZE;

	enc = packet.m_body;
	enc = AMF_EncodeString(enc, pend, &av_secureTokenResponse);
	enc = AMF_EncodeNumber(enc, pend, 0.0);
	*enc++ = AMF_NULL;
	enc = AMF_EncodeString(enc, pend, resp);
	if (!enc)
		return FALSE;

	packet.m_nBodySize = enc - packet.m_body;

	return sendPacket(r,&packet, FALSE);
}
/*
 from http://jira.red5.org/confluence/display/docs/Ping:

 Ping is the most mysterious message in RTMP and till now we haven't fully interpreted it yet. In summary, Ping message is used as a special command that are exchanged between client and server. This page aims to document all known Ping messages. Expect the list to grow.

 The type of Ping packet is 0x4 and contains two mandatory parameters and two optional parameters. The first parameter is the type of Ping and in short integer. The second parameter is the target of the ping. As Ping is always sent in Channel 2 (control channel) and the target object in RTMP header is always 0 which means the Connection object, it's necessary to put an extra parameter to indicate the exact target object the Ping is sent to. The second parameter takes this responsibility. The value has the same meaning as the target object field in RTMP header. (The second value could also be used as other purposes, like RTT Ping/Pong. It is used as the timestamp.) The third and fourth parameters are optional and could be looked upon as the parameter of the Ping packet. Below is an unexhausted list of Ping messages.

 * type 0: Clear the stream. No third and fourth parameters. The second parameter could be 0. After the connection is established, a Ping 0,0 will be sent from server to client. The message will also be sent to client on the start of Play and in response of a Seek or Pause/Resume request. This Ping tells client to re-calibrate the clock with the timestamp of the next packet server sends.
 * type 1: Tell the stream to clear the playing buffer.
 * type 3: Buffer time of the client. The third parameter is the buffer time in millisecond.
 * type 4: Reset a stream. Used together with type 0 in the case of VOD. Often sent before type 0.
 * type 6: Ping the client from server. The second parameter is the current time.
 * type 7: Pong reply from client. The second parameter is the time the server sent with his ping request.
 * type 26: SWFVerification request
 * type 27: SWFVerification response
 */
int32_t RTMP_SendCtrl(RTMP* r,short nType, uint32_t  nObject,
		uint32_t  nTime) {
	RTMPPacket packet;
	char pbuf[256], *pend = pbuf + sizeof(pbuf);
	int32_t nSize;
	char *buf;

	yang_debug( "sending ctrl. type: 0x%04x",
			(unsigned short) nType);

	packet.m_nChannel = 0x02; /* control channel (ping) */
	packet.m_headerType = RTMP_PACKET_SIZE_LARGE;	//RTMP_PACKET_SIZE_MEDIUM;
	packet.m_packetType = RTMP_PACKET_TYPE_CONTROL;
	packet.m_nTimeStamp = 0; /* RTMP_GetTime(); */
	packet.m_nInfoField2 = 0;
	packet.m_hasAbsTimestamp = 0;
	packet.m_body = pbuf + RTMP_MAX_HEADER_SIZE;

	switch (nType) {
	case 0x03:
		nSize = 10;
		break; /* buffer time */
	case 0x1A:
		nSize = 3;
		break; /* SWF verify request */
	case 0x1B:
		nSize = 44;
		break; /* SWF verify response */
	default:
		nSize = 6;
		break;
	}

	packet.m_nBodySize = nSize;

	buf = packet.m_body;
	buf = AMF_EncodeInt16(buf, pend, nType);

	if (nType == 0x1B) {
	} else if (nType == 0x1A) {
		*buf = nObject & 0xff;
	} else {
		if (nSize > 2)
			buf = AMF_EncodeInt32(buf, pend, nObject);

		if (nSize > 6)
			buf = AMF_EncodeInt32(buf, pend, nTime);
	}

	return sendPacket(r,&packet, FALSE);
}

int32_t RTMP_SendCtrlPong(RTMP* r,short nType, char *p, int32_t len) {
	RTMPPacket packet;
	char pbuf[256], *pend = pbuf + sizeof(pbuf);
	char *buf;
	packet.m_nChannel = 0x02; /* control channel (ping) */
	packet.m_headerType = RTMP_PACKET_SIZE_LARGE;//RTMP_PACKET_SIZE_MEDIUM;//RTMP_PACKET_SIZE_MEDIUM;
	packet.m_packetType = RTMP_PACKET_TYPE_CONTROL;
	packet.m_nTimeStamp = 0; /* RTMP_GetTime(); */
	packet.m_nInfoField2 = 0;
	packet.m_hasAbsTimestamp = 0;
	packet.m_body = pbuf + RTMP_MAX_HEADER_SIZE;
	packet.m_nBodySize = 6;
	buf = packet.m_body;
	buf = AMF_EncodeInt16(buf, pend, nType);
	memcpy(packet.m_body + 2, p, 4);

	return yangSendPacket(r,&packet, FALSE);
}
int32_t SendCheckBW(RTMP* r) {
	RTMPPacket packet;
	char pbuf[256], *pend = pbuf + sizeof(pbuf);
	char *enc;

	packet.m_nChannel = 0x03; /* control channel (invoke) */
	packet.m_headerType = RTMP_PACKET_SIZE_LARGE;
	packet.m_packetType = RTMP_PACKET_TYPE_INVOKE;
	packet.m_nTimeStamp = 0; /* RTMP_GetTime(); */
	packet.m_nInfoField2 = 0;
	packet.m_hasAbsTimestamp = 0;
	packet.m_body = pbuf + RTMP_MAX_HEADER_SIZE;

	enc = packet.m_body;
	enc = AMF_EncodeString(enc, pend, &av__checkbw);
	enc = AMF_EncodeNumber(enc, pend, ++r->m_numInvokes);
	*enc++ = AMF_NULL;

	packet.m_nBodySize = enc - packet.m_body;

	/* triggers _onbwcheck and eventually results in _onbwdone */
	return sendPacket(r,&packet, FALSE);
}

int32_t SendCheckBWResult(RTMP* r,double txn) {
	RTMPPacket packet;
	char pbuf[256], *pend = pbuf + sizeof(pbuf);
	char *enc;

	packet.m_nChannel = 0x03; /* control channel (invoke) */
	packet.m_headerType = RTMP_PACKET_SIZE_LARGE;	//RTMP_PACKET_SIZE_MEDIUM;
	packet.m_packetType = RTMP_PACKET_TYPE_INVOKE;
	packet.m_nTimeStamp = 0x16 * r->m_nBWCheckCounter; /* temp inc value. till we figure it out. */
	packet.m_nInfoField2 = 0;
	packet.m_hasAbsTimestamp = 0;
	packet.m_body = pbuf + RTMP_MAX_HEADER_SIZE;

	enc = packet.m_body;
	enc = AMF_EncodeString(enc, pend, &av__result);
	enc = AMF_EncodeNumber(enc, pend, txn);
	*enc++ = AMF_NULL;
	enc = AMF_EncodeNumber(enc, pend, (double) r->m_nBWCheckCounter++);

	packet.m_nBodySize = enc - packet.m_body;

	return sendPacket(r,&packet, FALSE);
}
int32_t SendPong(RTMP* r,double txn) {
	RTMPPacket packet;
	char pbuf[256], *pend = pbuf + sizeof(pbuf);
	char *enc;

	packet.m_nChannel = 0x03; /* control channel (invoke) */
	packet.m_headerType = RTMP_PACKET_SIZE_LARGE;	//RTMP_PACKET_SIZE_MEDIUM;
	packet.m_packetType = RTMP_PACKET_TYPE_INVOKE;
	packet.m_nTimeStamp = 0x16 * r->m_nBWCheckCounter; /* temp inc value. till we figure it out. */
	packet.m_nInfoField2 = 0;
	packet.m_hasAbsTimestamp = 0;
	packet.m_body = pbuf + RTMP_MAX_HEADER_SIZE;

	enc = packet.m_body;
	enc = AMF_EncodeString(enc, pend, &av_pong);
	enc = AMF_EncodeNumber(enc, pend, txn);
	*enc++ = AMF_NULL;

	packet.m_nBodySize = enc - packet.m_body;

	return sendPacket(r,&packet, FALSE);
}

int32_t RTMP_SendSeek(RTMP* r,int32_t iTime) {
	RTMPPacket packet;
	char pbuf[256], *pend = pbuf + sizeof(pbuf);
	char *enc;

	packet.m_nChannel = 0x08; /* video channel */
	packet.m_headerType = RTMP_PACKET_SIZE_MEDIUM;
	packet.m_packetType = RTMP_PACKET_TYPE_INVOKE;
	packet.m_nTimeStamp = 0;
	packet.m_nInfoField2 = 0;
	packet.m_hasAbsTimestamp = 0;
	packet.m_body = pbuf + RTMP_MAX_HEADER_SIZE;

	enc = packet.m_body;
	enc = AMF_EncodeString(enc, pend, &av_seek);
	enc = AMF_EncodeNumber(enc, pend, ++r->m_numInvokes);
	*enc++ = AMF_NULL;
	enc = AMF_EncodeNumber(enc, pend, (double) iTime);

	packet.m_nBodySize = enc - packet.m_body;

	r->m_read.flags |= RTMP_READ_SEEKING;
	r->m_read.nResumeTS = 0;

	return sendPacket(r,&packet, TRUE);
}

int32_t RTMP_SendServerBW(RTMP* r) {
	RTMPPacket packet;
	char pbuf[256], *pend = pbuf + sizeof(pbuf);

	packet.m_nChannel = 0x02; /* control channel (invoke) */
	packet.m_headerType = RTMP_PACKET_SIZE_LARGE;
	packet.m_packetType = RTMP_PACKET_TYPE_SERVER_BW;
	packet.m_nTimeStamp = 0;
	packet.m_nInfoField2 = 0;
	packet.m_hasAbsTimestamp = 0;
	packet.m_body = pbuf + RTMP_MAX_HEADER_SIZE;

	packet.m_nBodySize = 4;

	AMF_EncodeInt32(packet.m_body, pend, r->m_nServerBW);
	return sendPacket(r,&packet, FALSE);
}

int32_t RTMP_SendClientBW(RTMP* r) {
	RTMPPacket packet;
	char pbuf[256], *pend = pbuf + sizeof(pbuf);

	packet.m_nChannel = 0x02; /* control channel (invoke) */
	packet.m_headerType = RTMP_PACKET_SIZE_LARGE;
	packet.m_packetType = RTMP_PACKET_TYPE_CLIENT_BW;
	packet.m_nTimeStamp = 0;
	packet.m_nInfoField2 = 0;
	packet.m_hasAbsTimestamp = 0;
	packet.m_body = pbuf + RTMP_MAX_HEADER_SIZE;

	packet.m_nBodySize = 5;

	AMF_EncodeInt32(packet.m_body, pend, r->m_nClientBW);
	packet.m_body[4] = r->m_nClientBW2;
	return sendPacket(r,&packet, FALSE);
}

int32_t SendDeleteStream(RTMP* r,double dStreamId) {
	RTMPPacket packet;
	char pbuf[256], *pend = pbuf + sizeof(pbuf);
	char *enc;

	packet.m_nChannel = 0x03; /* control channel (invoke) */
	packet.m_headerType = RTMP_PACKET_SIZE_LARGE;	//RTMP_PACKET_SIZE_MEDIUM;
	packet.m_packetType = RTMP_PACKET_TYPE_INVOKE;
	packet.m_nTimeStamp = 0;
	packet.m_nInfoField2 = 0;
	packet.m_hasAbsTimestamp = 0;
	packet.m_body = pbuf + RTMP_MAX_HEADER_SIZE;

	enc = packet.m_body;
	enc = AMF_EncodeString(enc, pend, &av_deleteStream);
	enc = AMF_EncodeNumber(enc, pend, ++r->m_numInvokes);
	*enc++ = AMF_NULL;
	enc = AMF_EncodeNumber(enc, pend, dStreamId);

	packet.m_nBodySize = enc - packet.m_body;

	/* no response expected */
	return sendPacket(r,&packet, FALSE);
}
int32_t SendCloseStream(RTMP* r,double dStreamId) {
	RTMPPacket packet;
	char pbuf[256], *pend = pbuf + sizeof(pbuf);
	char *enc;

	packet.m_nChannel = 0x03; /* control channel (invoke) */
	packet.m_headerType = RTMP_PACKET_SIZE_LARGE;	//RTMP_PACKET_SIZE_MEDIUM;
	packet.m_packetType = RTMP_PACKET_TYPE_INVOKE;
	packet.m_nTimeStamp = 0;
	packet.m_nInfoField2 = 0;
	packet.m_hasAbsTimestamp = 0;
	packet.m_body = pbuf + RTMP_MAX_HEADER_SIZE;

	enc = packet.m_body;
	enc = AMF_EncodeString(enc, pend, &av_closeStream);
	enc = AMF_EncodeNumber(enc, pend, ++r->m_numInvokes);
	*enc++ = AMF_NULL;
	enc = AMF_EncodeNumber(enc, pend, dStreamId);

	packet.m_nBodySize = enc - packet.m_body;

	/* no response expected */
	return sendPacket(r,&packet, FALSE);
}
int32_t SendReleaseStream(RTMP* r) {

	RTMPPacket packet;
	char pbuf[1024], *pend = pbuf + sizeof(pbuf);
	char *enc;

	packet.m_nChannel = 0x03; /* control channel (invoke) */
	packet.m_headerType = RTMP_PACKET_SIZE_LARGE;	//RTMP_PACKET_SIZE_MEDIUM;
	packet.m_packetType = RTMP_PACKET_TYPE_INVOKE;
	packet.m_nTimeStamp = 0;
	packet.m_nInfoField2 = 0;
	packet.m_hasAbsTimestamp = 0;
	packet.m_body = pbuf + RTMP_MAX_HEADER_SIZE;

	enc = packet.m_body;
	enc = AMF_EncodeString(enc, pend, &av_releaseStream);
	enc = AMF_EncodeNumber(enc, pend, ++r->m_numInvokes);
	*enc++ = AMF_NULL;
	enc = AMF_EncodeString(enc, pend, getPlayPath(r));
	if (!enc)
		return FALSE;

	packet.m_nBodySize = enc - packet.m_body;

	return sendPacket(r,&packet, FALSE);
}

/* Read from the stream until we get a media packet.
 * Returns -3 if Play.Close/Stop, -2 if fatal error, -1 if no more media
 * packets, 0 if ignorable error, >0 if there is a media packet
 */

int32_t sendPacket(RTMP* r,RTMPPacket *packet, int32_t queue) {
	const RTMPPacket *prevPacket;
	uint32_t last = 0;
	int32_t nSize;
	int32_t hSize, cSize;
	char *header, *hptr, *hend, hbuf[RTMP_MAX_HEADER_SIZE], c;
	uint32_t t;
	char *buffer;	//, *tbuf = NULL, *toff = NULL;
	int32_t nChunkSize;

	// @remark debug info by http://github.com/ossrs/srs
	if (packet->m_packetType == 8 || packet->m_packetType == 9) {

	}

	if (packet->m_nChannel >= r->m_channelsAllocatedOut) {
		int32_t n = packet->m_nChannel + 10;
		RTMPPacket **packets = (RTMPPacket**) realloc(r->m_vecChannelsOut,
				sizeof(RTMPPacket*) * n);
		if (!packets) {
			free(r->m_vecChannelsOut);
			r->m_vecChannelsOut = NULL;
			r->m_channelsAllocatedOut = 0;
			return FALSE;
		}
		r->m_vecChannelsOut = packets;
		memset(r->m_vecChannelsOut + r->m_channelsAllocatedOut, 0,
				sizeof(RTMPPacket*) * (n - r->m_channelsAllocatedOut));
		r->m_channelsAllocatedOut = n;
	}

	prevPacket = r->m_vecChannelsOut[packet->m_nChannel];
	if (prevPacket && packet->m_headerType != RTMP_PACKET_SIZE_LARGE) {

		if (prevPacket->m_nBodySize
				== packet->m_nBodySize&& prevPacket->m_packetType == packet->m_packetType
				&& packet->m_headerType == RTMP_PACKET_SIZE_MEDIUM)
			packet->m_headerType = RTMP_PACKET_SIZE_SMALL;

		if (prevPacket->m_nTimeStamp
				== packet->m_nTimeStamp&& packet->m_headerType == RTMP_PACKET_SIZE_SMALL)
			packet->m_headerType = RTMP_PACKET_SIZE_MINIMUM;
		last = prevPacket->m_nTimeStamp;
	}

	if (packet->m_headerType > 3) {
		yang_error(
				"sanity failed!! trying to send header of type: 0x%02x.",
				(uint8_t) packet->m_headerType);
		return FALSE;
	}

	nSize = packetSize[packet->m_headerType];
	hSize = nSize;
	cSize = 0;
	t = packet->m_nTimeStamp - last;

	if (packet->m_body) {
		header = packet->m_body - nSize;
		hend = packet->m_body;
	} else {
		header = hbuf + 6;
		hend = hbuf + sizeof(hbuf);
	}

	if (packet->m_nChannel > 319)
		cSize = 2;
	else if (packet->m_nChannel > 63)
		cSize = 1;
	if (cSize) {
		header -= cSize;
		hSize += cSize;
	}

	if (t >= 0xffffff) {
		header -= 4;
		hSize += 4;
		yang_warn( "Larger timestamp than 24-bit: 0x%x", t);
	}

	hptr = header;
	c = packet->m_headerType << 6;
	switch (cSize) {
	case 0:
		c |= packet->m_nChannel;
		break;
	case 1:
		break;
	case 2:
		c |= 1;
		break;
	}
	*hptr++ = c;
	if (cSize) {
		int32_t tmp = packet->m_nChannel - 64;
		*hptr++ = tmp & 0xff;
		if (cSize == 2)
			*hptr++ = tmp >> 8;
	}

	if (nSize > 1) {
		hptr = AMF_EncodeInt24(hptr, hend, t > 0xffffff ? 0xffffff : t);
	}

	if (nSize > 4) {
		hptr = AMF_EncodeInt24(hptr, hend, packet->m_nBodySize);
		*hptr++ = packet->m_packetType;
	}

	if (nSize > 8)
		hptr += EncodeInt32LE(hptr, packet->m_nInfoField2);

	if (t >= 0xffffff)
		hptr = AMF_EncodeInt32(hptr, hend, t);

	nSize = packet->m_nBodySize;
	buffer = packet->m_body;
	nChunkSize = r->m_outChunkSize;

	//yang_debug( "%s: fd=%d, size=%d", __FUNCTION__,	r->m_sb.sb_socket, nSize);

	while (nSize + hSize) {
		int32_t wrote;

		if (nSize < nChunkSize)
			nChunkSize = nSize;


		wrote = WriteN(r,header, nChunkSize + hSize);
		if (!wrote)
			return FALSE;
		nSize -= nChunkSize;
		buffer += nChunkSize;
		hSize = 0;
		if (nSize > 0) {
			header = buffer - 1;
			hSize = 1;
			if (cSize) {
				header -= cSize;
				hSize += cSize;
			}
			if (t >= 0xffffff) {
				header -= 4;
				hSize += 4;
			}
			*header = (0xc0 | c);
			if (cSize) {
				int32_t tmp = packet->m_nChannel - 64;
				header[1] = tmp & 0xff;
				if (cSize == 2)
					header[2] = tmp >> 8;
			}
			if (t >= 0xffffff) {
				char *extendedTimestamp = header + 1 + cSize;
				AMF_EncodeInt32(extendedTimestamp, extendedTimestamp + 4, t);
			}
		}
	}

	if (packet->m_packetType == RTMP_PACKET_TYPE_INVOKE) {
		AVal method;
		char *ptr;
		ptr = packet->m_body + 1;
		AMF_DecodeString(ptr, &method);
		yang_debug( "Invoking %s", method.av_val);

		if (queue) {
			int32_t txn;
			ptr += 3 + method.av_len;
			txn = (int) AMF_DecodeNumber(ptr);
			AV_queue(&r->m_methodCalls, &r->m_numCalls, &method, txn);
		}
	}

	if (!r->m_vecChannelsOut[packet->m_nChannel])
		r->m_vecChannelsOut[packet->m_nChannel] = (RTMPPacket*) malloc(
				sizeof(RTMPPacket));
	memcpy(r->m_vecChannelsOut[packet->m_nChannel], packet, sizeof(RTMPPacket));
	return TRUE;
}
int32_t yangSendPacket(RTMP* r,RTMPPacket *packet, int32_t queue) {
	const RTMPPacket *prevPacket;
	uint32_t last = 0;
	int32_t nSize;
	int32_t hSize, cSize;
	char *header, *hptr, *hend, hbuf[RTMP_MAX_HEADER_SIZE], c;
	uint32_t t;
	char *buffer;	//, *tbuf = NULL, *toff = NULL;
	int32_t nChunkSize;


	// @remark debug info by http://github.com/ossrs/srs
	if (packet->m_packetType == 8 || packet->m_packetType == 9) {
		//_srs_state = 3;
	}

	if (packet->m_nChannel >= r->m_channelsAllocatedOut) {
		int32_t n = packet->m_nChannel + 10;
		RTMPPacket **packets = (RTMPPacket**) realloc(r->m_vecChannelsOut,
				sizeof(RTMPPacket*) * n);
		if (!packets) {
			free(r->m_vecChannelsOut);
			r->m_vecChannelsOut = NULL;
			r->m_channelsAllocatedOut = 0;
			return FALSE;
		}
		r->m_vecChannelsOut = packets;
		memset(r->m_vecChannelsOut + r->m_channelsAllocatedOut, 0,
				sizeof(RTMPPacket*) * (n - r->m_channelsAllocatedOut));
		r->m_channelsAllocatedOut = n;
	}

	prevPacket = r->m_vecChannelsOut[packet->m_nChannel];
	if (prevPacket && packet->m_headerType != RTMP_PACKET_SIZE_LARGE) {

		last = prevPacket->m_nTimeStamp;
	}

	if (packet->m_headerType > 3) {
		yang_error(
				"sanity failed!! trying to send header of type: 0x%02x.",
				(uint8_t) packet->m_headerType);
		return FALSE;
	}

	nSize = packetSize[packet->m_headerType];
	hSize = nSize;
	cSize = 0;
	t = packet->m_nTimeStamp - last;

	if (packet->m_body) {
		header = packet->m_body - nSize;
		hend = packet->m_body;
	} else {
		header = hbuf + 6;
		hend = hbuf + sizeof(hbuf);
	}

	if (packet->m_nChannel > 319)
		cSize = 2;
	else if (packet->m_nChannel > 63)
		cSize = 1;
	if (cSize) {
		header -= cSize;
		hSize += cSize;
	}

	if (t >= 0xffffff) {
		header -= 4;
		hSize += 4;
		yang_warn( "Larger timestamp than 24-bit: 0x%x", t);
	}

	hptr = header;
	c = packet->m_headerType << 6;
	switch (cSize) {
	case 0:
		c |= packet->m_nChannel;
		break;
	case 1:
		break;
	case 2:
		c |= 1;
		break;
	}
	*hptr++ = c;
	if (cSize) {
		int32_t tmp = packet->m_nChannel - 64;
		*hptr++ = tmp & 0xff;
		if (cSize == 2)
			*hptr++ = tmp >> 8;
	}

	if (nSize > 1) {
		hptr = AMF_EncodeInt24(hptr, hend, t > 0xffffff ? 0xffffff : t);
	}

	if (nSize > 4) {
		hptr = AMF_EncodeInt24(hptr, hend, packet->m_nBodySize);
		*hptr++ = packet->m_packetType;
	}

	if (nSize > 8)
		hptr += EncodeInt32LE(hptr, packet->m_nInfoField2);

	if (t >= 0xffffff)
		hptr = AMF_EncodeInt32(hptr, hend, t);

	nSize = packet->m_nBodySize;
	buffer = packet->m_body;
	nChunkSize = r->m_outChunkSize;

	yang_debug( "%s: fd=%d, size=%d", __FUNCTION__,
			r->m_sb.sb_socket, nSize);

	while (nSize + hSize) {
		int32_t wrote;

		if (nSize < nChunkSize)
			nChunkSize = nSize;


		wrote = WriteN(r,header, nChunkSize + hSize);
		if (!wrote)
			return FALSE;
		nSize -= nChunkSize;
		buffer += nChunkSize;
		hSize = 0;
		if (nSize > 0) {
			header = buffer - 1;
			hSize = 1;
			if (cSize) {
				header -= cSize;
				hSize += cSize;
			}
			if (t >= 0xffffff) {
				header -= 4;
				hSize += 4;
			}
			*header = (0xc0 | c);
			if (cSize) {
				int32_t tmp = packet->m_nChannel - 64;
				header[1] = tmp & 0xff;
				if (cSize == 2)
					header[2] = tmp >> 8;
			}
			if (t >= 0xffffff) {
				char *extendedTimestamp = header + 1 + cSize;
				AMF_EncodeInt32(extendedTimestamp, extendedTimestamp + 4, t);
			}
		}
	}

	if (packet->m_packetType == RTMP_PACKET_TYPE_INVOKE) {
		AVal method;
		char *ptr;
		ptr = packet->m_body + 1;
		AMF_DecodeString(ptr, &method);
		yang_debug( "Invoking %s", method.av_val);

		if (queue) {
			int32_t txn;
			ptr += 3 + method.av_len;
			txn = (int) AMF_DecodeNumber(ptr);
			AV_queue(&r->m_methodCalls, &r->m_numCalls, &method, txn);
		}
	}

	if (!r->m_vecChannelsOut[packet->m_nChannel])
		r->m_vecChannelsOut[packet->m_nChannel] = (RTMPPacket*) malloc(
				sizeof(RTMPPacket));
	memcpy(r->m_vecChannelsOut[packet->m_nChannel], packet, sizeof(RTMPPacket));
	return TRUE;
}

int32_t RTMP_Serve(RTMP* r) {
	return SHandShake(r);
}

int32_t ReadN(RTMP* r,char *buffer, int32_t n) {
	if(!r) return 0;
	int32_t nOriginalSize = n;
	int32_t avail;
	char *ptr;

	r->m_sb.sb_timedout = FALSE;

#ifdef _DEBUG
	memset(buffer, 0, n);
#endif

	// @remark debug info by http://github.com/ossrs/srs
	//_srs_rbytes += n;

	ptr = buffer;
	while (n > 0) {
		int32_t nBytes = 0, nRead;
        if(!r) return 0;
		avail = r->m_sb.sb_size;
		if (avail == 0) {
                        if (r&&RTMPSockBuf_Fill(&r->m_sb) < 1) {
                                if (r&&!r->m_sb.sb_timedout) {
					//if (m_socketClose)	m_socketClose->socketClose(1);	//(*m_socketClose)(1);
					yang_error(
							"%s, failed to read RTMP packet.timeout,%d",
							__FUNCTION__, r->m_sb.sb_socket);

					return 0;
					//RTMP_Close();
				}
				return 0;
			}
                        if(!r) return 0;
                        avail = r->m_sb.sb_size;
		}
		//}
		nRead = ((n < avail) ? n : avail);
		if (nRead > 0) {
			memcpy(ptr, r->m_sb.sb_start, nRead);
			r->m_sb.sb_start += nRead;
			r->m_sb.sb_size -= nRead;
			nBytes = nRead;
			r->m_nBytesIn += nRead;
			if (r->m_bSendCounter
					&& r->m_nBytesIn
							> (r->m_nBytesInSent + r->m_nClientBW / 10))
				if (!SendBytesReceived(r))
					return FALSE;
		}
		//yang_debug( "%s: %d bytes\n", __FUNCTION__, nBytes);

		if (nBytes == 0) {
			yang_error( "%s, RTMP socket closed by peer",
					__FUNCTION__);
			break;
		}


		n -= nBytes;
		ptr += nBytes;
	}

	return nOriginalSize - n;
}

int32_t WriteN(RTMP* r,const char *buffer, int32_t n) {
	const char *ptr = buffer;
	// @remark debug info by http://github.com/ossrs/srs

	while (n > 0) {
		int32_t nBytes;
		nBytes = RTMPSockBuf_Send(&r->m_sb, ptr, n);

		if (nBytes < 0) {
			int32_t sockerr = GetSockError();
			yang_error( "%s, RTMP send error %d (%d bytes)",
					__FUNCTION__, sockerr, n);

			if (sockerr == EINTR && !RTMP_ctrlC)
				continue;

			n = 1;
			break;
		}

		if (nBytes == 0)
			break;

		n -= nBytes;
		ptr += nBytes;
	}
	return n == 0;
}
int32_t SendUsherToken(RTMP* r,AVal *usherToken) {
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

	yang_debug( "UsherToken: %s", usherToken->av_val);
	enc = packet.m_body;
	enc = AMF_EncodeString(enc, pend, &av_NetStream_Authenticate_UsherToken);
	enc = AMF_EncodeNumber(enc, pend, ++r->m_numInvokes);
	*enc++ = AMF_NULL;
	enc = AMF_EncodeString(enc, pend, usherToken);

	if (!enc)
		return FALSE;

	packet.m_nBodySize = enc - packet.m_body;

	return sendPacket(r,&packet, FALSE);
}

int32_t RTMP_SendCreateStream(RTMP* r) {
	RTMPPacket packet;
	char pbuf[256], *pend = pbuf + sizeof(pbuf);
	char *enc;

	packet.m_nChannel = 0x03; /* control channel (invoke) */
	packet.m_headerType = RTMP_PACKET_SIZE_MEDIUM;
	packet.m_packetType = RTMP_PACKET_TYPE_INVOKE;
	packet.m_nTimeStamp = 0;
	packet.m_nInfoField2 = 0;
	packet.m_hasAbsTimestamp = 0;
	packet.m_body = pbuf + RTMP_MAX_HEADER_SIZE;

	enc = packet.m_body;
	enc = AMF_EncodeString(enc, pend, &av_createStream);
	enc = AMF_EncodeNumber(enc, pend, ++r->m_numInvokes);
	*enc++ = AMF_NULL; /* NULL */

	packet.m_nBodySize = enc - packet.m_body;

	return sendPacket(r,&packet, TRUE);
}

int32_t SendFCSubscribe(RTMP* r,AVal *subscribepath) {
	//printf("\n*******SendFCSubscribe***********\n");
	RTMPPacket packet;
	char pbuf[512], *pend = pbuf + sizeof(pbuf);
	char *enc;
	packet.m_nChannel = 0x03; /* control channel (invoke) */
	packet.m_headerType = RTMP_PACKET_SIZE_MEDIUM;
	packet.m_packetType = RTMP_PACKET_TYPE_INVOKE;
	packet.m_nTimeStamp = 0;
	packet.m_nInfoField2 = 0;
	packet.m_hasAbsTimestamp = 0;
	packet.m_body = pbuf + RTMP_MAX_HEADER_SIZE;

	yang_debug( "FCSubscribe: %s", subscribepath->av_val);
	enc = packet.m_body;
	enc = AMF_EncodeString(enc, pend, &av_FCSubscribe);
	enc = AMF_EncodeNumber(enc, pend, ++r->m_numInvokes);
	*enc++ = AMF_NULL;
	enc = AMF_EncodeString(enc, pend, subscribepath);

	if (!enc)
		return FALSE;

	packet.m_nBodySize = enc - packet.m_body;

	return sendPacket(r,&packet, TRUE);
}

int32_t add_addr_info(RTMP* r,struct sockaddr_in *service, AVal *host,
		int32_t port) {
	char *hostname;
	int32_t ret = TRUE;
	if (host->av_val[host->av_len]) {
		hostname = (char*) malloc(host->av_len + 1);
		memcpy(hostname, host->av_val, host->av_len);
		hostname[host->av_len] = '\0';
	} else {
		hostname = host->av_val;
	}

	service->sin_addr.s_addr = inet_addr(hostname);
	if (service->sin_addr.s_addr == INADDR_NONE) {
		struct hostent *host = gethostbyname(hostname);
		if (host == NULL || host->h_addr== NULL)
		{
			yang_error( "Problem accessing the DNS. (addr: %s)", hostname);
			ret = FALSE;
			goto finish;
		}
		service->sin_addr = *(struct in_addr*) host->h_addr;
	}

	service->sin_port = htons(port);
	finish: if (hostname != host->av_val)
		free(hostname);
	return ret;
}
int32_t SocksNegotiate(RTMP* r) {
	unsigned long addr;
	struct sockaddr_in service;
	memset(&service, 0, sizeof(struct sockaddr_in));

	add_addr_info(r,&service, &r->Link.hostname, r->Link.port);
	addr = htonl(service.sin_addr.s_addr);

	{
		char packet[] = { 4, 1, /* SOCKS 4, connect */
		(r->Link.port >> 8) & 0xFF, (r->Link.port) & 0xFF, (char) (addr >> 24)
				& 0xFF, (char) (addr >> 16) & 0xFF, (char) (addr >> 8) & 0xFF,
				(char) addr & 0xFF, 0 }; /* NULL terminate */

		WriteN(r,packet, sizeof packet);

		if (ReadN(r,packet, 8) != 8)
			return FALSE;

		if (packet[0] == 0 && packet[1] == 90) {
			return TRUE;
		} else {
			yang_error( "%s, SOCKS returned error code %d",
					__FUNCTION__, packet[1]);
			return FALSE;
		}
	}
}

int32_t RTMP_SetOpt(RTMP* r,const AVal *opt, AVal *arg) {
	int32_t i;
	void *v;

	for (i = 0; options[i].name.av_len; i++) {
		if (opt->av_len != options[i].name.av_len)
			continue;
		if (strcasecmp(opt->av_val, options[i].name.av_val))
			continue;
		v = (char*) r + options[i].off;
		switch (options[i].otype) {
		case OPT_STR: {
			AVal *aptr = (AVal*) v;
			*aptr = *arg;
		}
			break;
		case OPT_INT: {
			long l = strtol(arg->av_val, NULL, 0);
			*(int*) v = l;
		}
			break;
		case OPT_BOOL: {
			int32_t j, fl;
			fl = *(int*) v;
			for (j = 0; truth[j].av_len; j++) {
				if (arg->av_len != truth[j].av_len)
					continue;
				if (strcasecmp(arg->av_val, truth[j].av_val))
					continue;
				fl |= options[i].omisc;
				break;
			}
			*(int*) v = fl;
		}
			break;
		case OPT_CONN:
			if (parseAMF(&r->Link.extras, arg, &r->Link.edepth))
				return FALSE;
			break;
		}
		break;
	}
	if (!options[i].name.av_len) {
		yang_error( "Unknown option %s", opt->av_val);
		RTMP_OptUsage();
		return FALSE;
	}

	return TRUE;
}

void HandleCtrl(RTMP* r,const RTMPPacket *packet) {
	short nType = -1;
	uint32_t  tmp;
	if (packet->m_body && packet->m_nBodySize >= 2)
		nType = AMF_DecodeInt16(packet->m_body);


	if (packet->m_nBodySize >= 6) {
		switch (nType) {
		case 0:
			tmp = AMF_DecodeInt32(packet->m_body + 2);
			yang_debug( "%s, Stream Begin %d", __FUNCTION__,
					tmp);
			break;

		case 1:
			tmp = AMF_DecodeInt32(packet->m_body + 2);
			yang_debug( "%s, Stream EOF %d", __FUNCTION__, tmp);
			if (r->m_pausing == 1)
				r->m_pausing = 2;
			break;

		case 2:
			tmp = AMF_DecodeInt32(packet->m_body + 2);
			yang_debug( "%s, Stream Dry %d", __FUNCTION__, tmp);
			break;

		case 4:
			tmp = AMF_DecodeInt32(packet->m_body + 2);
			yang_debug( "%s, Stream IsRecorded %d", __FUNCTION__,
					tmp);
			break;

		case 6: /* server ping. reply with pong. */
			tmp = AMF_DecodeInt32(packet->m_body + 2);

			//RTMP_SendCtrl(0x07, tmp, 0);
			RTMP_SendCtrlPong(r,0x07, packet->m_body + 2, 4);
			//RTMP_SendCtrl(0x07, tmp, tmp);
			break;

			/* FMS 3.5 servers send the following two controls to let the client
			 * know when the server has sent a complete buffer. I.e., when the
			 * server has sent an amount of data equal to m_nBufferMS in duration.
			 * The server meters its output so that data arrives at the client
			 * in realtime and no faster.
			 *
			 * The rtmpdump program tries to set m_nBufferMS as large as
			 * possible, to force the server to send data as fast as possible.
			 * In practice, the server appears to cap this at about 1 hour's
			 * worth of data. After the server has sent a complete buffer, and
			 * sends this BufferEmpty message, it will wait until the play
			 * duration of that buffer has passed before sending a new buffer.
			 * The BufferReady message will be sent when the new buffer starts.
			 * (There is no BufferReady message for the very first buffer;
			 * presumably the Stream Begin message is sufficient for that
			 * purpose.)
			 *
			 * If the network speed is much faster than the data bitrate, then
			 * there may be long delays between the end of one buffer and the
			 * start of the next.
			 *
			 * Since usually the network allows data to be sent at
			 * faster than realtime, and rtmpdump wants to download the data
			 * as fast as possible, we use this RTMP_LF_BUFX hack: when we
			 * get the BufferEmpty message, we send a Pause followed by an
			 * Unpause. This causes the server to send the next buffer immediately
			 * instead of waiting for the full duration to elapse. (That's
			 * also the purpose of the ToggleStream function, which rtmpdump
			 * calls if we get a read timeout.)
			 *
			 * Media player apps don't need this hack since they are just
			 * going to play the data in realtime anyway. It also doesn't work
			 * for live streams since they obviously can only be sent in
			 * realtime. And it's all moot if the network speed is actually
			 * slower than the media bitrate.
			 */
		case 31:
			tmp = AMF_DecodeInt32(packet->m_body + 2);
			yang_debug( "%s, Stream BufferEmpty %d",
					__FUNCTION__, tmp);
			if (!(r->Link.lFlags & RTMP_LF_BUFX))
				break;
			if (!r->m_pausing) {
				r->m_pauseStamp =
						r->m_mediaChannel < r->m_channelsAllocatedIn ?
								r->m_channelTimestamp[r->m_mediaChannel] : 0;
				RTMP_SendPause(r, TRUE, r->m_pauseStamp);
				r->m_pausing = 1;
			} else if (r->m_pausing == 2) {
				RTMP_SendPause( r,FALSE, r->m_pauseStamp);
				r->m_pausing = 3;
			}
			break;

		case 32:
			tmp = AMF_DecodeInt32(packet->m_body + 2);
			yang_debug( "%s, Stream BufferReady %d",
					__FUNCTION__, tmp);
			break;

		default:
			tmp = AMF_DecodeInt32(packet->m_body + 2);
			yang_debug( "%s, Stream xx %d", __FUNCTION__, tmp);
			break;
		}

	}

	if (nType == 0x1A) {
		yang_debug( "%s, SWFVerification ping received: ",
				__FUNCTION__);
		if (packet->m_nBodySize > 2 && packet->m_body[2] > 0x01) {
			yang_error(
					"%s: SWFVerification Type %d request not supported! Patches welcome...",
					__FUNCTION__, packet->m_body[2]);
		}

		yang_error(
				"%s: Ignoring SWFVerification request, no CRYPTO support!",
				__FUNCTION__);
	}
}

void freePacket(RTMPPacket *packet) {
	RTMPPacket_Free(packet);
}

int32_t RTMP_ToggleStream(RTMP* r) {
	int32_t res;

	if (!r->m_pausing) {
		if (RTMP_IsTimedout(r) && r->m_read.status == RTMP_READ_EOF)
			r->m_read.status = 0;

		res = RTMP_SendPause(r, TRUE, r->m_pauseStamp);
		if (!res)
			return res;

		r->m_pausing = 1;
		sleep(1);
	}
	res = RTMP_SendPause(r, FALSE, r->m_pauseStamp);
	r->m_pausing = 3;
	return res;
}

void RTMP_DeleteStream(RTMP* r) {
	if (r->m_stream_id < 0)
		return;

	r->m_bPlaying = FALSE;

	SendDeleteStream(r,r->m_stream_id);
	r->m_stream_id = -1;
}

int32_t readPacket(RTMP* r,RTMPPacket *packet) {
	uint8_t hbuf[RTMP_MAX_HEADER_SIZE] = { 0 };
	char *header = (char*) hbuf;
	int32_t nSize, hSize, nToRead, nChunk;
	//int32_t didAlloc = FALSE;
	int32_t extendedTimestamp;


	if (ReadN(r,(char*) hbuf, 1) == 0) {
		yang_error( "%s, failed to read RTMP packet header",
				__FUNCTION__);
		return FALSE;
	}

	packet->m_headerType = (hbuf[0] & 0xc0) >> 6;
	packet->m_nChannel = (hbuf[0] & 0x3f);
	header++;
	if (packet->m_nChannel == 0) {
		if (ReadN(r,(char*) &hbuf[1], 1) != 1) {
			yang_error(
					"%s, failed to read RTMP packet header 2nd byte",
					__FUNCTION__);
			return FALSE;
		}
		packet->m_nChannel = hbuf[1];
		packet->m_nChannel += 64;
		header++;
	} else if (packet->m_nChannel == 1) {
		int32_t tmp;
		if (ReadN(r,(char*) &hbuf[1], 2) != 2) {
			yang_error(
					"%s, failed to read RTMP packet header 3nd byte",
					__FUNCTION__);
			return FALSE;
		}
		tmp = (hbuf[2] << 8) + hbuf[1];
		packet->m_nChannel = tmp + 64;
		yang_debug( "%s, m_nChannel: %0x", __FUNCTION__,
				packet->m_nChannel);
		header += 2;
	}

	nSize = packetSize[packet->m_headerType];

	if (packet->m_nChannel >= r->m_channelsAllocatedIn) {
		int32_t n = packet->m_nChannel + 10;
		int32_t *timestamp = (int*) realloc(r->m_channelTimestamp, sizeof(int) * n);
		RTMPPacket **packets = (RTMPPacket**) realloc(r->m_vecChannelsIn,
				sizeof(RTMPPacket*) * n);
		if (!timestamp)
			free(r->m_channelTimestamp);
		if (!packets)
			free(r->m_vecChannelsIn);
		r->m_channelTimestamp = timestamp;
		r->m_vecChannelsIn = packets;
		if (!timestamp || !packets) {
			r->m_channelsAllocatedIn = 0;
			return FALSE;
		}
		memset(r->m_channelTimestamp + r->m_channelsAllocatedIn, 0,
				sizeof(int) * (n - r->m_channelsAllocatedIn));
		memset(r->m_vecChannelsIn + r->m_channelsAllocatedIn, 0,
				sizeof(RTMPPacket*) * (n - r->m_channelsAllocatedIn));
		r->m_channelsAllocatedIn = n;
	}

	if (nSize == RTMP_LARGE_HEADER_SIZE)
		packet->m_hasAbsTimestamp = TRUE;

	else if (nSize < RTMP_LARGE_HEADER_SIZE) {
		if (r->m_vecChannelsIn[packet->m_nChannel])
			memcpy(packet, r->m_vecChannelsIn[packet->m_nChannel],	sizeof(RTMPPacket));
	}

	nSize--;

	if (nSize > 0 && ReadN(r,header, nSize) != nSize) {
		yang_error(
				"%s, failed to read RTMP packet header. type: %x", __FUNCTION__,
				(unsigned int) hbuf[0]);
		return FALSE;
	}

	hSize = nSize + (header - (char*) hbuf);

	if (nSize >= 3) {
		packet->m_nTimeStamp = AMF_DecodeInt24(header);


		if (nSize >= 6) {
			packet->m_nBodySize = AMF_DecodeInt24(header + 3);
			packet->m_nBytesRead = 0;

			if (nSize > 6) {
				packet->m_packetType = header[6];

				if (nSize == 11)
					packet->m_nInfoField2 = DecodeInt32LE(header + 7);
			}
		}
	}

	extendedTimestamp = packet->m_nTimeStamp == 0xffffff;
	if (extendedTimestamp) {
		if (ReadN(r,header + nSize, 4) != 4) {
			yang_error( "%s, failed to read extended timestamp",
					__FUNCTION__);
			return FALSE;
		}
		packet->m_nTimeStamp = AMF_DecodeInt32(header + nSize);
		hSize += 4;
	}



	if (packet->m_nBodySize > 0 && packet->m_body == NULL) {
		if (!RTMPPacket_Alloc(packet, packet->m_nBodySize)) {
			yang_debug( "%s, failed to allocate packet",
					__FUNCTION__);
			return FALSE;
		}

		packet->m_headerType = (hbuf[0] & 0xc0) >> 6;
	}

	nToRead = packet->m_nBodySize - packet->m_nBytesRead;
	nChunk = r->m_inChunkSize;
	if (nToRead < nChunk)
		nChunk = nToRead;

	/* Does the caller want the raw chunk? */
	if (packet->m_chunk) {
		packet->m_chunk->c_headerSize = hSize;
		memcpy(packet->m_chunk->c_header, hbuf, hSize);
		packet->m_chunk->c_chunk = packet->m_body + packet->m_nBytesRead;
		packet->m_chunk->c_chunkSize = nChunk;
	}

	if (ReadN(r,packet->m_body + packet->m_nBytesRead, nChunk) != nChunk) {
		yang_error(
				"%s, failed to read RTMP packet body. len: %u", __FUNCTION__,
				packet->m_nBodySize);
		return FALSE;
	}

	// RTMP_LogHexString(RTMP_LOGDEBUG2, (uint8_t *)packet->m_body + packet->m_nBytesRead, nChunk);

	packet->m_nBytesRead += nChunk;

	/* keep the packet as ref for other packets on this channel */
	if (!r->m_vecChannelsIn[packet->m_nChannel])
		r->m_vecChannelsIn[packet->m_nChannel] = (RTMPPacket*) malloc(
				sizeof(RTMPPacket));
	memcpy(r->m_vecChannelsIn[packet->m_nChannel], packet, sizeof(RTMPPacket));
	if (extendedTimestamp) {
		r->m_vecChannelsIn[packet->m_nChannel]->m_nTimeStamp = 0xffffff;
	}

	if (RTMPPacket_IsReady(packet)) {
		/* make packet's timestamp absolute */
		if (!packet->m_hasAbsTimestamp)
			packet->m_nTimeStamp += r->m_channelTimestamp[packet->m_nChannel]; /* timestamps seem to be always relative!! */

		r->m_channelTimestamp[packet->m_nChannel] = packet->m_nTimeStamp;

		/* reset the data from the stored packet. we keep the header since we may use it later if a new packet for this channel */
		/* arrives and requests to re-use some info (small packet header) */
		r->m_vecChannelsIn[packet->m_nChannel]->m_body = NULL;
		r->m_vecChannelsIn[packet->m_nChannel]->m_nBytesRead = 0;
		r->m_vecChannelsIn[packet->m_nChannel]->m_hasAbsTimestamp = FALSE; /* can only be false if we reuse header */
	} else {
		packet->m_body = NULL; /* so it won't be erased on free */
	}

	return TRUE;
}

int32_t yangReadPacket(RTMP* r,RTMPPacket *packet, char *pszbody) {

	uint8_t hbuf[RTMP_MAX_HEADER_SIZE] = { 0 };
	char *header = (char*) hbuf;
	int32_t nSize, hSize, nToRead, nChunk;
	int32_t extendedTimestamp;


	if (ReadN(r,(char*) hbuf, 1) == 0) {
              printf( "failed to read RTMP packet header\n");
		return FALSE;
	}

	packet->m_headerType = (hbuf[0] & 0xc0) >> 6;
	packet->m_nChannel = (hbuf[0] & 0x3f);
	header++;
	if (packet->m_nChannel == 0) {
		if (ReadN(r,(char*) &hbuf[1], 1) != 1) {
			printf("%s, failed to read RTMP packet header 2nd byte\n",__FUNCTION__);
			return FALSE;
		}
		packet->m_nChannel = hbuf[1];
		packet->m_nChannel += 64;
		header++;
	} else if (packet->m_nChannel == 1) {
		int32_t tmp;
		if (ReadN(r,(char*) &hbuf[1], 2) != 2) {
			printf("%s, failed to read RTMP packet header 3nd byte\n",__FUNCTION__);
			return FALSE;
		}
		tmp = (hbuf[2] << 8) + hbuf[1];
		packet->m_nChannel = tmp + 64;
		header += 2;
	}

	nSize = packetSize[packet->m_headerType];

	if (packet->m_nChannel >= r->m_channelsAllocatedIn) {
		int32_t n = packet->m_nChannel + 10;
		int32_t *timestamp = (int*) realloc(r->m_channelTimestamp, sizeof(int) * n);
		RTMPPacket **packets = (RTMPPacket**) realloc(r->m_vecChannelsIn,
				sizeof(RTMPPacket*) * n);
		if (!timestamp)
			free(r->m_channelTimestamp);
		if (!packets)
			free(r->m_vecChannelsIn);
		r->m_channelTimestamp = timestamp;
		r->m_vecChannelsIn = packets;
		if (!timestamp || !packets) {
			r->m_channelsAllocatedIn = 0;
			yang_error("m_channelsAllocatedIn error");
			return FALSE;
		}
		memset(r->m_channelTimestamp + r->m_channelsAllocatedIn, 0,
				sizeof(int) * (n - r->m_channelsAllocatedIn));
		memset(r->m_vecChannelsIn + r->m_channelsAllocatedIn, 0,
				sizeof(RTMPPacket*) * (n - r->m_channelsAllocatedIn));
		r->m_channelsAllocatedIn = n;
	}

	if (nSize == RTMP_LARGE_HEADER_SIZE) /* if we get a full header the timestamp is absolute */
		packet->m_hasAbsTimestamp = TRUE;

	else if (nSize < RTMP_LARGE_HEADER_SIZE) { /* using values from the last message of this channel */
		if (r->m_vecChannelsIn[packet->m_nChannel])
			memcpy(packet, r->m_vecChannelsIn[packet->m_nChannel],
					sizeof(RTMPPacket));
	}

	nSize--;

	if (nSize > 0 && ReadN(r,header, nSize) != nSize) {
		printf("%s, failed to read RTMP packet header. type: %x", __FUNCTION__,	(unsigned int) hbuf[0]);
		yang_error("%s, failed to read RTMP packet header. type: %x", __FUNCTION__,	(unsigned int) hbuf[0]);
		return FALSE;
	}

	hSize = nSize + (header - (char*) hbuf);

	if (nSize >= 3) {
		packet->m_nTimeStamp = AMF_DecodeInt24(header);

		if (nSize >= 6) {
			packet->m_nBodySize = AMF_DecodeInt24(header + 3);
			packet->m_nBytesRead = 0;

			if (nSize > 6) {
				packet->m_packetType = header[6];

				if (nSize == 11)
					packet->m_nInfoField2 = DecodeInt32LE(header + 7);
			}
		}
	}

	extendedTimestamp = packet->m_nTimeStamp == 0xffffff;
	if (extendedTimestamp) {
		if (ReadN(r,header + nSize, 4) != 4) {
			printf( "%s, failed to read extended timestamp",__FUNCTION__);
			return FALSE;
		}
		packet->m_nTimeStamp = AMF_DecodeInt32(header + nSize);
		hSize += 4;
	}

	if (packet->m_nBodySize > 0 && packet->m_body == NULL) {
		if (packet->m_nBodySize > 256000){
			printf("\nread size too big...");
			return 1;
		}
		packet->m_body = pszbody;
		packet->m_headerType = (hbuf[0] & 0xc0) >> 6;
	}

	nToRead = packet->m_nBodySize - packet->m_nBytesRead;
	nChunk = r->m_inChunkSize;
	if (nToRead < nChunk)
		nChunk = nToRead;


	if (packet->m_chunk) {
		packet->m_chunk->c_headerSize = hSize;
		memcpy(packet->m_chunk->c_header, hbuf, hSize);
		packet->m_chunk->c_chunk = packet->m_body + packet->m_nBytesRead;
		packet->m_chunk->c_chunkSize = nChunk;
	}

	if (ReadN(r,packet->m_body + packet->m_nBytesRead, nChunk) != nChunk) {
		printf("%s, failed to read RTMP packet body. len: %u", __FUNCTION__,packet->m_nBodySize);
		return FALSE;
	}


	packet->m_nBytesRead += nChunk;

	/* keep the packet as ref for other packets on this channel */
	if (!r->m_vecChannelsIn[packet->m_nChannel])
		r->m_vecChannelsIn[packet->m_nChannel] = (RTMPPacket*) malloc(
				sizeof(RTMPPacket));
	memcpy(r->m_vecChannelsIn[packet->m_nChannel], packet, sizeof(RTMPPacket));
	if (extendedTimestamp) {
		r->m_vecChannelsIn[packet->m_nChannel]->m_nTimeStamp = 0xffffff;
	}

	if (RTMPPacket_IsReady(packet)) {
		/* make packet's timestamp absolute */
		if (!packet->m_hasAbsTimestamp)
			packet->m_nTimeStamp += r->m_channelTimestamp[packet->m_nChannel]; /* timestamps seem to be always relative!! */

		r->m_channelTimestamp[packet->m_nChannel] = packet->m_nTimeStamp;

		/* reset the data from the stored packet. we keep the header since we may use it later if a new packet for this channel */
		/* arrives and requests to re-use some info (small packet header) */
		r->m_vecChannelsIn[packet->m_nChannel]->m_body = NULL;
		r->m_vecChannelsIn[packet->m_nChannel]->m_nBytesRead = 0;
		r->m_vecChannelsIn[packet->m_nChannel]->m_hasAbsTimestamp = FALSE; /* can only be false if we reuse header */
	} else {
		packet->m_body = NULL; /* so it won't be erased on free */
	}

	return TRUE;
}

int32_t HandShake(RTMP* r,int32_t FP9HandShake) {
	int32_t i;
	uint32_t uptime, suptime;
	int32_t bMatch;
	char type;
	char clientbuf[RTMP_SIG_SIZE + 1], *clientsig = clientbuf + 1;
	char serversig[RTMP_SIG_SIZE];

	clientbuf[0] = 0x03; /* not encrypted */

	uptime = htonl(RTMP_GetTime());
	memcpy(clientsig, &uptime, 4);

	memset(&clientsig[4], 0, 4);

#ifdef _DEBUG
	for (i = 8; i < RTMP_SIG_SIZE; i++)
	clientsig[i] = 0xff;
#else
	for (i = 8; i < RTMP_SIG_SIZE; i++)
		clientsig[i] = (char) (rand() % 256);
#endif

	if (!WriteN(r,clientbuf, RTMP_SIG_SIZE + 1))
		return FALSE;

	if (ReadN(r,&type, 1) != 1) /* 0x03 or 0x06 */
		return FALSE;

	yang_debug( "%s: Type Answer   : %02X", __FUNCTION__, type);

	if (type != clientbuf[0])
		yang_warn(
				"%s: Type mismatch: client sent %d, server answered %d",
				__FUNCTION__, clientbuf[0], type);

	if (ReadN(r,serversig, RTMP_SIG_SIZE) != RTMP_SIG_SIZE)
		return FALSE;

	/* decode server response */

	memcpy(&suptime, serversig, 4);
	suptime = ntohl(suptime);

	yang_debug( "%s: Server Uptime : %d", __FUNCTION__, suptime);
	yang_debug( "%s: FMS Version   : %d.%d.%d.%d", __FUNCTION__,
			serversig[4], serversig[5], serversig[6], serversig[7]);

	/* 2nd part of handshake */
	if (!WriteN(r,serversig, RTMP_SIG_SIZE))
		return FALSE;

	if (ReadN(r,serversig, RTMP_SIG_SIZE) != RTMP_SIG_SIZE)
		return FALSE;

	bMatch = (memcmp(serversig, clientsig, RTMP_SIG_SIZE) == 0);
	if (!bMatch) {
		yang_warn( "%s, client signature does not match!",
				__FUNCTION__);
	}
	return TRUE;
}

int32_t SHandShake(RTMP* r) {
	int32_t i;
	char serverbuf[RTMP_SIG_SIZE + 1], *serversig = serverbuf + 1;
	char clientsig[RTMP_SIG_SIZE];
	uint32_t uptime;
	int32_t bMatch;

	if (ReadN(r,serverbuf, 1) != 1) /* 0x03 or 0x06 */
		return FALSE;

//	yang_debug( "%s: Type Request  : %02X", __FUNCTION__,serverbuf[0]);

	if (serverbuf[0] != 3) {
		yang_error( "%s: Type unknown: client sent %02X",
				__FUNCTION__, serverbuf[0]);
		return FALSE;
	}

	uptime = htonl(RTMP_GetTime());
	memcpy(serversig, &uptime, 4);

	memset(&serversig[4], 0, 4);
#ifdef _DEBUG
	for (i = 8; i < RTMP_SIG_SIZE; i++)
	serversig[i] = 0xff;
#else
	for (i = 8; i < RTMP_SIG_SIZE; i++)
		serversig[i] = (char) (rand() % 256);
#endif

	if (!WriteN(r,serverbuf, RTMP_SIG_SIZE + 1))
		return FALSE;

	if (ReadN(r,clientsig, RTMP_SIG_SIZE) != RTMP_SIG_SIZE)
		return FALSE;

	/* decode client response */

	memcpy(&uptime, clientsig, 4);
	uptime = ntohl(uptime);

//	yang_debug( "%s: Client Uptime : %d", __FUNCTION__, uptime);
	//yang_debug( "%s: Player Version: %d.%d.%d.%d", __FUNCTION__,	clientsig[4], clientsig[5], clientsig[6], clientsig[7]);

	/* 2nd part of handshake */
	if (!WriteN(r,clientsig, RTMP_SIG_SIZE))
		return FALSE;

	if (ReadN(r,clientsig, RTMP_SIG_SIZE) != RTMP_SIG_SIZE)
		return FALSE;

	bMatch = (memcmp(serversig, clientsig, RTMP_SIG_SIZE) == 0);
	if (!bMatch) {
		yang_warn( "%s, client signature does not match!",
				__FUNCTION__);
	}
	return TRUE;
}

int32_t RTMP_SendChunk(RTMP* r,RTMPChunk *chunk) {
	int32_t wrote;
	char hbuf[RTMP_MAX_HEADER_SIZE];

//	yang_debug( "%s: fd=%d, size=%d", __FUNCTION__,			r->m_sb.sb_socket, chunk->c_chunkSize);

	if (chunk->c_chunkSize) {
		char *ptr = chunk->c_chunk - chunk->c_headerSize;

		memcpy(hbuf, ptr, chunk->c_headerSize);
		memcpy(ptr, chunk->c_header, chunk->c_headerSize);
		wrote = WriteN(r,ptr, chunk->c_headerSize + chunk->c_chunkSize);
		memcpy(ptr, hbuf, chunk->c_headerSize);
	} else
		wrote = WriteN(r,chunk->c_header, chunk->c_headerSize);
	return wrote;
}

int32_t RTMP_Write(RTMP* r,const char *buf, int32_t size) {
	RTMPPacket *pkt = &r->m_write;
	char *pend, *enc;
	int32_t s2 = size, ret, num;

	pkt->m_nChannel = 0x04; /* source channel */
	pkt->m_nInfoField2 = r->m_stream_id;

	while (s2) {
		if (!pkt->m_nBytesRead) {
			if (size < 11) {
				/* FLV pkt too small */
				return 0;
			}

			if (buf[0] == 'F' && buf[1] == 'L' && buf[2] == 'V') {
				buf += 13;
				s2 -= 13;
			}

			pkt->m_packetType = *buf++;
			pkt->m_nBodySize = AMF_DecodeInt24(buf);
			buf += 3;
			pkt->m_nTimeStamp = AMF_DecodeInt24(buf);
			buf += 3;
			pkt->m_nTimeStamp |= *buf++ << 24;
			buf += 3;
			s2 -= 11;

			if (((pkt->m_packetType == RTMP_PACKET_TYPE_AUDIO
					|| pkt->m_packetType == RTMP_PACKET_TYPE_VIDEO)
					&& !pkt->m_nTimeStamp)
					|| pkt->m_packetType == RTMP_PACKET_TYPE_INFO) {
				pkt->m_headerType = RTMP_PACKET_SIZE_LARGE;
				if (pkt->m_packetType == RTMP_PACKET_TYPE_INFO)
					pkt->m_nBodySize += 16;
			} else {
				pkt->m_headerType = RTMP_PACKET_SIZE_MEDIUM;
			}

			if (!RTMPPacket_Alloc(pkt, pkt->m_nBodySize)) {
				yang_debug( "%s, failed to allocate packet",
						__FUNCTION__);
				return FALSE;
			}
			enc = pkt->m_body;
			pend = enc + pkt->m_nBodySize;
			if (pkt->m_packetType == RTMP_PACKET_TYPE_INFO) {
				enc = AMF_EncodeString(enc, pend, &av_setDataFrame);
				pkt->m_nBytesRead = enc - pkt->m_body;
			}
		} else {
			enc = pkt->m_body + pkt->m_nBytesRead;
		}
		num = pkt->m_nBodySize - pkt->m_nBytesRead;
		if (num > s2)
			num = s2;
		memcpy(enc, buf, num);
		pkt->m_nBytesRead += num;
		s2 -= num;
		buf += num;
		if (pkt->m_nBytesRead == pkt->m_nBodySize) {
			ret = sendPacket(r,pkt, FALSE);
			RTMPPacket_Free(pkt);
			pkt->m_nBytesRead = 0;
			if (!ret)
				return -1;
			buf += 4;
			s2 -= 4;
			if (s2 < 0)
				break;

		}

	}
	return size + s2;
}

void RTMP_Init(RTMP* r) {

#ifdef _WIN32
	WORD	wVersionRequested;
		WSADATA wsaData;
		//int		err;
		wVersionRequested	=	MAKEWORD(2,2);//create 16bit data
		if(WSAStartup(wVersionRequested,&wsaData)!=0)
		{
			yang_error("Load WinSock Failed!");
			exit(1);
			return;
		}
#endif
	memset(r, 0, sizeof(RTMP));
	r->m_sb.sb_socket = -1;
	r->m_inChunkSize = RTMP_DEFAULT_CHUNKSIZE;
	r->m_outChunkSize = RTMP_DEFAULT_CHUNKSIZE;
	r->m_nBufferMS = 0;
	r->m_nClientBW = 2500000;
	r->m_nClientBW2 = 2;
	r->m_nServerBW = 2500000;
	r->m_fAudioCodecs = 3191.0;
	r->m_fVideoCodecs = 252.0;
	r->Link.timeout = 30;
	r->Link.swfAge = 30;

	memset(&r->netStatus, 0, sizeof(struct YangNetStatus));
	r->m_numInvokes = 0;

}

int32_t initConnect(RTMP* r,RTMPPacket *cp) {
	struct sockaddr_in service;
	if (!r->Link.hostname.av_len)
		return ERROR_SOCKET;

	memset(&service, 0, sizeof(struct sockaddr_in));
	service.sin_family = AF_INET;

	add_addr_info(r,&service, &r->Link.hostname, r->Link.port);
	if (!RTMP_Connect0(r,(struct sockaddr*) &service))
		return ERROR_CONNECT_Fail;

	r->m_bSendCounter = TRUE;

	 if(RTMP_Connect1(r,cp)) return Yang_Ok;
	 return ERROR_CONNECT_Fail;
}

int32_t RTMP_Connect0(RTMP* r,struct sockaddr *service) {
	int32_t on = 1;
	r->m_sb.sb_timedout = FALSE;
	r->m_pausing = 0;
	r->m_fDuration = 0.0;
	r->m_sb.sb_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (r->m_sb.sb_socket != -1) {
		if (connect(r->m_sb.sb_socket, service, sizeof(struct sockaddr)) < 0) {
			int32_t err = GetSockError();
			yang_error( "%s, failed to connect socket. %d (%s)",
					__FUNCTION__, err, strerror(err));

			return FALSE;
		}

		if (r->Link.socksport) {
			yang_debug( "%s ... SOCKS negotiation",
					__FUNCTION__);
			if (!SocksNegotiate(r)) {
				yang_error( "%s, SOCKS negotiation failed.",
						__FUNCTION__);

				//RTMP_Close();
				return FALSE;
			}
		}
	} else {

		return FALSE;
	}

	/* set timeout */
	//{
	SET_RCVTIMEO(tv, r->Link.timeout);
	if (setsockopt(r->m_sb.sb_socket, SOL_SOCKET, SO_RCVTIMEO, (char*) &tv,
			sizeof(tv))) {
		yang_error( "%s, Setting socket timeout to %ds failed!",
				__FUNCTION__, r->Link.timeout);
	}
	//}

	setsockopt(r->m_sb.sb_socket, IPPROTO_TCP, TCP_NODELAY, (char*) &on,
			sizeof(on));

	// @remark debug info by http://github.com/ossrs/srs
	//_srs_state = 2;

	return TRUE;
}

int32_t RTMP_Connect1(RTMP* r,RTMPPacket *cp) {

	//yang_debug( "%s, ... connected, handshaking", __FUNCTION__);
	if (!HandShake( r,TRUE)) {
		yang_error( "%s, handshake failed.", __FUNCTION__);


		return FALSE;
	}
	yang_debug( "%s, handshaked", __FUNCTION__);

	if (!SendConnectPacket(r,cp)) {
		yang_error( "%s, RTMP connect failed.", __FUNCTION__);
;
		return FALSE;
	}
	return TRUE;
}
int32_t SendConnectPacket(RTMP* r,RTMPPacket *cp) {

	RTMPPacket packet;
	char pbuf[4096], *pend = pbuf + sizeof(pbuf);
	char *enc;

	if (cp)
		return sendPacket(r,cp, TRUE);

	packet.m_nChannel = 0x03; /* control channel (invoke) */
	packet.m_headerType = RTMP_PACKET_SIZE_LARGE;
	packet.m_packetType = RTMP_PACKET_TYPE_INVOKE;
	packet.m_nTimeStamp = 0;
	packet.m_nInfoField2 = 0;
	packet.m_hasAbsTimestamp = 0;
	packet.m_body = pbuf + RTMP_MAX_HEADER_SIZE;

	enc = packet.m_body;
	enc = AMF_EncodeString(enc, pend, &av_connect);
	enc = AMF_EncodeNumber(enc, pend, ++r->m_numInvokes);
	*enc++ = AMF_OBJECT;

	enc = AMF_EncodeNamedString(enc, pend, &av_app, &r->Link.app);
	if (!enc)
		return FALSE;
	if (r->m_isPublish) {
		enc = AMF_EncodeNamedString(enc, pend, &av_type, &av_nonprivate);
		if (!enc)
			return FALSE;
	}
	if (r->Link.flashVer.av_len) {
		enc = AMF_EncodeNamedString(enc, pend, &av_flashVer, &r->Link.flashVer);
		if (!enc)
			return FALSE;
	}
	if (r->Link.swfUrl.av_len) {
		enc = AMF_EncodeNamedString(enc, pend, &av_swfUrl, &r->Link.swfUrl);
		if (!enc)
			return FALSE;
	}
	if (r->Link.tcUrl.av_len) {
		enc = AMF_EncodeNamedString(enc, pend, &av_tcUrl, &r->Link.tcUrl);
		if (!enc)
			return FALSE;
	}
	if (!(r->m_isPublish)) {
		enc = AMF_EncodeNamedBoolean(enc, pend, &av_fpad, FALSE);
		if (!enc)
			return FALSE;
		enc = AMF_EncodeNamedNumber(enc, pend, &av_capabilities, 15.0);
		if (!enc)
			return FALSE;
		enc = AMF_EncodeNamedNumber(enc, pend, &av_audioCodecs,
				r->m_fAudioCodecs);
		if (!enc)
			return FALSE;
		enc = AMF_EncodeNamedNumber(enc, pend, &av_videoCodecs,
				r->m_fVideoCodecs);
		if (!enc)
			return FALSE;
		enc = AMF_EncodeNamedNumber(enc, pend, &av_videoFunction, 1.0);
		if (!enc)
			return FALSE;
		if (r->Link.pageUrl.av_len) {
			enc = AMF_EncodeNamedString(enc, pend, &av_pageUrl,
					&r->Link.pageUrl);
			if (!enc)
				return FALSE;
		}
	}
	if (r->m_fEncoding != 0.0 || r->m_bSendEncoding) { /* AMF0, AMF3 not fully supported yet */
		enc = AMF_EncodeNamedNumber(enc, pend, &av_objectEncoding,
				r->m_fEncoding);
		if (!enc)
			return FALSE;
	}
	if (enc + 3 >= pend)
		return FALSE;
	*enc++ = 0;
	*enc++ = 0; /* end of object - 0x00 0x00 0x09 */
	*enc++ = AMF_OBJECT_END;

	/* add auth string */
	if (r->Link.auth.av_len) {
		enc = AMF_EncodeBoolean(enc, pend, r->Link.lFlags & RTMP_LF_AUTH);
		if (!enc)
			return FALSE;
		enc = AMF_EncodeString(enc, pend, &r->Link.auth);
		if (!enc)
			return FALSE;
	}
	if (r->Link.extras.o_num) {
		int32_t i;
		for (i = 0; i < r->Link.extras.o_num; i++) {
			enc = AMFProp_Encode(&r->Link.extras.o_props[i], enc, pend);
			if (!enc)
				return FALSE;
		}
	}
	packet.m_nBodySize = enc - packet.m_body;

	return sendPacket(r,&packet, TRUE);
}

int32_t clientPacket(RTMP* r,RTMPPacket *packet) {

	int32_t bHasMediaPacket = 0;
	switch (packet->m_packetType) {
	case RTMP_PACKET_TYPE_CHUNK_SIZE:
		/* chunk size */
		HandleChangeChunkSize(r, packet);
		break;

	case RTMP_PACKET_TYPE_BYTES_READ_REPORT:
		/* bytes read report */
		yang_debug( "%s, received: bytes read report",
				__FUNCTION__);
		break;

	case RTMP_PACKET_TYPE_CONTROL:
		/* ctrl */
		HandleCtrl(r,packet);
		break;

	case RTMP_PACKET_TYPE_SERVER_BW:
		/* server bw */
		HandleServerBW( r,packet);
		break;

	case RTMP_PACKET_TYPE_CLIENT_BW:
		/* client bw */
		HandleClientBW(r,packet);
		break;

	case RTMP_PACKET_TYPE_AUDIO:
		/* audio data */

		HandleAudio(r, packet);
		bHasMediaPacket = 1;
		if (!r->m_mediaChannel)
			r->m_mediaChannel = packet->m_nChannel;
		if (!r->m_pausing)
			r->m_mediaStamp = packet->m_nTimeStamp;
		break;

	case RTMP_PACKET_TYPE_VIDEO:
		/* video data */

		HandleVideo(r, packet);
		bHasMediaPacket = 1;
		if (!r->m_mediaChannel)
			r->m_mediaChannel = packet->m_nChannel;
		if (!r->m_pausing)
			r->m_mediaStamp = packet->m_nTimeStamp;
		break;

	case RTMP_PACKET_TYPE_FLEX_STREAM_SEND:
		/* flex stream send */
		yang_debug(
				"%s, flex stream send, size %u bytes, not supported, ignoring",
				__FUNCTION__, packet->m_nBodySize);
		break;

	case RTMP_PACKET_TYPE_FLEX_SHARED_OBJECT:
		/* flex shared object */
		yang_error(
				"%s, flex shared object, size %u bytes, not supported, ignoring",
				__FUNCTION__, packet->m_nBodySize);
		break;

	case RTMP_PACKET_TYPE_FLEX_MESSAGE:
		/* flex message */
	{
		yang_debug(	"%s, flex message, size %u bytes, not fully supported",
				__FUNCTION__, packet->m_nBodySize);


		if (HandleInvoke(r,packet->m_body + 1, packet->m_nBodySize - 1) == 1)
			bHasMediaPacket = 2;
		break;
	}
	case RTMP_PACKET_TYPE_INFO:
		/* metadata (notify) */
		yang_debug( "%s, received: notify %u bytes",
				__FUNCTION__, packet->m_nBodySize);
		if (HandleMetadata(r,packet->m_body, packet->m_nBodySize))
			bHasMediaPacket = 1;
		break;

	case RTMP_PACKET_TYPE_SHARED_OBJECT:
		yang_debug( "%s, shared object", __FUNCTION__);

		break;

	case RTMP_PACKET_TYPE_INVOKE:
		/* invoke */
		yang_debug( "%s, received: invoke %u bytes",
				__FUNCTION__, packet->m_nBodySize);


		// @remark debug info by http://github.com/ossrs/srs
		while (1) {
			// String(_result)
			char *p = packet->m_body;
			int32_t nb = packet->m_nBodySize;
			// Marker.
			if (nb < 1) {
				yang_error( "ignore string marker for nb=%d",
						nb);
				break;
			}
			AMFDataType t = (AMFDataType) p[0];
			if (t != AMF_STRING) {
				yang_error("ignore string marker for type=%d,len=%d,header=%d",
						t, nb, packet->m_headerType);

				break;
			}
			nb--;
			p++;
			// String content.
			if (nb < 2) {
				yang_error( "ignore string data for nb=%d", nb);
				break;
			}
			AVal _result;
			AMF_DecodeString(p, &_result);
			nb -= (int) _result.av_len + 2;
			p += (int) _result.av_len + 2;

			// Number(0.0)
			// Marker
			if (nb < 1) {
				yang_error( "ignore number marker for nb=%d",
						nb);
				break;
			}
			t = (AMFDataType) p[0];
			if (t != AMF_NUMBER) {
				yang_error( "ignore number marker for type=%d",
						t);
				break;
			}
			nb--;
			p++;
			// Number content.
			if (nb < 8) {
				yang_error( "ignore number data for nb=%d", nb);
				break;
			}
			double tid = AMF_DecodeNumber(p);
			(void) tid;
			nb -= 8;
			p += 8;

			// Object
			// Marker
			if (nb < 1) {
				yang_error( "ignore object marker for nb=%d",
						nb);
				break;
			}
			t = (AMFDataType) p[0];
			if (t != AMF_OBJECT) {
				//yang_error( "ignore object marker for type=%d", t);
				break;
			}
			nb--;
			p++;
			// Object info content
			AMFObject obj;
			if (nb < 3) {
				yang_error( "ignore object eof for nb=%d", nb);
				break;
			}
			int32_t nRes = -1;
			if ((nRes = AMF_Decode(&obj, p, nb, TRUE)) < 0) {
				yang_error( "decode object failed, ret=%d",
						nRes);
				break;
			}
			nb -= nRes;
			p += nRes;

			// Object
			// Marker
			if (nb < 1) {
				yang_error( "ignore object marker for nb=%d",
						nb);
				break;
			}
			t = (AMFDataType) p[0];
			if (t != AMF_OBJECT) {
				//yang_error( "ignore object marker for type=%d", t);
				break;
			}
			nb--;
			p++;
			// Object data content
			if (nb < 3) {
				yang_error( "ignore object eof for nb=%d", nb);
				break;
			}
			if ((nRes = AMF_Decode(&obj, p, nb, TRUE)) < 0) {
				yang_error( "decode object failed, ret=%d",
						nRes);
				break;
			}
			nb -= nRes;
			p += nRes;
			// Parse data object.
			int32_t i, j;
			for (i = 0; i < obj.o_num; i++) {
				AMFObjectProperty *prop = &obj.o_props[i];
				if (prop->p_type == AMF_OBJECT
						|| prop->p_type == AMF_ECMA_ARRAY) {
					obj = prop->p_vu.p_object;
					for (j = 0; j < obj.o_num; j++) {
						prop = &obj.o_props[j];
						//if (AVMATCH(&prop->p_name, &_const_srs_server_ip)) {
						//	if (_srs_ip) {	free(_srs_ip);	}
						//	_srs_ip = (char*) malloc(prop->p_vu.p_aval.av_len + 1);
						//	memcpy(_srs_ip, prop->p_vu.p_aval.av_val,prop->p_vu.p_aval.av_len);	_srs_ip[prop->p_vu.p_aval.av_len] = 0;
						//} else if (AVMATCH(&prop->p_name, &_const_srs_pid)) {
						//	_srs_pid = (int) prop->p_vu.p_number;
						//} else if (AVMATCH(&prop->p_name, &_const_srs_cid)) {
						//	_srs_cid = (int) prop->p_vu.p_number;
						//}
					}
					break;
				}
			}

			// Print32_t info.RTMP_LOGDEBUG RTMP_LOGINFO
			//	if (_srs_pid > 0) {
			//		yang_Logs(Yang_LOGINFO, "SRS ip=%s, pid=%d, cid=%d", _srs_ip,	_srs_pid, _srs_cid);
			//	}

			break;
		}

		if (HandleInvoke(r,packet->m_body, packet->m_nBodySize) == 1)
			bHasMediaPacket = 2;
		break;

	case RTMP_PACKET_TYPE_FLASH_VIDEO: {
		/* go through FLV packets and handle metadata packets */
		uint32_t  pos = 0;
		uint32_t nTimeStamp = packet->m_nTimeStamp;

		while (pos + 11 < packet->m_nBodySize) {
			uint32_t dataSize = AMF_DecodeInt24(packet->m_body + pos + 1); /* size without header (11) and prevTagSize (4) */

			if (pos + 11 + dataSize + 4 > packet->m_nBodySize) {
				yang_warn( "Stream corrupt?!");
				break;
			}
			if (packet->m_body[pos] == 0x12) {
				HandleMetadata(r,packet->m_body + pos + 11, dataSize);
			} else if (packet->m_body[pos] == 8 || packet->m_body[pos] == 9) {
				nTimeStamp = AMF_DecodeInt24(packet->m_body + pos + 4);
				nTimeStamp |= (packet->m_body[pos + 7] << 24);
			}
			pos += (11 + dataSize + 4);
		}
		if (!r->m_pausing)
			r->m_mediaStamp = nTimeStamp;

		bHasMediaPacket = 1;
		break;
	}
	default:
		yang_debug( "%s, unknown packet type received: 0x%02x",
				__FUNCTION__, packet->m_packetType);

	}

	return bHasMediaPacket;
}
int32_t clientPacket_conn(RTMP* r,RTMPPacket *packet) {
	int32_t bHasMediaPacket = 0;
	switch (packet->m_packetType) {
	case RTMP_PACKET_TYPE_CHUNK_SIZE:
		/* chunk size */
		HandleChangeChunkSize(r, packet);
		break;

	case RTMP_PACKET_TYPE_BYTES_READ_REPORT:
		/* bytes read report */
		yang_debug( "%s, received: bytes read report",
				__FUNCTION__);
		break;

	case RTMP_PACKET_TYPE_CONTROL:
		/* ctrl */
		HandleCtrl(r,packet);
		break;

	case RTMP_PACKET_TYPE_SERVER_BW:
		/* server bw */
		HandleServerBW(r,packet);
		break;

	case RTMP_PACKET_TYPE_CLIENT_BW:
		/* client bw */
		HandleClientBW( r,packet);
		break;
	case RTMP_PACKET_TYPE_AUDIO:
		/* audio data */

		HandleAudio(r, packet);
		bHasMediaPacket = 1;
		if (!r->m_mediaChannel)
			r->m_mediaChannel = packet->m_nChannel;
		if (!r->m_pausing)
			r->m_mediaStamp = packet->m_nTimeStamp;
		break;

	case RTMP_PACKET_TYPE_VIDEO:
		/* video data */
		HandleVideo(r, packet);
		bHasMediaPacket = 1;
		if (!r->m_mediaChannel)
			r->m_mediaChannel = packet->m_nChannel;
		if (!r->m_pausing)
			r->m_mediaStamp = packet->m_nTimeStamp;
		break;

	case RTMP_PACKET_TYPE_FLEX_STREAM_SEND:
		/* flex stream send */
		yang_debug(
				"%s, flex stream send, size %u bytes, not supported, ignoring",
				__FUNCTION__, packet->m_nBodySize);
		break;

	case RTMP_PACKET_TYPE_FLEX_SHARED_OBJECT:
		/* flex shared object */
		yang_debug(
				"%s, flex shared object, size %u bytes, not supported, ignoring",
				__FUNCTION__, packet->m_nBodySize);
		break;

	case RTMP_PACKET_TYPE_FLEX_MESSAGE:
		/* flex message */
	{
		yang_debug(
				"%s, flex message, size %u bytes, not fully supported",
				__FUNCTION__, packet->m_nBodySize);
		/*RTMP_LogHex(packet.m_body, packet.m_nBodySize); */

		/* some DEBUG code */
#if 0
		RTMP_LIB_AMFObject obj;
		int32_t nRes = obj.Decode(packet.m_body+1, packet.m_nBodySize-1);
		if(nRes < 0) {
			yang_error( "%s, error decoding AMF3 packet", __FUNCTION__);
			/*return; */
		}

		obj.Dump();
#endif

		if (HandleInvoke(r,packet->m_body + 1, packet->m_nBodySize - 1) == 1)
			bHasMediaPacket = 2;
		break;
	}
	case RTMP_PACKET_TYPE_INFO:
		/* metadata (notify) */
		yang_debug( "%s, received: notify %u bytes",
				__FUNCTION__, packet->m_nBodySize);
		if (HandleMetadata( r,packet->m_body, packet->m_nBodySize))
			bHasMediaPacket = 1;
		break;

	case RTMP_PACKET_TYPE_SHARED_OBJECT:
		yang_debug( "%s, shared object", __FUNCTION__);

		break;

	case RTMP_PACKET_TYPE_INVOKE:
		/* invoke */
		yang_debug( "%s, received: invoke %u bytes",
				__FUNCTION__, packet->m_nBodySize);


		// @remark debug info by http://github.com/ossrs/srs
		while (1) {
			// String(_result)
			char *p = packet->m_body;
			int32_t nb = packet->m_nBodySize;
			// Marker.
			if (nb < 1) {
				yang_error( "ignore string marker for nb=%d",
						nb);
				break;
			}
			AMFDataType t = (AMFDataType) p[0];
			if (t != AMF_STRING) {
				yang_error( "ignore string marker for type=%d",
						t);
				break;
			}
			nb--;
			p++;
			// String content.
			if (nb < 2) {
				yang_error( "ignore string data for nb=%d", nb);
				break;
			}
			AVal _result;
			AMF_DecodeString(p, &_result);
			nb -= (int) _result.av_len + 2;
			p += (int) _result.av_len + 2;

			// Number(0.0)
			// Marker
			if (nb < 1) {
				yang_error( "ignore number marker for nb=%d",
						nb);
				break;
			}
			t = (AMFDataType) p[0];
			if (t != AMF_NUMBER) {
				yang_error( "ignore number marker for type=%d",
						t);
				break;
			}
			nb--;
			p++;
			// Number content.
			if (nb < 8) {
				yang_error( "ignore number data for nb=%d", nb);
				break;
			}
			double tid = AMF_DecodeNumber(p);
			(void) tid;
			nb -= 8;
			p += 8;

			// Object
			// Marker
			if (nb < 1) {
				yang_error( "ignore object marker for nb=%d",
						nb);
				break;
			}
			t = (AMFDataType) p[0];
			if (t != AMF_OBJECT) {
				//yang_error( "ignore object marker for type=%d", t);
				break;
			}
			nb--;
			p++;
			// Object info content
			AMFObject obj;
			if (nb < 3) {
				yang_error( "ignore object eof for nb=%d", nb);
				break;
			}
			int32_t nRes = -1;
			if ((nRes = AMF_Decode(&obj, p, nb, TRUE)) < 0) {
				yang_error( "decode object failed, ret=%d",
						nRes);
				break;
			}
			nb -= nRes;
			p += nRes;

			// Object
			// Marker
			if (nb < 1) {
				yang_error( "ignore object marker for nb=%d",
						nb);
				break;
			}
			t = (AMFDataType) p[0];
			if (t != AMF_OBJECT) {
				//yang_error( "ignore object marker for type=%d", t);
				break;
			}
			nb--;
			p++;
			// Object data content
			if (nb < 3) {
				yang_error( "ignore object eof for nb=%d", nb);
				break;
			}
			if ((nRes = AMF_Decode(&obj, p, nb, TRUE)) < 0) {
				yang_error( "decode object failed, ret=%d",
						nRes);
				break;
			}
			nb -= nRes;
			p += nRes;
			// Parse data object.
			int32_t i, j;
			for (i = 0; i < obj.o_num; i++) {
				AMFObjectProperty *prop = &obj.o_props[i];
				if (prop->p_type == AMF_OBJECT
						|| prop->p_type == AMF_ECMA_ARRAY) {
					obj = prop->p_vu.p_object;
					for (j = 0; j < obj.o_num; j++) {
						prop = &obj.o_props[j];
						//	if (AVMATCH(&prop->p_name, &_const_srs_server_ip)) {
						//		if (_srs_ip) {	free(_srs_ip);	}
						//		_srs_ip = (char*) malloc(prop->p_vu.p_aval.av_len + 1);
						//		memcpy(_srs_ip, prop->p_vu.p_aval.av_val,prop->p_vu.p_aval.av_len);
						//		_srs_ip[prop->p_vu.p_aval.av_len] = 0;
						//	} else if (AVMATCH(&prop->p_name, &_const_srs_pid)) {
						//	_srs_pid = (int) prop->p_vu.p_number;
						//} else if (AVMATCH(&prop->p_name, &_const_srs_cid)) {
						//	_srs_cid = (int) prop->p_vu.p_number;
						//	}
					}
					break;
				}
			}

			// Print32_t info.RTMP_LOGDEBUG RTMP_LOGINFO
			//	if (_srs_pid > 0) {
			//	yang_Logs(Yang_LOGINFO, "SRS ip=%s, pid=%d, cid=%d", _srs_ip,	_srs_pid, _srs_cid);
			//	}

			break;
		}

		if (HandleInvoke(r,packet->m_body, packet->m_nBodySize) == 1)
			bHasMediaPacket = 2;
		break;

	case RTMP_PACKET_TYPE_FLASH_VIDEO: {
		/* go through FLV packets and handle metadata packets */
		uint32_t  pos = 0;
		uint32_t nTimeStamp = packet->m_nTimeStamp;

		while (pos + 11 < packet->m_nBodySize) {
			uint32_t dataSize = AMF_DecodeInt24(packet->m_body + pos + 1); /* size without header (11) and prevTagSize (4) */

			if (pos + 11 + dataSize + 4 > packet->m_nBodySize) {
				yang_warn( "Stream corrupt?!");
				break;
			}
			if (packet->m_body[pos] == 0x12) {
				HandleMetadata(r,packet->m_body + pos + 11, dataSize);
			} else if (packet->m_body[pos] == 8 || packet->m_body[pos] == 9) {
				nTimeStamp = AMF_DecodeInt24(packet->m_body + pos + 4);
				nTimeStamp |= (packet->m_body[pos + 7] << 24);
			}
			pos += (11 + dataSize + 4);
		}
		if (!r->m_pausing)
			r->m_mediaStamp = nTimeStamp;

		/* FLV tag(s) */

		bHasMediaPacket = 1;
		break;
	}
	default:
		yang_debug( "%s, unknown packet type received: 0x%02x",
				__FUNCTION__, packet->m_packetType);

	}

	return bHasMediaPacket;
}

int32_t HandleInvoke(RTMP* r,const char *body, uint32_t  nBodySize) {

	AMFObject obj;
	AVal method;
	double txn;
	int32_t ret = 0, nRes;
	if (body[0] != 0x02) /* make sure it is a string method name we start with */
	{
		yang_warn(
				"%s, Sanity failed. no string method in invoke packet",
				__FUNCTION__);
		return 0;
	}

	nRes = AMF_Decode(&obj, body, nBodySize, FALSE);
	if (nRes < 0) {
		yang_error( "%s, error decoding invoke packet",
				__FUNCTION__);
		return 0;
	}

	AMF_Dump(&obj);
	AMFProp_GetString(AMF_GetProp(&obj, NULL, 0), &method);
	txn = AMFProp_GetNumber(AMF_GetProp(&obj, NULL, 1));

	if (AVMATCH(&method, &av__result)) {

		AVal methodInvoked = { 0 };
		int32_t i;
		for (i = 0; i < r->m_numCalls; i++) {
			if (r->m_methodCalls[i].num == (int) txn) {
				methodInvoked = r->m_methodCalls[i].name;
				AV_erase(r->m_methodCalls, &r->m_numCalls, i, FALSE);
				break;
			}
		}
		if (!methodInvoked.av_val) {
			yang_debug("%s, received result id %f without matching request",
					__FUNCTION__, txn);
			goto leave;
		}
		//yang_error( "%s, received result for method call <%s>",	__FUNCTION__, methodInvoked.av_val);
		if (AVMATCH(&methodInvoked, &av_connect)) {
			yang_handleconn(r, &obj);
			if (r->Link.token.av_len) {
				AMFObjectProperty p;
				if (RTMP_FindFirstMatchingProperty(&obj, &av_secureToken, &p)) {
					DecodeTEA(&r->Link.token, &p.p_vu.p_aval);
					SendSecureTokenResponse(r,&p.p_vu.p_aval);
				}
			}
			r->m_bConnected = TRUE;
		} else if (AVMATCH(&methodInvoked, &av_createStream)) {

			r->m_stream_id = (int) AMFProp_GetNumber(AMF_GetProp(&obj, NULL, 3));

			if (r->m_isPublish) {
				SendPublish(r);
				//newPubStreamId = r->m_stream_id;

			} else {
				SendPlay(r);
				RTMP_SendCtrl(r,3, r->m_stream_id, r->m_nBufferMS);
			}
		} else if (AVMATCH(&methodInvoked, &av_FCSubscribe)) {


		} else if (AVMATCH(&methodInvoked,&av_play) || AVMATCH(&methodInvoked, &av_publish)) {

			r->m_bPlaying = TRUE;
			//m_streamState = 0;
		}
		free(methodInvoked.av_val);
	} else if (AVMATCH(&method, &av_onBWDone)) {
		if (!r->m_nBWCheckCounter)
			SendCheckBW(r);
	} else if (AVMATCH(&method, &av_FCSubscribe)) {

	} else if (AVMATCH(&method, &av_onFCUnsubscribe)) {

		ret = 1;
	} else if (AVMATCH(&method, &av_ping)) {
		SendPong(r,txn);
	} else if (AVMATCH(&method, &av__onbwcheck)) {
		SendCheckBWResult(r,txn);
	} else if (AVMATCH(&method, &av__onbwdone)) {
		int32_t i;
		for (i = 0; i < r->m_numCalls; i++)
			if (AVMATCH(&r->m_methodCalls[i].name, &av__checkbw)) {
				AV_erase(r->m_methodCalls, &r->m_numCalls, i, TRUE);
				break;
			}
	} else if (AVMATCH(&method, &av__error)) {
		yang_error( "rtmp server sent error");
	} else if (AVMATCH(&method, &av_close)) {
		yang_error( "rtmp server requested close");
		//handleError(ERROR_REQUEST_Close);
		//RTMP_Close();
	} else if (AVMATCH(&method, &av_onStatus)) {

		AMFObject obj2;
		AVal code, level,description;
		AMFProp_GetObject(AMF_GetProp(&obj, NULL, 3), &obj2);
		AMFProp_GetString(AMF_GetProp(&obj2, &av_code, -1), &code);
		AMFProp_GetString(AMF_GetProp(&obj2, &av_level, -1), &level);
		AMFProp_GetString(AMF_GetProp(&obj2, &av_description, -1), &description);
		if (AVMATCH(&code,
				&av_NetStream_Failed) || AVMATCH(&code, &av_NetStream_Play_Failed)|| AVMATCH(&code, &av_NetStream_Play_StreamNotFound)|| AVMATCH(&code, &av_NetConnection_Connect_InvalidApp)) {
			//m_streamState = 1;
			yang_error(	"av_NetStream_Error:%s,..level==%s,..desc==%s", code.av_val,level.av_val,description.av_val);
		}

		else if (AVMATCH(&code,
				&av_NetStream_Play_Start) || AVMATCH(&code, &av_NetStream_Play_PublishNotify)) {

			int32_t i;
			r->m_bPlaying = TRUE;
			//m_streamState = 0;
			for (i = 0; i < r->m_numCalls; i++) {
				if (AVMATCH(&r->m_methodCalls[i].name, &av_play)) {
					AV_erase(r->m_methodCalls, &r->m_numCalls, i, TRUE);
					break;
				}
			}
		}

		else if (AVMATCH(&code, &av_NetStream_Publish_Start)) {
			int32_t i;
			r->m_bPlaying = TRUE;
			//m_streamState = 0;
			for (i = 0; i < r->m_numCalls; i++) {
				if (AVMATCH(&r->m_methodCalls[i].name, &av_publish)) {
					AV_erase(r->m_methodCalls, &r->m_numCalls, i, TRUE);
					break;
				}
			}
		} else if (AVMATCH(&code, &av_NetStream_Unpublish_Success)) {
;
		}

		else if (AVMATCH(&code, &av_NetStream_Publish_BadName)) {
			//m_streamState = 1;

		}
		/* Return 1 if this is a Play.Complete or Play.Stop */
		else if (AVMATCH(&code,
				&av_NetStream_Play_Complete) || AVMATCH(&code, &av_NetStream_Play_Stop) || AVMATCH(&code, &av_NetStream_Play_UnpublishNotify)) {

			//RTMP_Close();
			ret = 1;
		}

		else if (AVMATCH(&code, &av_NetStream_Seek_Notify)) {
			r->m_read.flags &= ~RTMP_READ_SEEKING;
		}

		else if (AVMATCH(&code, &av_NetStream_Pause_Notify)) {
			if (r->m_pausing == 1 || r->m_pausing == 2) {
				RTMP_SendPause( r,FALSE, r->m_pauseStamp);
				r->m_pausing = 3;
			}
		}
	} else if (AVMATCH(&method, &av_playlist_ready)) {
		int32_t i;
		for (i = 0; i < r->m_numCalls; i++) {
			if (AVMATCH(&r->m_methodCalls[i].name, &av_set_playlist)) {
				AV_erase(r->m_methodCalls, &r->m_numCalls, i, TRUE);
				break;
			}
		}
	} else {

	}
	leave: AMF_Reset(&obj);
	return ret;
}

int32_t connectServer(RTMP* r) {
	RTMPPacket packet = { 0 };
	r->m_mediaChannel = 0;

	while (!r->m_bConnected && RTMP_IsConnected(r) && readPacket(r,&packet)) {
		if (RTMPPacket_IsReady(&packet)) {
			if (!packet.m_nBodySize)
				continue;
			if ((packet.m_packetType == RTMP_PACKET_TYPE_AUDIO)
					|| (packet.m_packetType == RTMP_PACKET_TYPE_VIDEO)
					|| (packet.m_packetType == RTMP_PACKET_TYPE_INFO)) {
				yang_warn("Received FLV packet before play()! Ignoring.");
				RTMPPacket_Free(&packet);
				continue;
			}

			clientPacket_conn(r,&packet);
			RTMPPacket_Free(&packet);
		}
	}

	return r->m_bConnected;
}


void setPublishState(RTMP* r,int32_t p_pubstate) {
	r->m_isPublish = p_pubstate;
	r->Link.protocol |= RTMP_FEATURE_WRITE;
}
int32_t connectRtmpServer(RTMP* r,YangStreamOptType pisPublish,char *serverIp,char *app,int32_t port){
	setPublishState(r,(int)pisPublish);

	if(initConnect(r,NULL)){
		return ERROR_CONNECT_Fail;
	}

	if(!connectServer(r)) return ERROR_CONNECT_Fail;
	return Yang_Ok;
}
int32_t HandleStream(RTMP* r,char *psName,YangStreamOptType pisPublish,int32_t puid) {

	int32_t isPublish=(int)pisPublish;
	r->m_isPublish = isPublish;

	memset(r->Link.pathChar, 0, sizeof(r->Link.pathChar));

	r->Link.curPath.av_len = strlen(psName);
	memcpy(r->Link.pathChar, psName, r->Link.curPath.av_len);



	if (isPublish) {
		SendReleaseStream(r);
		SendFCPublish(r);
	}

	RTMP_SendCreateStream(r);
	if (!isPublish) {
		SendFCSubscribe(r,getPlayPath(r));
	}
	r->m_bPlaying = 0;
	r->m_mediaChannel = 0;

	RTMPPacket packet = { 0 };
	while (!r->m_bPlaying && readPacket(r,&packet)) {
		if (RTMPPacket_IsReady(&packet)) {
			if (!packet.m_nBodySize)
				continue;
			if ((packet.m_packetType == RTMP_PACKET_TYPE_AUDIO)
					|| (packet.m_packetType == RTMP_PACKET_TYPE_VIDEO)
					|| (packet.m_packetType == RTMP_PACKET_TYPE_INFO)) {
				RTMPPacket_Free(&packet);
				continue;
			}
			clientPacket(r,&packet);
			RTMPPacket_Free(&packet);
		}
	}


	return r->m_bPlaying?Yang_Ok:ERROR_RTMP_PlayFailure;
}


int32_t yang_HandleStream(void* context,char *psName,YangStreamOptType pisPublish,int32_t puid){
	RTMP* r=(RTMP*)context;
	return HandleStream(r,psName, pisPublish,puid);
}
void yang_RTMP_Close(void* context){
	RTMP* r=(RTMP*)context;
	RTMP_Close(r);
}
int32_t yang_clientPacket(void* context ,RTMPPacket *packet) {
	RTMP* r=(RTMP*)context;
	return clientPacket(r,packet);
}
int32_t yang_connectRtmpServer(void* context,YangStreamOptType pisPublish,char *serverIp,char *app,int32_t port){
	RTMP* r=(RTMP*)context;
	return connectRtmpServer(r,pisPublish,serverIp,app,port);
}
void yang_initRTMP(void* context,char *serverIp, char *app, int32_t port) {
	RTMP* r=(RTMP*)context;
	return rtmp_initRTMP(r,serverIp,app,port);
}

int32_t yang_sendPacket(void* context,RTMPPacket *packet, int32_t queue){
	RTMP* r=(RTMP*)context;
	return sendPacket(r,packet,queue);
}
int32_t yang_yangReadPacket(void* context,RTMPPacket *packet, char *pszbody) {
	RTMP* r=(RTMP*)context;
	return yangReadPacket(r,packet,pszbody);
}
int32_t yang_rtmp_isConnect(void *context){
	RTMP* r=(RTMP*)context;
	return r->m_bConnected;
}
int32_t yang_rtmp_getStreamId(void* context){
	RTMP* r=(RTMP*)context;
	return r->m_stream_id;
}
void yang_create_rtmp(YangRtmpHandle* rtmp){
	RTMP_ctrlC = 0;
	rtmp->context = calloc(sizeof(RTMP),1);
	rtmp->HandleStream=yang_HandleStream;
	rtmp->RTMP_Close=yang_RTMP_Close;
	rtmp->clientPacket=yang_clientPacket;
	rtmp->connectRtmpServer=yang_connectRtmpServer;
	rtmp->initRTMP=yang_initRTMP;
	rtmp->sendPacket=yang_sendPacket;
	rtmp->yangReadPacket=yang_yangReadPacket;
	rtmp->getStreamId=yang_rtmp_getStreamId;
	rtmp->isConnect=yang_rtmp_isConnect;

}
void yang_destroy_rtmp(YangRtmpHandle* rtmp){
	RTMP* r=(RTMP*)rtmp->context;
	RTMP_Close(r);
    yang_free(rtmp->context);
}


