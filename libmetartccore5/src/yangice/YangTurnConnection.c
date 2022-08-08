//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangice/YangTurnConnection.h>
#include <yangice/YangRtcStunH.h>
#include <yangutil/sys/YangEndian.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangSocket.h>
#include <yangutil/sys/YangEndian.h>
#include <yangutil/sys/YangCString.h>
#include <yangrtc/YangRtcConnection.h>

int32_t yang_turnconn_receive(YangTurnSession *session, char *data, int32_t nb);

void g_yang_turn_receive_data(char *data, int32_t size, void *user) {
	if (user == NULL)
		return;
	YangTurnSession *session = (YangTurnSession*) user;
	yang_turnconn_receive(session, data, size);
}

int32_t yang_rtc_turn_sendData(YangUdpSession *session, char *data, int32_t nb) {

	if (session == NULL || !session->isStart || session->notRemoteInit|| data == NULL)		return ERROR_RTC_UDP;

	memcpy(session->turnBuffer + 4, data, nb);
	yang_put_be16(session->turnBuffer + 2, nb);
	return sendto(session->fd, session->turnBuffer, nb + 4, 0,
			(struct sockaddr*) &session->remote_addr, sizeof(struct sockaddr))
			> 0 ? Yang_Ok : ERROR_RTC_UDP;
}

int32_t yang_turnconn_sendData(YangTurnSession *psession, int32_t uid,	char *data, int32_t nb) {
	if (psession == NULL || data == NULL)		return ERROR_RTC_UDP;

	YangUdpSession *session = &psession->udp->session;

	if (session == NULL || !session->isStart || session->notRemoteInit||psession->state!=YangTurnReady)				return ERROR_RTC_UDP;

	memcpy(session->turnBuffer + 4, data, nb);
	yang_put_be16(session->turnBuffer + 2, nb);
	YangTurnPeer *peer = NULL;
	for (int32_t i = 0; i < psession->peers.vec.vsize; i++) {
		peer = psession->peers.vec.payload[i];
		if (peer && peer->state == YangTurnReady && peer->uid == uid) {

			yang_put_be16(session->turnBuffer, peer->channelNumber);

			return  sendto(session->fd, session->turnBuffer, nb+4, 0, (struct sockaddr*) &session->remote_addr,sizeof(struct sockaddr))> 0 ? Yang_Ok : 1;
		}
	}

	return ERROR_RTC_UDP;
}

int32_t yang_turnconn_handleError(YangTurnSession *session, char *data,
		int32_t nb) {
	int32_t err = Yang_Ok;
	uint16_t stuntype = yang_get_be16((uint8_t*) data);
	switch (stuntype) {
	case StunAllocateErrorResponse:
		yang_error("StunAllocateErrorResponse");
		if (session->nonceLen > 0)
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
		yang_error("Yang_Turn_Error_Unauthoorized");
		break;
	case Yang_Turn_Error_Nonce:
		yang_error("\nYang_Turn_Error_Nonce");
		break;
	}

	return err;
}

int32_t yang_turnconn_handleRecv(YangTurnSession *session, char *data,
		int32_t nb) {
	int32_t err = Yang_Ok;
	uint16_t stuntype = yang_get_be16((uint8_t*) data);

	switch (stuntype) {
	case StunBindingSuccessResponse:
		session->state = YangTurnCredential;
		break;
	case StunAllocateSuccessResponse:
		if (session->state == YangTurnCredential) {
			session->state = YangTurnAllocate;
			break;
		}
		session->server->relayIp = yang_get_be32((uint8_t*)(&session->response.mappedAddress));
		session->server->relayPort = session->response.mappedPort;
		if(session->relayIp==0) session->relayIp=session->server->relayIp;
		if(session->relayPort==0) {
			session->relayPort=session->response.mappedPort;
			uint8_t* ip=(uint8_t*)&session->relayIp;
			yang_trace("\nturn allocate ip=%u.%u.%u.%u,turn port=%d",ip[0],ip[1],ip[2],ip[3],session->relayPort);
		}
		session->state = YangTurnPermission;
		break;
	case StunRefreshteSuccessResponse:
		session->refreshTime=yang_get_system_time()/(1000*1000);
		break;
	case StunPermissionSuccessResponse:
		for (int32_t i = 0; i < session->peers.vec.vsize; i++) {
			if (session->peers.vec.payload[i]->state == YangTurnPermission) {
				session->peers.vec.payload[i]->state = YangTurnBindChannel;
			}
		}
		if (session->state < YangTurnBindChannel)
			session->state = YangTurnBindChannel;
		break;
	case StunChannelBindSuccessResponse:
		for (int32_t i = 0; i < session->peers.vec.vsize; i++) {
			if (session->peers.vec.payload[i]->state == YangTurnBindChannel) {
				session->peers.vec.payload[i]->state = YangTurnReady;
			}
		}
		if (session->state < YangTurnReady)
			session->state = YangTurnReady;
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

		if ((stuntype >= StunBindingErrorResponse && stuntype <= StunRefreshErrorResponse)
				|| (stuntype == StunPermissionErrorResponse	|| stuntype == StunChannelBindErrorResponse)) {
			yang_turnconn_handleError(session, data, nb);
		} else {
			yang_turnconn_handleRecv(session, data, nb);
		}
	} else { //channel data

		channelNum = yang_get_be16((uint8_t*) data);
		len = (int32_t)yang_get_be16((uint8_t*) data + 2);

		YangTurnPeer *peer = NULL;
		for (int32_t i = 0; i < session->peers.vec.vsize; i++) {
			peer = session->peers.vec.payload[i];
			if (peer && peer->state == YangTurnReady && peer->channelNumber == channelNum) {
				peer->receive(peer->receiveUser, data + 4, len);
				break;
			}
		}

	}

	return err;
}

int32_t yang_run_turnconn_app(YangTurnSession *session) {
	int32_t err = Yang_Ok;
	session->isLoop = 1;
	while (session->isLoop) {
		yang_usleep(200000);
		switch (session->state) {
		case YangTurnNew:
			err = yang_turn_encode_request(session);
			break;
		case YangTurnCredential:
			err = yang_turn_encode_allocate(session, Yang_Turn_Transport_Udp,yangtrue);
			break;
		case YangTurnAllocate:
			err = yang_turn_encode_allocate(session, Yang_Turn_Transport_Udp,yangfalse);
			break;
		case YangTurnPermission:
			if (session->peers.vec.vsize == 0)
				break;
			for (int32_t i = 0; i < session->peers.vec.vsize; i++) {
				if (session->peers.vec.payload[i]->state
						== YangTurnPermission) {
					yang_turn_encode_permission(session,
							session->peers.vec.payload[i]);
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
			if(session->refreshTime==0){
				err = yang_turn_encode_refresh(session);
			}else if(yang_get_system_time()/(1000*1000)-session->refreshTime>=session->lifetime){
				err = yang_turn_encode_refresh(session);
			}
			break;
		case YangTurnClose:
			session->lifetime = 0;
			err = yang_turn_encode_refresh(session);
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

	if (session->channelNum > Yang_Turn_ChaneelBase + 31)
		session->channelNum = Yang_Turn_ChaneelBase;
	peer->channelNumber = session->channelNum;
	session->channelNum++;
	peer->state = YangTurnPermission;

	pthread_mutex_lock(&session->turnLock);
	session->peers.insert(&session->peers.vec, peer);
	pthread_mutex_unlock(&session->turnLock);

	if (session->state >= YangTurnPermission)
		return yang_turn_encode_permission(session, peer);

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


yangbool yang_turnconn_isReady(YangTurnSession* session){
	return session->state==YangTurnReady?yangtrue:yangfalse;
}
yangbool yang_turnconn_isAllocated(YangTurnSession* session){
	return session->state==YangTurnPermission?yangtrue:yangfalse;
}
void yang_create_turnConnection(YangTurnConnection *conn, YangIceServer *server,
		YangRtcUdp *udp, int32_t localport) {
	YangTurnSession *session = &conn->session;
	session->udp = udp;
	session->server = server;

	udp->session.user = session;
	udp->session.receive = g_yang_turn_receive_data;

	yang_create_YangTurnPeerVector2(&session->peers);

	char serverIp[64] = { 0 };
	yang_getIp(session->server->serverIp, serverIp);
	session->udp->updateRemoteAddress(&session->udp->session, serverIp,
			session->server->serverPort);

	session->icePwd = (char*) calloc(33, 1);
	yang_cstr_random(32, session->icePwd);

	session->peerCount = 0;
	session->channelNum = Yang_Turn_ChaneelBase;
	session->lifetime = 600;
	session->state = YangTurnNew;
	pthread_mutex_init(&session->turnLock, NULL);

	conn->start = yang_turnconn_start;
	conn->stop = yang_turnconn_stop;
	conn->addPeer = yang_turnconn_addPeer;
	conn->removePeer = yang_turnconn_removePeer;
	conn->sendData = yang_turnconn_sendData;
	conn->isReady=yang_turnconn_isReady;
	conn->isAllocated=yang_turnconn_isAllocated;
}
void yang_destroy_turnConnection(YangTurnConnection *conn) {
	YangTurnSession *session = &conn->session;
	if (session->isStart) {
		session->isLoop = 0;
		while (session->isStart)
			yang_usleep(2000);
	}
	for (int32_t i = 0; i < session->peers.vec.vsize; i++) {
		yang_free(session->peers.vec.payload[i]);
	}
	yang_destroy_YangTurnPeerVector2(&session->peers);

	yang_free(session->icePwd);
	pthread_mutex_destroy(&session->turnLock);
}

