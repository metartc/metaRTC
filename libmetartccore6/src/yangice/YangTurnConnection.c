//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangice/YangTurnConnection.h>

#include <yangutil/sys/YangEndian.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangSocket.h>
#include <yangutil/sys/YangEndian.h>
#include <yangutil/sys/YangCString.h>
#include <yangrtc/YangRtcConnection.h>

#include <yangssl/YangSsl.h>

#ifdef _WIN32
#include <winsock2.h>
#define be32toh ntohl
#else
#include <arpa/inet.h>
#endif

int32_t yang_turnconn_receive(YangTurnSession *session, char *data, int32_t nb);

yang_vector_impl2(YangTurnPeer)

void yang_turn_addFingerprint(YangTurnSession *session, YangBuffer *stream,
		yangbool addFingerprint) {
	uint32_t ret = 0;
	stream->data[2] = ((yang_buffer_pos(stream) - 20 + 24) & 0x0000FF00) >> 8;
	stream->data[3] = ((yang_buffer_pos(stream) - 20 + 24) & 0x000000FF);

	char keystr[128] = { 0 };
	char key[128] = { 0 };
	snprintf(keystr, sizeof(keystr), "%s:%s:%s", session->server->username,
			session->realm, session->server->password);

	yang_ssl_md5((const uint8_t*) keystr, strlen(keystr), (uint8_t*) key);
	//MessageIntegrity
	char *hmac = NULL;
	ret = session->stun.encode_hmac(stream, key, &hmac);
	yang_write_bytes(stream, hmac, ret);
	yang_free(hmac);

	if (addFingerprint) {
		stream->data[2] = ((yang_buffer_pos(stream) - 20 + 8) & 0x0000FF00)
				>> 8;
		stream->data[3] = ((yang_buffer_pos(stream) - 20 + 8) & 0x000000FF);

		char *fingerprint = NULL;
		ret = session->stun.encode_fingerprint(stream, &fingerprint);
		yang_write_bytes(stream, fingerprint, ret);
		yang_free(fingerprint);
	}
}

int32_t yang_turn_encode_allocate(YangTurnSession *session, uint8_t protocol,
		yangbool beforCredential) {

	uint32_t ret = 0;
	char s[1024] = { 0 };
	YangBuffer stream;
	yang_init_buffer(&stream, s, 1024);

	YangStunPacket packet;
	memset(&packet, 0, sizeof(YangStunPacket));
	packet.message_type = StunAllocate;

	memset(&session->response, 0, sizeof(YangTurnResponseData));

	session->stun.encode_header(&packet, &stream, 0);

	char *transportStr = NULL;
	ret = session->stun.encode_transport(protocol, &transportStr);
	yang_write_bytes(&stream, transportStr, ret);
	yang_free(transportStr);

	if (!beforCredential) {
#if 0
		int32_t lifetime=session->lifetime;
		char* lifetimeStr=NULL;
		ret=session->stun.encode_lifetime(lifetime,&lifetimeStr);
		yang_write_bytes(&stream,lifetimeStr,ret);
		yang_free(lifetimeStr);
#endif

		char *username = NULL;
		ret = session->stun.encode_username(session->server->username,
				&username);
		yang_write_bytes(&stream, username, ret);
		yang_free(username);

		char *realm = NULL;
		ret = session->stun.encode_realm(session->realm, &realm);
		yang_write_bytes(&stream, realm, ret);
		yang_free(realm);

		char *nonce = NULL;
		ret = session->stun.encode_nonce((char*) session->nonce,
				session->nonceLen, &nonce);
		yang_write_bytes(&stream, nonce, ret);
		yang_free(nonce);

		yang_turn_addFingerprint(session, &stream, yangfalse);
	}

	stream.data[2] = ((yang_buffer_pos(&stream) - 20) & 0x0000FF00) >> 8;
	stream.data[3] = ((yang_buffer_pos(&stream) - 20) & 0x000000FF);

	return session->udp->sendData(&session->udp->session, s,
			yang_buffer_pos(&stream));
}

int32_t yang_turn_encode_refresh(YangTurnSession *session, yangbool isClose) {

	uint32_t ret = 0;
	char s[1024] = { 0 };
	YangBuffer stream;
	yang_init_buffer(&stream, s, 1024);

	YangStunPacket packet;
	memset(&packet, 0, sizeof(YangStunPacket));
	packet.message_type = StunRefresh;

	memset(&session->response, 0, sizeof(YangTurnResponseData));

	session->stun.encode_header(&packet, &stream, 0);

	if (isClose) {
		int32_t lifetime = 0;
		char *lifetimeStr = NULL;
		ret = session->stun.encode_lifetime(lifetime, &lifetimeStr);
		yang_write_bytes(&stream, lifetimeStr, ret);
		yang_free(lifetimeStr);
	}

	char *username = NULL;
	ret = session->stun.encode_username(session->server->username, &username);
	yang_write_bytes(&stream, username, ret);
	yang_free(username);

	char *realm = NULL;
	ret = session->stun.encode_realm(session->realm, &realm);
	yang_write_bytes(&stream, realm, ret);
	yang_free(realm);

	char *nonce = NULL;
	ret = session->stun.encode_nonce((char*) session->nonce, session->nonceLen,
			&nonce);
	yang_write_bytes(&stream, nonce, ret);
	yang_free(nonce);

	yang_turn_addFingerprint(session, &stream, yangfalse);

	stream.data[2] = ((yang_buffer_pos(&stream) - 20) & 0x0000FF00) >> 8;
	stream.data[3] = ((yang_buffer_pos(&stream) - 20) & 0x000000FF);

	return session->udp->sendData(&session->udp->session, s,
			yang_buffer_pos(&stream));
}

int32_t yang_turn_encode_permission(YangTurnSession *session,
		YangTurnPeer *peer) {

	uint32_t ret = 0;
	char s[1024] = { 0 };
	YangBuffer stream;
	yang_init_buffer(&stream, s, 1024);

	YangStunPacket packet;
	memset(&packet, 0, sizeof(YangStunPacket));
	packet.message_type = StunPermission;

	memset(&session->response, 0, sizeof(YangTurnResponseData));

	session->stun.encode_header(&packet, &stream, 0);

	char *hostStr = NULL;
	ret = session->stun.encode_peer_address_ipv4(peer->address.mapAddress,
			peer->address.port, &hostStr);
	yang_write_bytes(&stream, hostStr, ret);
	yang_free(hostStr);

	char *username = NULL;
	ret = session->stun.encode_username(session->server->username, &username);
	yang_write_bytes(&stream, username, ret);
	yang_free(username);

	char *realm = NULL;
	ret = session->stun.encode_realm(session->realm, &realm);
	yang_write_bytes(&stream, realm, ret);
	yang_free(realm);

	char *nonce = NULL;
	ret = session->stun.encode_nonce((char*) session->nonce, session->nonceLen,
			&nonce);
	yang_write_bytes(&stream, nonce, ret);
	yang_free(nonce);

	yang_turn_addFingerprint(session, &stream, yangfalse);

	stream.data[2] = ((yang_buffer_pos(&stream) - 20) & 0x0000FF00) >> 8;
	stream.data[3] = ((yang_buffer_pos(&stream) - 20) & 0x000000FF);
	return session->udp->sendData(&session->udp->session, s,
			yang_buffer_pos(&stream));
}

int32_t yang_turn_encode_bindChannel(YangTurnSession *session,
		YangTurnPeer *peer) {

	uint32_t ret = 0;
	char s[1024] = { 0 };
	YangBuffer stream;
	yang_init_buffer(&stream, s, 1024);

	YangStunPacket packet;
	memset(&packet, 0, sizeof(YangStunPacket));
	packet.message_type = StunChannelBindingRequest;

	session->stun.encode_header(&packet, &stream, 0);

	memset(&session->response, 0, sizeof(YangTurnResponseData));

	uint16_t channelNum = peer->channelNumber;
	char *chnnelNumStr = NULL;
	ret = session->stun.encode_channelNumber(channelNum, &chnnelNumStr);
	yang_write_bytes(&stream, chnnelNumStr, ret);
	yang_free(chnnelNumStr);

	char *hostStr = NULL;
	ret = session->stun.encode_peer_address_ipv4(peer->address.mapAddress,
			peer->address.port, &hostStr);
	yang_write_bytes(&stream, hostStr, ret);
	yang_free(hostStr);

	char *username = NULL;
	ret = session->stun.encode_username(session->server->username, &username);
	yang_write_bytes(&stream, username, ret);
	yang_free(username);

	char *realm = NULL;
	ret = session->stun.encode_realm(session->realm, &realm);
	yang_write_bytes(&stream, realm, ret);
	yang_free(realm);

	char *nonce = NULL;
	ret = session->stun.encode_nonce((char*) session->nonce, session->nonceLen,
			&nonce);
	yang_write_bytes(&stream, nonce, ret);
	yang_free(nonce);

	yang_turn_addFingerprint(session, &stream, yangfalse);

	stream.data[2] = ((yang_buffer_pos(&stream) - 20) & 0x0000FF00) >> 8;
	stream.data[3] = ((yang_buffer_pos(&stream) - 20) & 0x000000FF);
	return session->udp->sendData(&session->udp->session, s,
			yang_buffer_pos(&stream));
}

int32_t yang_turn_encode_sendIndication(YangTurnSession *session,
		YangTurnPeer *peer, char *data) {

	uint32_t ret = 0;
	char s[1024] = { 0 };
	YangBuffer stream;
	yang_init_buffer(&stream, s, 1024);

	YangStunPacket packet;
	memset(&packet, 0, sizeof(YangStunPacket));
	packet.message_type = StunSendIndication;

	memset(&session->response, 0, sizeof(YangTurnResponseData));

	session->stun.encode_header(&packet, &stream, 0);

	char *hostStr = NULL;
	ret = session->stun.encode_peer_address_ipv4(peer->address.mapAddress,
			peer->address.port, &hostStr);
	yang_write_bytes(&stream, hostStr, ret);
	yang_free(hostStr);

	char *tdata = NULL;
	ret = session->stun.encode_data(data, &tdata);
	yang_write_bytes(&stream, tdata, ret);
	yang_free(tdata);

	stream.data[2] = ((yang_buffer_pos(&stream) - 20) & 0x0000FF00) >> 8;
	stream.data[3] = ((yang_buffer_pos(&stream) - 20) & 0x000000FF);

	return session->udp->sendData(&session->udp->session, s,
			yang_buffer_pos(&stream));
}

int32_t yang_turn_encode_request(YangTurnSession *session) {
	char tmp[1024] = { 0 };

	YangBuffer stream;
	yang_init_buffer(&stream, tmp, 1024);

	session->stun.encode_request(&stream, session->udp,
			session->server->username, session->server->password);
	return session->udp->sendData(&session->udp->session, stream.data,
			yang_buffer_pos(&stream));

}

int32_t yang_turn_decode(YangTurnSession *session, char *buf,
		const int32_t nb_buf) {
	int32_t err = Yang_Ok;

	YangBuffer stream;
	yang_init_buffer(&stream, buf, nb_buf);

	if (yang_buffer_left(&stream) < 20) {
		return yang_error_wrap(ERROR_RTC_STUN,
				"ERROR_RTC_STUN invalid stun packet, size=%d", stream.size);
	}

	//pkt->message_type = yang_read_2bytes(&stream);
	yang_buffer_skip(&stream, 2);
	uint16_t message_len = yang_read_2bytes(&stream);
	char magic_cookie[4];
	yang_read_bytes(&stream, magic_cookie, 4);

	//yang_read_bytes(&stream,pkt->transcation_id,12);
	yang_buffer_skip(&stream, 12);
	if (nb_buf != 20 + message_len) {
		return yang_error_wrap(ERROR_RTC_STUN,
				"ERROR_RTC_STUN invalid stun packet, message_len=%d, nb_buf=%d",
				message_len, nb_buf);
	}

	while (yang_buffer_left(&stream) >= 4) {
		uint16_t type = yang_read_2bytes(&stream);
		uint16_t len = yang_read_2bytes(&stream);

		if (yang_buffer_left(&stream) < len) {
			return yang_error_wrap(ERROR_RTC_STUN, "invalid stun packet");
		}

		char *val = (char*) calloc(len + 1, 1);
		yang_read_bytes(&stream, val, len);
		// padding
		if (len % 4 != 0) {
			yang_buffer_skip(&stream, 4 - (len % 4));
		}

		switch (type) {
		case StunErrorCode: {
			uint8_t stunClass = *(val + 2);
			uint8_t stunCode = *(val + 3);
			session->response.errorCode = 100 * stunClass + stunCode;
			break;
		}
		case StunXorRelayedAddress: {
			session->response.mappedPort = yang_get_be16((uint8_t*) val + 2)
					^ ((uint16_t) (kStunMagicCookie >> 16));
			session->response.mappedAddress = yang_get_be32(
					(uint8_t*) val + 4)^ kStunMagicCookie;
			break;
		}
		case StunLifetime: {
			session->response.lifetime = yang_get_be32((uint8_t*) val);
			break;
		}
		case StunRealm: {
			if (strlen(session->realm))
				break;
			memset(session->realm, 0, sizeof(session->realm));
			memcpy(session->realm, val, len);
			break;
		}
		case StunNonce: {
			if (session->nonceLen)
				break;
			memset(session->response.nonce, 0, sizeof(session->response.nonce));
			session->response.nonceLen = len;
			memcpy(session->response.nonce, val, len);
			break;
		}
		case StunData: {
			yang_trace("recv turn data=%s,", val);
			break;
		}
		default: {
			break;
		}
		}
		yang_free(val);
	}

	return err;
}

void g_yang_turn_receive_data(char *data, int32_t size, void *user) {
	if (user == NULL)
		return;
	YangTurnSession *session = (YangTurnSession*) user;
	yang_turnconn_receive(session, data, size);
}

int32_t yang_rtc_turn_sendData(YangUdpSession *session, char *data, int32_t nb) {

	if (session == NULL || !session->isStart || session->notRemoteInit
			|| data == NULL)
		return ERROR_RTC_UDP;

	memcpy(session->turnBuffer + 4, data, nb);
	yang_put_be16(session->turnBuffer + 2, nb);
	return sendto(session->fd, session->turnBuffer, nb + 4, 0,
			(struct sockaddr*) &session->remote_addr, sizeof(struct sockaddr))
			> 0 ? Yang_Ok : ERROR_RTC_TURN;
}

int32_t yang_turnconn_sendData(YangTurnSession *psession, int32_t uid,
		char *data, int32_t nb) {
	if (psession == NULL || data == NULL)
		return ERROR_RTC_TURN;

	YangUdpSession *session = &psession->udp->session;

	if (session == NULL || !session->isStart || session->notRemoteInit
			|| psession->state != YangTurnReady){
		return ERROR_RTC_UDP;
	}


	memcpy(session->turnBuffer + 4, data, nb);
	yang_put_be16(session->turnBuffer + 2, nb);
	YangTurnPeer *peer = NULL;
	for (int32_t i = 0; i < psession->peers.vec.vsize; i++) {
		peer = psession->peers.vec.payload[i];
		if (peer && peer->state == YangTurnReady && peer->uid == uid) {

			yang_put_be16(session->turnBuffer, peer->channelNumber);

			return sendto(session->fd, session->turnBuffer, nb + 4, 0,
					(struct sockaddr*) &session->remote_addr,
					sizeof(struct sockaddr)) > 0 ? Yang_Ok : 1;
		}
	}

	return ERROR_RTC_TURN;
}

void yang_turnconn_updateNonce(YangTurnSession *session) {

	if (session->response.nonceLen > 0) {

		memset(session->nonce, 0, sizeof(session->nonce));
		session->nonceLen = session->response.nonceLen;
		memcpy(session->nonce, session->response.nonce,
				session->response.nonceLen);
	}
}

int32_t yang_turnconn_handleError(YangTurnSession *session, char *data,
		int32_t nb) {

	uint16_t stuntype = yang_get_be16((uint8_t*) data);
	switch (stuntype) {
	case StunAllocateErrorResponse:
		if (session->state == YangTurnAllocate)
			yang_error("StunAllocateErrorResponse");
		if (session->state < YangTurnAllocate)
			session->state = YangTurnAllocate;
		break;
	case StunRefreshErrorResponse:
		yang_error("StunRefreshErrorResponse");
		session->errorCode = YangTurnRefreshError;
		break;
	case StunPermissionErrorResponse:
		yang_error("StunPermissionErrorResponse");
		session->errorCode = YangTurnPermissionError;
		break;
	case StunChannelBindErrorResponse:
		yang_error("StunChannelBindErrorResponse");
		session->errorCode = YangTurnBindChannelError;
		break;
	default:
		yang_error("YangTurnKwnownError");
		session->errorCode = YangTurnKwnownError;
		break;
	}

	switch (session->response.errorCode) {
	case Yang_Turn_Error_BadRequest:
		yang_error("Yang_Turn_Error_Bad_Request");
		break;
	case Yang_Turn_Error_Unauthoorized:
		yang_turnconn_updateNonce(session);
		yang_error("Yang_Turn_Error_Unauthoorized");
		break;
	case Yang_Turn_Error_Nonce:
		yang_turnconn_updateNonce(session);
		yang_error("Yang_Turn_Error_Nonce");
		break;
	}

	return Yang_Ok;
}

int32_t yang_turnconn_handleRecv(YangTurnSession *session, char *data,
		int32_t nb) {
	int32_t err = Yang_Ok;
	uint16_t stuntype = yang_get_be16((uint8_t*) data);

	switch (stuntype) {
	case StunDataIndication:
		break;
	case StunBindingSuccessResponse:
		if (session->state == YangTurnNew)
			session->state = YangTurnCredential;
		break;
	case StunAllocateSuccessResponse:
		if (session->state == YangTurnCredential) {
			session->state = YangTurnAllocate;
			break;
		}
		session->server->relayIp = yang_get_be32(
				(uint8_t*) (&session->response.mappedAddress));
		session->server->relayPort = session->response.mappedPort;
		if (session->relayIp == 0)
			session->relayIp = session->server->relayIp;
		if (session->relayPort == 0) {
			session->relayPort = session->response.mappedPort;
			uint8_t *ip = (uint8_t*) &session->relayIp;
			yang_trace(
					"\nturn allocate ip=%u.%u.%u.%u,turn port=%d,lifetime=%d",
					ip[0], ip[1], ip[2], ip[3], session->relayPort,
					session->response.lifetime);
		}
		if (session->response.lifetime > 0)
			session->lifetime = session->response.lifetime;
		session->allocateOuttime = yang_get_system_time() / YANG_UTIME_SECONDS
				+ session->lifetime;
		session->state = YangTurnPermission;
		break;
	case StunRefreshteSuccessResponse:
		session->allocateOuttime = yang_get_system_time() / YANG_UTIME_SECONDS
				+ session->lifetime;
		break;
	case StunPermissionSuccessResponse:
		for (int32_t i = 0; i < session->peers.vec.vsize; i++) {
			if (session->peers.vec.payload[i]->state == YangTurnPermission) {
				session->peers.vec.payload[i]->state = YangTurnBindChannel;
				session->peers.vec.payload[i]->lifetimeOuttime =
						yang_get_system_time() / YANG_UTIME_SECONDS
								+ Yang_Turn_Permission_Lifetime;
			}
		}
		session->permissionCount++;
		if (session->state < YangTurnBindChannel)
			session->state = YangTurnBindChannel;
		break;
	case StunChannelBindSuccessResponse:
		for (int32_t i = 0; i < session->peers.vec.vsize; i++) {
			if (session->peers.vec.payload[i]->state == YangTurnBindChannel) {
				session->peers.vec.payload[i]->state = YangTurnReady;
				session->peers.vec.payload[i]->lifetimeOuttime =
						yang_get_system_time() / YANG_UTIME_SECONDS
								+ Yang_Turn_Permission_Lifetime;

			}
		}
		if (session->state < YangTurnReady)
			session->state = YangTurnReady;
		session->waitTime = 1000;
		break;
	default:
		break;
	}
	return err;
}

int32_t yang_turnconn_receive(YangTurnSession *session, char *data, int32_t nb) {
	int32_t err = Yang_Ok;
	uint16_t stuntype = 0;
	int32_t len = 0;
	uint16_t channelNum = 0;

	if (yang_get_be32((uint8_t*) data + 4) == kStunMagicCookie) { //turn
		memset(&session->response, 0, sizeof(YangTurnResponseData));
		yang_turn_decode(session, data, nb);
		stuntype = yang_get_be16((uint8_t*) data);

		if ((stuntype >= StunBindingErrorResponse
				&& stuntype <= StunRefreshErrorResponse)
				|| (stuntype == StunPermissionErrorResponse
						|| stuntype == StunChannelBindErrorResponse)) {
			return yang_turnconn_handleError(session, data, nb);
		} else {
			return yang_turnconn_handleRecv(session, data, nb);
		}
	} else { //channel data

		channelNum = yang_get_be16((uint8_t*) data);
		len = (int32_t) yang_get_be16((uint8_t*) data + 2);

		YangTurnPeer *peer = NULL;
		for (int32_t i = 0; i < session->peers.vec.vsize; i++) {
			peer = session->peers.vec.payload[i];
			if (peer && peer->state == YangTurnReady
					&& peer->channelNumber == channelNum) {
				peer->receive(peer->receiveUser, data + 4, len);
				return Yang_Ok;
			}
		}

	}

	return err;
}

void yang_turnconn_refreshAllocate(YangTurnSession *session) {

	YangTurnPeer *peer = NULL;
	for (int32_t i = 0; i < session->peers.vec.vsize; i++) {
		peer = session->peers.vec.payload[i];
		if (yang_get_system_time() / YANG_UTIME_SECONDS
				> peer->lifetimeOuttime - 60) {
			peer->state =
					session->permissionCount >= 2 ?
							YangTurnBindChannel : YangTurnPermission; //;
			session->waitTime = 20;
			return;
		}
	}

	if (session->allocateOuttime
			&& yang_get_system_time() / YANG_UTIME_SECONDS
					>= session->allocateOuttime - 60) {

		yang_turn_encode_refresh(session, yangfalse);
		return;
	}

}

int32_t yang_run_turnconn_app(YangTurnSession *session) {
	int32_t err = Yang_Ok;
	session->isLoop = 1;

	while (session->isLoop) {
		yang_usleep(session->waitTime * YANG_UTIME_MILLISECONDS);
		switch (session->state) {
		case YangTurnNew:
			err = yang_turn_encode_request(session);
			break;
		case YangTurnCredential:
			err = yang_turn_encode_allocate(session, Yang_Turn_Transport_Udp,
					yangtrue);
			break;
		case YangTurnAllocate:
			err = yang_turn_encode_allocate(session, Yang_Turn_Transport_Udp,
					yangfalse);
			break;
		case YangTurnPermission:
			if (session->peers.vec.vsize == 0)
				break;
			for (int32_t i = 0; i < session->peers.vec.vsize; i++) {
				if (session->peers.vec.payload[i]->state
						== YangTurnPermission) {
					yang_turn_encode_permission(session,session->peers.vec.payload[i]);
				}
			}
			break;
		case YangTurnBindChannel:
			if (session->peers.vec.vsize == 0)
				break;
			for (int32_t i = 0; i < session->peers.vec.vsize; i++) {
				if (session->peers.vec.payload[i]->state
						== YangTurnBindChannel) {
					yang_turn_encode_bindChannel(session,
							session->peers.vec.payload[i]);
				}
			}
			break;
		case YangTurnReady:
			yang_turnconn_refreshAllocate(session);
			for (int32_t i = 0; i < session->peers.vec.vsize; i++) {
				switch (session->peers.vec.payload[i]->state) {
				case YangTurnPermission:
					yang_turn_encode_permission(session,
							session->peers.vec.payload[i]);
					break;

				case YangTurnBindChannel:
					yang_turn_encode_bindChannel(session,
							session->peers.vec.payload[i]);
					break;

				default:
					break;
				}
			}
			break;
		case YangTurnClose:
			session->lifetime = 0;
			err = yang_turn_encode_refresh(session, yangtrue);
			break;
		default:
			break;
		}
	}

	return err;
}

void* yang_run_turnconn_handle_thread(void *obj) {
	YangTurnSession *session = (YangTurnSession*) obj;
	session->isStart = 1;
	yang_run_turnconn_app(session);
	session->isStart = 0;
	return NULL;
}

int32_t yang_turnconn_start(YangTurnSession *session) {
	if (session->isStart == 1)
		return Yang_Ok;
	int32_t err = Yang_Ok;
	if (session->udp->session.turnBuffer == NULL)
		session->udp->session.turnBuffer = (char*) calloc(1500, 1);
	session->udp->start(&session->udp->session);
	if (pthread_create(&session->threadId, 0, yang_run_turnconn_handle_thread,
			session)) {
		yang_error("YangThread::start could not start thread");
	}
	return err;
}

void yang_turnconn_stop(YangTurnSession *session) {
	if (session == NULL)
		return;
	session->isLoop = 0;
	session->waitTime = 1;
	session->lifetime = 0;
	yang_turn_encode_refresh(session, yangtrue);
	session->state = YangTurnClose;
	while (session->isStart)
		yang_usleep(1000);
}

int32_t yang_turnconn_addPeer(YangTurnSession *session, int32_t uid,
		void *rtcSession, yang_turn_receive receive, char *remoteIp,
		uint16_t port) {

	YangTurnPeer *peer = (YangTurnPeer*) calloc(sizeof(YangTurnPeer), 1);

	peer->address.mapAddress = be32toh(inet_addr(remoteIp));
	peer->address.port = port;

	peer->receiveUser = rtcSession;
	peer->receive = receive;

	peer->uid=uid;

	if (session->channelNum > Yang_Turn_ChannelNumBase + 31)
		session->channelNum = Yang_Turn_ChannelNumBase;
	peer->channelNumber = session->channelNum;
	session->channelNum++;
	peer->state = YangTurnPermission;

	pthread_mutex_lock(&session->turnLock);
	session->peers.insert(&session->peers.vec, peer);
	pthread_mutex_unlock(&session->turnLock);

	session->waitTime = 50;

	//if (session->state >= YangTurnPermission)		return yang_turn_encode_permission(session, peer);

	return Yang_Ok;

}

int32_t yang_turnconn_removePeer(YangTurnSession *session, int32_t uid) {
	pthread_mutex_lock(&session->turnLock);
	YangTurnPeer *peer = NULL;
	for (int32_t i = 0; i < session->peers.vec.vsize; i++) {
		peer = session->peers.vec.payload[i];
		if (peer->uid == uid) {
			session->channelNum = peer->channelNumber;
			yang_free(session->peers.vec.payload[i]);
			session->peers.remove(&session->peers.vec, i);
			return Yang_Ok;
		}
	}
	pthread_mutex_unlock(&session->turnLock);
	return Yang_Ok;
}

yangbool yang_turnconn_isReady(YangTurnSession *session) {
	return session->state == YangTurnReady ? yangtrue : yangfalse;
}

yangbool yang_turnconn_isAllocated(YangTurnSession *session) {
	return session->state == YangTurnPermission ? yangtrue : yangfalse;
}

void yang_create_turnConnection(YangTurnConnection *conn, YangIceServer *server,
		YangRtcUdp *udp, int32_t localport) {
	YangTurnSession *session = &conn->session;
	session->udp = udp;
	session->server = server;

	udp->session.user = session;
	udp->session.receive = g_yang_turn_receive_data;

	yang_create_stunlib(&session->stun);

	yang_create_YangTurnPeerVector2(&session->peers);

	char serverIp[64] = { 0 };
	yang_getIp(session->server->serverIp, serverIp);
	session->udp->updateRemoteAddress(&session->udp->session, serverIp,
			session->server->serverPort);

	session->icePwd = (char*) calloc(33, 1);
	yang_cstr_random(32, session->icePwd);

	session->isLoop = 0;
	session->isStart = 0;
	session->nonceLen = 0;
	session->relayIp = 0;
	session->relayPort = 0;
	session->allocateOuttime = 0;
	session->permissionCount = 0;
	session->channelNum = Yang_Turn_ChannelNumBase;
	session->lifetime = Yang_Turn_Default_Lifetime;
	session->state = YangTurnNew;
	session->waitTime = 20;

	pthread_mutex_init(&session->turnLock, NULL);

	conn->start = yang_turnconn_start;
	conn->stop = yang_turnconn_stop;
	conn->addPeer = yang_turnconn_addPeer;
	conn->removePeer = yang_turnconn_removePeer;
	conn->sendData = yang_turnconn_sendData;
	conn->isReady = yang_turnconn_isReady;
	conn->isAllocated = yang_turnconn_isAllocated;
}

void yang_destroy_turnConnection(YangTurnConnection *conn) {

	YangTurnSession *session = &conn->session;

	yang_turnconn_stop(session);

	session->udp->stop(&session->udp->session);
	yang_destroy_rtcudp(session->udp);
	yang_free(session->udp);

	for (int32_t i = 0; i < session->peers.vec.vsize; i++) {
		yang_free(session->peers.vec.payload[i]);
	}
	yang_destroy_YangTurnPeerVector2(&session->peers);

	yang_destroy_stunlib(&session->stun);
	yang_free(session->icePwd);

	pthread_mutex_destroy(&session->turnLock);

}

