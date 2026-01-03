//
// Copyright (c) 2019-2023 yanggaofeng
//

#include <yangice/YangIceAgent.h>

#include <yangice/YangIce.h>
#include <yangice/YangIceUtil.h>

#include <yangutil/sys/YangVector.h>
#include <yangutil/sys/YangCString.h>

#include <yangsdp/YangCandidate.h>
#include <yangrtc/YangRtcSession.h>

#include <yangutil/sys/YangTime.h>

#include <yangjson/YangJson.h>

typedef struct {
	yangbool responsed;

	YangCandidate *localCandidate;
	YangCandidate *remoteCandidate;
	char tid[13];
} YangCandidatePair;

yang_vector_declare2(YangCandidate)

yang_vector_declare2(YangCandidatePair)

typedef struct {
	yangbool gather_isStart;
	yangbool connect_isStart;
	yangbool connect_isLoop;
	yangbool isTurnAddPeer;
	int32_t maxCheckTime; //ms
	int32_t foundation;
	int32_t gatherTurnWaitTime;

	yang_thread_t gather_threadId;
	yang_thread_t connect_threadId;
	yang_thread_mutex_t connect_lock;

	void *iceSession;

	YangCandidateVector2 local_candidates;
	YangCandidateVector2 remote_candidates;
	YangCandidatePairVector2 pair;
} YangIceAgentSession;

yang_vector_impl2(YangCandidate)

yang_vector_impl2(YangCandidatePair)

static int32_t yang_iceagent_getIndex(YangIceAgentSession *session, YangIpAddress *ip) {
	int32_t i;
	for (i = 0; i < session->remote_candidates.vec.vsize; i++) {
		YangCandidate *cand = session->remote_candidates.vec.payload[i];
		if (cand && yang_addr_cmp(&cand->address, ip)) {
			return i;
		}
	}
	return -1;
}

static int32_t yang_turn_addpeer(YangIceAgentSession *session,YangIpAddress *address){
	YangIceSession *iceSession = (YangIceSession*) session->iceSession;
	YangRtcSession *rtcSession = (YangRtcSession*) iceSession->rtcSession;
	int32_t turnid=session->remote_candidates.vec.vsize-1;
	if(turnid<0)
		return 1;
	if(session->isTurnAddPeer&&iceSession->turnconn&&iceSession->turnconn->isAllocated(&iceSession->turnconn->session)){
			iceSession->turnconn->addPeer(&iceSession->turnconn->session,turnid,rtcSession,iceSession->receive,address);
	}
	return Yang_Ok;

}

static int32_t yang_iceagent_addRemoteCandidate(void *psession,
		char *candidateStr) {

	int32_t err = Yang_Ok;
	YangIceAgentSession *session = (YangIceAgentSession*) psession;
	YangIceSession *iceSession = NULL;
	YangRtcSession *rtcSession = NULL;
	YangCandidate *candidate = NULL;
	YangJson *candiatejson = NULL;
	YangJsonReader reader = { 0 };

	if (psession == NULL || candidateStr == NULL)
		return ERROR_RTC_ICE;
	iceSession = (YangIceSession*) session->iceSession;
	rtcSession = (YangRtcSession*) iceSession->rtcSession;
	if (yang_create_jsonReader(&reader, candidateStr) != Yang_Ok) {
		yang_error("read remote candidate str json error!\n%s", candidateStr);
		err = ERROR_RTC_ICE;
		goto cleanup;
	}
	candiatejson = reader.getObjectItemCaseSensitive(reader.session,
			"candidate");
	if (candiatejson == NULL
			|| !reader.isString(
					candiatejson) || candiatejson->valuestring == NULL) {
		err = ERROR_RTC_ICE;
		goto cleanup;
	}

	candidate = (YangCandidate*) yang_calloc(sizeof(YangCandidate), 1);

	yang_trace("\n add remote candidate:%s", candiatejson->valuestring);
	if ((err = yang_candidate_parse(candidate, candiatejson->valuestring,
			rtcSession->context.peerInfo->familyType)) != Yang_Ok) {
		goto cleanup;
	}

	yang_thread_mutex_lock(&session->connect_lock);
	if (yang_iceagent_getIndex(session, &candidate->address) == -1) {

		session->remote_candidates.insert(&session->remote_candidates.vec,
				candidate);
		yang_turn_addpeer(session,&candidate->address);

		YangCandidatePair *pair = (YangCandidatePair*) yang_calloc(
				sizeof(YangCandidatePair), 1);



		pair->remoteCandidate = candidate;
		yang_cstr_random(12, pair->tid);
		session->pair.insert(&session->pair.vec, pair);
	} else {
		yang_free(candidate);
	}
	yang_thread_mutex_unlock(&session->connect_lock);

	cleanup: if (err)
		yang_free(candidate);
	yang_destroy_jsonReader(&reader);
	return err;
}

static int32_t yang_iceagent_addRemoteSdpCandidate(void *psession,
		YangCandidate *pCandidate) {
	int32_t err = Yang_Ok;
	YangIceAgentSession *session = (YangIceAgentSession*) psession;
	if (psession == NULL || pCandidate == NULL)
		return ERROR_RTC_ICE;

	if (yang_iceagent_getIndex(session, &pCandidate->address) == -1) {
		YangCandidate *candidate = (YangCandidate*) yang_calloc(
				sizeof(YangCandidate), 1);
		yang_memcpy((char*) candidate, (char*) pCandidate,
				sizeof(YangCandidate));
		session->remote_candidates.insert(&session->remote_candidates.vec,
				candidate);
		YangCandidatePair *pair = (YangCandidatePair*) yang_calloc(
				sizeof(YangCandidatePair), 1);
		pair->remoteCandidate = candidate;
		yang_cstr_random(12, pair->tid);
		session->pair.insert(&session->pair.vec, pair);
	}
	return err;
}

static int32_t yang_iceagent_addRemoteCandidate2(void *psession,
		YangIpAddress *ip) {

	int32_t err = Yang_Ok;
	YangIceAgentSession *session = (YangIceAgentSession*) psession;
	YangIceSession *iceSession = NULL;
	YangRtcSession *rtcSession = NULL;
	YangCandidate *candidate = NULL;
	YangCandidatePair *pair = NULL;
	if (psession == NULL || ip == NULL)
			return ERROR_RTC_ICE;

	iceSession = (YangIceSession*) session->iceSession;
	rtcSession = (YangRtcSession*) iceSession->rtcSession;

	if (yang_iceagent_getIndex(session, ip) == -1) {
		candidate = (YangCandidate*) yang_calloc(
				sizeof(YangCandidate), 1);
		candidate->candidateType = YangIcePeerReflexive;
		yang_addr_copy(ip, &candidate->address,
				rtcSession->context.sock->session.familyType);
		session->remote_candidates.insert(&session->remote_candidates.vec,
				candidate);
		pair = (YangCandidatePair*) yang_calloc(
				sizeof(YangCandidatePair), 1);
		pair->remoteCandidate = candidate;
		yang_cstr_random(12, pair->tid);
		session->pair.insert(&session->pair.vec, pair);
	}
	return err;
}


static void yang_iceagent_getStunStr(YangIceSession* iceSession,char** stunStr){

	char str[128]={0};
	char ip[64] = { 0 };
	int32_t len=0;
	char *p;
	YangRtcSession* rtcSession=(YangRtcSession*)iceSession->rtcSession;

	yang_addr_getIPStr(&rtcSession->context.sock->session.local_addr, ip, sizeof(ip));
	sprintf(str," raddr %s rport %d", ip,iceSession->localPort);
	len=yang_strlen(str);
	p=(char*)yang_malloc(len+1);
	yang_memcpy(p,str,len);
	p[len]=0;
	*stunStr=p;
}

void yang_iceagent_addLocalCandidate(YangIceAgentSession *session,
		YangCandidate *candidate) {
	YangIceSession *iceSession = (YangIceSession*) session->iceSession;
	//YangRtcSession* rtcSession=(YangRtcSession*)iceSession->rtcSession;
	char *candidateStr = NULL;
	char *stunStr=NULL;
	if(candidate->candidateType==YangIceServerReflexive)
		yang_iceagent_getStunStr(iceSession,&stunStr);


	yang_candidate_toJson(candidate, session->foundation++,
			iceSession->local_ufrag,stunStr, &candidateStr);

	yang_free(stunStr);

	if (candidateStr && iceSession->callback.onIceCandidate)
		iceSession->callback.onIceCandidate(iceSession->callback.context,
				iceSession->uid, candidateStr);

	yang_free(candidateStr);
}

static void yang_iceagent_createHostCandidate(YangIceAgentSession *session,
		YangIceSession *iceSession, YangRtcSession *rtcSession) {
	int32_t i;
	YangStringVector iplists;
	yang_create_stringVector(&iplists);
	yang_getLocalInfoList(rtcSession->context.peerInfo->familyType, &iplists);

	for (i = 0; i < iplists.vsize; i++) {
		YangCandidate *candidate = (YangCandidate*) yang_calloc(
				sizeof(YangCandidate), 1);
		candidate->candidateType = YangIceHost;
		candidate->socketProtocol =
				rtcSession->context.peerInfo->rtc.rtcSocketProtocol;
		yang_addr_set(&candidate->address, iplists.payload[i],
				rtcSession->context.peerInfo->rtc.rtcLocalPort,
				rtcSession->context.peerInfo->familyType,candidate->socketProtocol);

		session->local_candidates.insert(&session->local_candidates.vec,
				candidate);
		yang_iceagent_addLocalCandidate(session, candidate);
	}
	yang_destroy_stringVector(&iplists);

}

static int32_t yang_iceagent_initTurn(YangIceSession *session) {

	int32_t count = 0;
	int32_t maxWaitCount = 100;
	yangbool response = yangtrue;
	YangRtcSession *rtcSession = (YangRtcSession*) session->rtcSession;
	YangRtcSocket *sock =NULL;
	int32_t port;

	maxWaitCount = rtcSession->context.peerInfo->rtc.maxTurnWaitTime/20;

	if (session->turnconn == NULL) {
		session->turnconn = (YangTurnConnection*) yang_calloc(
				sizeof(YangTurnConnection), 1);

		sock = (YangRtcSocket*) yang_calloc(sizeof(YangRtcSocket), 1);

		port = rtcSession->context.peerInfo->rtc.rtcLocalPort++;

		yang_create_rtcsocket(sock, rtcSession->context.peerInfo->familyType,session->turnSocketProtocol,port);

		while (yangtrue) {
			if (sock->listen == NULL)
				break;
			if (sock->listen(&sock->session) == Yang_Ok)
				break;
			port = rtcSession->context.peerInfo->rtc.rtcLocalPort++;
			yang_addr_updatePort(&sock->session.local_addr, port);
			if (rtcSession->context.peerInfo->rtc.rtcLocalPort > 65000)
				rtcSession->context.peerInfo->rtc.rtcLocalPort = 15000;
		}

		yang_create_turnConnection(session->turnconn, session, sock, port);
	}

	session->turnconn->start(&session->turnconn->session);
	session->isTurnAllocated = yangtrue;

	while (count < maxWaitCount
			&& !session->turnconn->isAllocated(&session->turnconn->session)) {
		if (session->turnconn->session.errorCode) {
			yang_error("turn handle fail(%d)",session->turnconn->session.errorCode);
			response = yangfalse;
			break;
		}
		yang_usleep(20*1000);
		count++;
	}

	if (!response || count >= maxWaitCount) {
		session->isTurnAllocated = yangfalse;
		session->turnconn->stop(&session->turnconn->session);
		if(count >= maxWaitCount)
			yang_error("turn request timeout!");
		return ERROR_RTC_TURN;
	}

	yang_addr_copy(&session->turnconn->session.relayAddress,
			&session->server.turnAddress,
			session->turnconn->session.relayAddress.familyType);
	return Yang_Ok;
}

static int32_t yang_iceagent_requestTurnServer(YangIceSession *session,
		yang_turn_receive receive, int32_t turnUid, YangIpAddress *addr) {
	yangbool response = yangtrue;
	int32_t count = 0;
	int32_t maxWaitCount = 100;

	if (session->turnconn == NULL) {
		yang_iceagent_initTurn(session);
	} else if (!session->isTurnAllocated) {
		return ERROR_RTC_TURN;
	}

	session->turnconn->addPeer(&session->turnconn->session, turnUid,
			session->rtcSession, receive, addr);

	session->isTurnReady = yangfalse;

	while (count < maxWaitCount
			&& !session->turnconn->isReady(&session->turnconn->session)) {
		if (session->turnconn->session.errorCode) {
			response = yangfalse;
			break;
		}
		yang_usleep(20000);
		count++;
	}

	if (!response || count >= maxWaitCount) {
		session->isTurnReady = yangfalse;
		if (session->turnconn->removePeer)
			session->turnconn->removePeer(&session->turnconn->session, turnUid);
		return ERROR_RTC_TURN;
	}
	if (session->turnStart)
		session->turnStart(session->rtcSession);
	session->isTurnReady = yangtrue;
	return Yang_Ok;
}

static int32_t yang_iceagent_sendTurnCandidate(YangIceAgentSession *session){
	YangIceSession *iceSession = (YangIceSession*) session->iceSession;
	YangRtcSession *rtcSession = (YangRtcSession*) iceSession->rtcSession;
	YangCandidate *turnCandidate = (YangCandidate*) yang_calloc(
			sizeof(YangCandidate), 1);
	turnCandidate->candidateType = YangIceRelayed;
	turnCandidate->socketProtocol =
			rtcSession->context.peerInfo->rtc.rtcSocketProtocol;
	yang_addr_copy(&iceSession->server.turnAddress,
			&turnCandidate->address, iceSession->server.familyType);
	session->local_candidates.insert(&session->local_candidates.vec,
			turnCandidate);
	yang_iceagent_addLocalCandidate(session, turnCandidate);
	return Yang_Ok;
}

void* yang_iceagent_gather_thread(void *obj) {
	int32_t i;
	int32_t err = Yang_Ok;
	YangIceAgentSession *session = (YangIceAgentSession*) obj;
	YangIceSession *iceSession = (YangIceSession*) session->iceSession;
	YangRtcSession *rtcSession = (YangRtcSession*) iceSession->rtcSession;

	session->gather_isStart = yangtrue;
	iceSession->gatherState = YangIceGathering;
	if (iceSession->callback.onIceGatheringState)
		iceSession->callback.onIceGatheringState(iceSession->callback.context,
				iceSession->uid, YangIceGathering);

	if(rtcSession->context.peerInfo->rtc.iceTransportPolicy==YangTransportAll){
		yang_iceagent_createHostCandidate(session, iceSession, rtcSession);
		//YangIceServerReflexive
		if (rtcSession->context.peerInfo->rtc.iceCandidateType > YangIceHost) {
			if (rtcSession->context.peerInfo->iceMode == YangIceModeFull && iceSession->candidateType > YangIceHost
					&& rtcSession->ice.requestStunServer) {

				if ((err = rtcSession->ice.requestStunServer(iceSession)) != Yang_Ok) {
					yang_error("\ngather stun candidate fail(%d)", err);
				}
			}
			if (iceSession->server.stunRequestOk) {
				YangCandidate *candidate = (YangCandidate*) yang_calloc(
						sizeof(YangCandidate), 1);
				candidate->candidateType = YangIceServerReflexive;
				candidate->socketProtocol =
						rtcSession->context.peerInfo->rtc.rtcSocketProtocol;
				yang_addr_copy(&iceSession->server.stunAddress, &candidate->address,
						rtcSession->context.peerInfo->familyType);

				session->local_candidates.insert(&session->local_candidates.vec,
						candidate);
				yang_iceagent_addLocalCandidate(session, candidate);
			}
			iceSession->gatherCandidateType = YangIceServerReflexive;
		}

		yang_usleep(session->gatherTurnWaitTime);
	}
	//YangIceRelayed
	if (rtcSession->context.state < Yang_Conn_State_Connected
			&& iceSession->iceState == YangIceNew
			&& rtcSession->context.peerInfo->rtc.iceCandidateType
					== YangIceRelayed) {

		if (yang_iceagent_initTurn(iceSession) == Yang_Ok) {
			if(!rtcSession->isControlled){
				yang_iceagent_sendTurnCandidate(session);
				yang_usleep(1200*1000);
			}

			int32_t maxCount=0;

			while(session->remote_candidates.vec.vsize==0&&maxCount<50){
				yang_usleep(20*1000);
				maxCount++;
			}

			session->isTurnAddPeer = yangfalse;
			for (i = 0; i < session->remote_candidates.vec.vsize; i++) {
				YangCandidate *remote =
						session->remote_candidates.vec.payload[i];
				if (remote == NULL)
					continue;

				if (yang_iceagent_requestTurnServer(iceSession,
						iceSession->receive, i, &remote->address) != Yang_Ok) {
					char ip[64] = { 0 };
					yang_addr_getIPStr(&remote->address, ip, sizeof(ip));
					yang_error("\nturn add peer(%s,%hu) fail!", ip,
							yang_addr_getPort(&remote->address));
				} else {
					iceSession->gatherCandidateType = YangIceRelayed;
					session->isTurnAddPeer = yangtrue;
				}

			}

			if (session->isTurnAddPeer) {
				for (i = 0; i < 100; i++) {
					if (iceSession->turnconn->isAllReady(
							&iceSession->turnconn->session))
						break;
					yang_usleep(5000);
				}

				if(rtcSession->isControlled){
					//yang_usleep(50*1000);
					yang_iceagent_sendTurnCandidate(session);
				}


			}

		} else {
			yang_error("request turn fail!");
			yang_destroy_turnConnection(iceSession->turnconn);
			yang_free(iceSession->turnconn);
		}
	}

	iceSession->gatherState = YangIceGatherComplete;

	if (iceSession->callback.onIceGatheringState)
		iceSession->callback.onIceGatheringState(iceSession->callback.context,
				iceSession->uid, YangIceGatherComplete);
	session->gather_isStart = yangfalse;

	return NULL;
}

static int32_t yang_iceagent_startGather(void *psession) {
	YangIceAgentSession *session = (YangIceAgentSession*) psession;
	if (session->gather_isStart)
		return Yang_Ok;
	if (yang_thread_create(&session->gather_threadId, 0,
			yang_iceagent_gather_thread, session)) {
		yang_error("YangThread::start could not start thread");
		return ERROR_THREAD;
	}
	return Yang_Ok;
}

static int32_t yang_iceagent_checkConnection(void *psession, int32_t turnUid,
		YangCandidatePair *pair) {

	int32_t err = Yang_Ok;
	YangIceAgentSession *session = (YangIceAgentSession*) psession;
	YangIceSession *iceSession = NULL;
	YangRtcSession *rtcSession = NULL;

	char *stundata = NULL;
	int32_t nb = 0;

	if (psession == NULL || pair == NULL)
			return ERROR_RTC_ICE;

	iceSession = (YangIceSession*) session->iceSession;
	rtcSession = (YangRtcSession*) iceSession->rtcSession;

	iceSession->stun.getRequestStunPacket(rtcSession, pair->tid,
			iceSession->remoteIcePwd, &stundata, &nb);


	if (iceSession->gatherCandidateType == YangIceRelayed) {
		if (iceSession->turnconn->isAllReady(&iceSession->turnconn->session)&&(err = iceSession->turnconn->sendData(
				&iceSession->turnconn->session, turnUid, stundata, nb))
				!= Yang_Ok) {
			yang_error("send turn data fail(%d)!", err);
			err = ERROR_RTC_ICE_AGENT;
		}
	} else if(rtcSession->context.peerInfo->rtc.iceTransportPolicy==YangTransportAll){
       // char ip[64]={0};
       // yang_addr_getIPStr(&pair->remoteCandidate->address,ip,64);
       // yang_trace("\ncheck candidate address ip=%s,port=%hu,stun nb=%d",ip,yang_addr_getPort(&pair->remoteCandidate->address),nb);
        if (nb> 0&&(err=rtcSession->context.sock->write2(&rtcSession->context.sock->session,&pair->remoteCandidate->address,stundata, nb))!=Yang_Ok) {
			yang_error("send stun fail(%d)!", err);
			err = ERROR_RTC_ICE_AGENT;
		}
	}
	yang_free(stundata);
	return err;
}

void* yang_iceagent_connectivity_thread(void *obj) {
	int32_t i;
    uint64_t startTime = yang_get_system_time();
	YangCandidatePair *pair;
	YangIceAgentSession *session = (YangIceAgentSession*) obj;
	YangIceSession *iceSession = (YangIceSession*) session->iceSession;
	YangRtcSession *rtcSession = (YangRtcSession*) iceSession->rtcSession;

	if (session->connect_isStart)
		return Yang_Ok;

	session->connect_isStart = yangtrue;
	session->connect_isLoop = yangtrue;
	if(rtcSession->isControlled)
		yang_usleep(1000*200);

	while (session->connect_isLoop && !iceSession->isPaired
			&& iceSession->iceState == YangIceNew) {
		if ((yang_get_system_time() - startTime) / 1000
				> session->maxCheckTime) {
			iceSession->iceState = YangIceFail;
			rtcSession->ice.onIceStateChange(iceSession, YangIceFail);
			session->connect_isLoop = yangfalse;
			break;
		}

		if (session->pair.vec.vsize > 0) {
			yang_thread_mutex_lock(&session->connect_lock);
			for (i = 0; i < session->pair.vec.vsize; i++) {
				pair = session->pair.vec.payload[i];
				yang_iceagent_checkConnection(session, i, pair);
			}
			yang_thread_mutex_unlock(&session->connect_lock);
		}

		yang_usleep(1000 * 100);
	}

	session->connect_isStart = yangfalse;

	return NULL;
}

static int32_t yang_iceagent_startConnectivity(void *psession) {

	YangIceAgentSession *session = (YangIceAgentSession*) psession;
	if (yang_thread_create(&session->connect_threadId, 0,
			yang_iceagent_connectivity_thread, session)) {
		yang_error("YangThread::start could not start thread");
		return ERROR_THREAD;
	}
	return Yang_Ok;
}

static void yang_iceagent_stopGather(void *psession) {
	if (psession == NULL)
		return;
}

static void yang_iceagent_stopConnectivity(void *psession) {
	YangIceAgentSession *session = (YangIceAgentSession*) psession;
	if (psession == NULL)
		return;

	session->connect_isLoop = yangfalse;
}

static YangCandidate* yang_iceagent_getLocalCandidate(YangIceAgentSession *session,
		YangIpAddress *address) {
	int32_t i;
	YangCandidate *candidate;
	for (i = 0; i < session->local_candidates.vec.vsize; i++) {
		candidate = session->local_candidates.vec.payload[i];
		if (yang_addr_cmp(&candidate->address, address)) {
			return candidate;
		}
	}
	return NULL;
}

static int32_t yang_iceagent_on_stun(void *psession, YangStunPacket *response) {
	int32_t i;
	YangCandidatePair *pair;
	YangIceAgentSession *session = (YangIceAgentSession*) psession;
	YangIceSession *iceSession = NULL;

	if (psession == NULL)
			return ERROR_RTC_ICE;
	iceSession = (YangIceSession*) session->iceSession;
	if (iceSession->isPaired)
		return Yang_Ok;


	for (i = 0; i < session->pair.vec.vsize; i++) {
		pair = session->pair.vec.payload[i];
		if (yang_memcmp(response->transcation_id, pair->tid, 12) == 0) {
			pair->responsed = yangtrue;
			iceSession->isPaired = yangtrue;
			iceSession->turnUid = i;
			pair->localCandidate = yang_iceagent_getLocalCandidate(session,
					&response->address);
		}
	}


	return Yang_Ok;
}

static int32_t yang_iceagent_on_server_stun(void *psession, YangStunPacket *request) {

	int32_t i;
	YangCandidate *remote;
	YangIceAgentSession *session = (YangIceAgentSession*) psession;
	YangIceSession *iceSession = NULL;
	if (psession == NULL)
			return ERROR_RTC_ICE;
	iceSession = (YangIceSession*) session->iceSession;

	if (iceSession->isPaired)
		return Yang_Ok;


	for (i = 0; i < session->remote_candidates.vec.vsize; i++) {
		remote = session->remote_candidates.vec.payload[i];
		if (yang_addr_cmp(&request->address, &remote->address) == 0) {
			iceSession->turnUid = i;
			iceSession->isPaired = yangtrue;
		}
	}

	return Yang_Ok;
}

static YangCandidate* yang_iceagent_getRemoteCandidateByTurn(void *psession) {
	YangIceAgentSession *session = (YangIceAgentSession*) psession;
	if (psession == NULL)
		return NULL;

	if (session->remote_candidates.vec.vsize > 0)
		return session->remote_candidates.vec.payload[0];
	return NULL;
}

void yang_create_iceAgent(YangIceAgent *agent, void *iceSession) {
	YangIceAgentSession *session;
	if (agent == NULL || agent->session)
		return;
	session = (YangIceAgentSession*) yang_calloc(sizeof(YangIceAgentSession), 1);
	agent->session = session;
	session->iceSession = iceSession;

	session->gather_isStart = yangfalse;
	session->connect_isStart = yangfalse;
	session->connect_isLoop = yangfalse;
	session->isTurnAddPeer= yangfalse;
	session->foundation = 0;
	session->maxCheckTime = 30 * 1000;
	session->gatherTurnWaitTime = Yang_gatherTurn_WaitTime * 1000;

	yang_thread_mutex_init(&session->connect_lock,NULL);
	yang_create_YangCandidateVector2(&session->local_candidates);
	yang_create_YangCandidateVector2(&session->remote_candidates);
	yang_create_YangCandidatePairVector2(&session->pair);

	agent->addRemoteCandidate = yang_iceagent_addRemoteCandidate;
	agent->addRemoteSdpCandidate = yang_iceagent_addRemoteSdpCandidate;
	agent->addRemoteCandidate2 = yang_iceagent_addRemoteCandidate2;
	agent->getRemoteCandidateByTurn = yang_iceagent_getRemoteCandidateByTurn;
	agent->startGather = yang_iceagent_startGather;
	agent->stopGather = yang_iceagent_stopGather;
	agent->startConnectivity = yang_iceagent_startConnectivity;
	agent->stopConnectivity = yang_iceagent_stopConnectivity;
	agent->on_stun = yang_iceagent_on_stun;
	agent->on_server_stun = yang_iceagent_on_server_stun;

}

void yang_destroy_iceAgent(YangIceAgent *agent) {
	int32_t i;
	YangIceAgentSession *session=NULL;
	if (agent == NULL || agent->session == NULL)
		return;
	session = (YangIceAgentSession*) agent->session;

	session->connect_isLoop=yangfalse;

	if (session->gather_isStart) {

		while (session->gather_isStart)
			yang_usleep(1000);
	}
	if (session->connect_isStart) {
		session->connect_isLoop = yangfalse;
		while (session->connect_isStart)
			yang_usleep(1000);
	}

	for (i = 0; i < session->local_candidates.vec.vsize; i++) {
		yang_free(session->local_candidates.vec.payload[i])
	}
	session->local_candidates.clear(&session->local_candidates.vec);
	yang_destroy_YangCandidateVector2(&session->local_candidates);

	for (i = 0; i < session->remote_candidates.vec.vsize; i++) {
		yang_free(session->remote_candidates.vec.payload[i])
	}
	session->remote_candidates.clear(&session->remote_candidates.vec);
	yang_destroy_YangCandidateVector2(&session->remote_candidates);

	session->pair.clear(&session->pair.vec);
	yang_destroy_YangCandidatePairVector2(&session->pair);

	yang_thread_mutex_destroy(&session->connect_lock);
	yang_free(agent->session);
}
