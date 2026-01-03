//
// Copyright (c) 2019-2026 yanggaofeng
//

#include <yangice/YangIce.h>
#include <yangice/YangIceUtil.h>

#include <yangutil/yangavinfo.h>

#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangMath.h>
#include <yangutil/sys/YangSocket.h>
#include <yangutil/sys/YangEndian.h>
#include <yangutil/sys/YangCString.h>
#include <yangrtc/YangRtcConnection.h>



static void ice_onIceStateChange(YangIceSession* session,YangIceCandidateState iceState){

	if(session->callback.onIceStateChange){
		session->callback.onIceStateChange(session->callback.context,session->uid,iceState);
	}
}

static int32_t yang_ice_onStunResponse(YangIceSession* session,char* data,int32_t nb){
	int32_t err=Yang_Ok;
	YangStunPacket request={0};
	YangRtcSession* rtcSession=(YangRtcSession*)session->rtcSession;

	if ((err = session->stun.decode(&request,data, nb)) != Yang_Ok) {

		yang_error("decode stun packet failed");
		return ERROR_RTC_ICE_STUN;
	}
	if(request.message_type==StunBindingSuccessResponse){
		yang_addr_copy(&request.address,&session->server.stunAddress,request.address.familyType);
		//char ips[64]={0};
		//yang_addr_getIPStr(&session->server.stunAddress,ips,64);
		//yang_trace("\n>>>stun ipadress===%s,port===%d",ips,yang_addr_getPort(&session->server.stunAddress));
		if(rtcSession)
			rtcSession->context.sock->session.stunRequesting=yangfalse;
		session->server.stunRequestOk=yangtrue;
	}
	return err;
}


 int32_t yang_ice_stun_request(YangIceSession* session,YangIceServer* server,int32_t localport){


	int32_t err=Yang_Ok;
	int32_t waitTime=0;
	char* pwd;
	YangRtcStun* stun;
    YangRtcSession* rtcSession=(YangRtcSession*)session->rtcSession;
	YangRtcSocket* sock=rtcSession->context.sock;
	YangBuffer buf;
	YangIpAddress stunServerAddress={0};
	char tmp[1024]={0};

	yang_addr_set(&stunServerAddress,server->serverIp,server->serverPort,server->familyType,session->rtcSocketProtocol);
	stun=(YangRtcStun*)yang_calloc(sizeof(YangRtcStun),1);

	yang_create_stun(stun);

	yang_init_buffer(&buf,tmp,1024);

	pwd = (char*) yang_calloc(33, 1);
	yang_cstr_random(32, pwd);
	stun->encodeStunServer(&buf,server->username,pwd);

	if((err=sock->write2(&sock->session,&stunServerAddress,buf.data, yang_buffer_pos(&buf)))!=Yang_Ok){
		yang_error("stun server request fail");
		goto ret;
	}

	while(rtcSession->context.sock->session.stunRequesting&&waitTime<=500){
		yang_usleep(YANG_UTIME_MILLISECONDS*2);
		waitTime++;
		if(waitTime%25==0){
			yang_memset(tmp,0,sizeof(tmp));
			yang_init_buffer(&buf,tmp,sizeof(tmp));
			stun->encodeStunServer(&buf,server->username,pwd);
			if((err=sock->write2(&sock->session,&stunServerAddress,buf.data, yang_buffer_pos(&buf)))!=Yang_Ok){
				yang_error("stun server request fail");
				goto ret;
			}
		}
	}

	if(waitTime>500){
		err=ERROR_RTC_STUN;
	}

	ret:
		rtcSession->context.sock->session.stunRequesting=yangfalse;
		yang_free(pwd);
		yang_destroy_stun(stun);
		yang_free(stun);
		return err;
}

static int32_t yang_ice_requestStunServer(YangIceSession *session){

	int32_t err=Yang_Ok;
	YangRtcSession* rtcSession=(YangRtcSession*)session->rtcSession;
	if(session->candidateType==YangIceHost)
		return ERROR_RTC_STUN;

	rtcSession->context.sock->session.stunRequesting=yangtrue;
    if((err=yang_ice_stun_request(session,&session->server,session->localPort))!=Yang_Ok){
		return yang_error_wrap(err,"stun request fail!");
	}

	return err;
}



static int32_t yang_ice_onStunRtp(YangIceSession* session,char* data,int32_t len){

	int32_t err=Yang_Ok;
	YangStunPacket request={0};
	YangRtcSession* rtcSession=NULL;
	YangIpAddress* ipAddress = NULL;
	if(session==NULL||data==NULL)
		return ERROR_RTC_ICE_STUN;

	rtcSession=(YangRtcSession*)session->rtcSession;

	if ((err = session->stun.decode(&request,data, len)) != Yang_Ok) {
		yang_error("decode stun packet failed");
		return ERROR_RTC_ICE_STUN;
	}

	if(request.message_type==StunBindingRequest){

		if(session->remoteIceMode==YangIceModeLite || session->gatherCandidateType==YangIceRelayed){
			ipAddress = &rtcSession->context.sock->session.remote_addr;
		}else if(rtcSession->context.sock->session.income_addr){
			ipAddress = rtcSession->context.sock->session.income_addr;
		}else{
			ipAddress = &rtcSession->context.sock->session.remote_addr;
		}

		if ((err =session->stun.createResponseStunPacket(&request,ipAddress,
				rtcSession->context.peerInfo->familyType,session->rtcSession)) != Yang_Ok) {
			yang_error("create response stun packet failed");
		}

		if(!session->isPaired&&rtcSession->isControlled&&session->agent.on_stun){

			session->agent.on_server_stun(session->agent.session,&request);
			if(session->isPaired){

				session->agent.stopConnectivity(session->agent.session);
				session->agent.stopGather(session->agent.session);
			}
		}
		return err;
	}



	if(request.message_type==StunBindingSuccessResponse){

		if(session->iceState==YangIceNew){
			session->iceState=YangIceSuccess;
			ice_onIceStateChange(session,YangIceSuccess);
		}

		if(session->remoteIceMode==YangIceModeLite) return err;

		if(!session->isPaired&&!rtcSession->isControlled&&session->agent.on_stun){

			session->agent.on_stun(session->agent.session,&request);
			if(session->isPaired){
				session->agent.stopConnectivity(session->agent.session);
				session->agent.stopGather(session->agent.session);
			}
		}

	}

	return err;

}

static yangbool yang_isStunRtp(YangIceSession* session,char* data,int32_t len){
	YangRtcSession* rtcSession=NULL;
	if(session==NULL || data == NULL)
		return yangfalse;
	rtcSession=(YangRtcSession*)session->rtcSession;

	if(rtcSession==NULL)
		return yangfalse;

	if(session->gatherCandidateType==YangIceRelayed)
		return yangtrue;

	if(rtcSession->isControlled && data[0] == 0x00 && data[1]==0x01)
		return yangtrue;

	if(!rtcSession->isControlled && data[0] == 0x01 && data[1]==0x01)
		return yangtrue;

	return yangfalse;
}

static void yang_ice_setRequestStunData(YangIceSession* session,char* data,int32_t nb){
	if(session==NULL) return;
	if (session->stunData.data == NULL)
			session->stunData.data = (char*) yang_calloc(1, nb);
		yang_memcpy(session->stunData.data, data, nb);

		session->stunData.nb = nb;
}

static int32_t yang_ice_sendRequestStun(YangIceSession* session){
	int32_t err=Yang_Ok;
	YangRtcSession* rtcSession=(YangRtcSession*)session->rtcSession;

	if(session==NULL)
		return ERROR_RTC_STUN;

	if(session->stunData.data&&(err=rtcSession->context.sock->write(&rtcSession->context.sock->session,session->stunData.data, session->stunData.nb))!=Yang_Ok){
					yang_error_wrap(ERROR_RTC_STUN,"send stun fail!");
	}
	return err;
}

static void yang_ice_initAgent(YangIceSession *session){
    if(session->agent.session==NULL)
    	yang_create_iceAgent(&session->agent,session);
}

static int32_t yang_ice_addIceCandidateStr(YangIceSession *session,char* candidateStr){
	if(session==NULL||candidateStr==NULL)
		return ERROR_RTC_ICE;

	yang_ice_initAgent(session);
	return session->agent.addRemoteCandidate(session->agent.session,candidateStr);
}

static int32_t yang_ice_addSdpCandidate(YangIceSession *session,YangCandidate* candidate){
	if(session==NULL||candidate==NULL)
		return ERROR_RTC_ICE;

	yang_ice_initAgent(session);
    return session->agent.addRemoteSdpCandidate(session->agent.session,candidate);
}

static int32_t yang_ice_addIceCandidate2(YangIceSession *session,YangIpAddress* ip){
	if(session==NULL||ip==NULL)
		return ERROR_RTC_ICE;

	yang_ice_initAgent(session);
	return session->agent.addRemoteCandidate2(session->agent.session,ip);
}

static int32_t yang_ice_startIceAgent(YangIceSession *session){
	yang_ice_initAgent(session);

    session->agent.startConnectivity(session->agent.session);

	if(session->sdpWithCandidate&&session->remoteIceMode==YangIceModeLite){
		return Yang_Ok;
	}

	session->agent.startGather(session->agent.session);
	return Yang_Ok;
}


void yang_create_ice(YangIce* ice,void* rtcSession,YangPeerInfo* peerInfo,YangPeerCallback* callback,yang_turn_receive receive,yang_turn_start turnStart){
	YangIceSession* session;
	if(ice==NULL)
		return;

	session=&ice->session;
	session->uid=peerInfo->uid;
	session->turnUid=0;
	yang_create_stun(&session->stun);

	session->rtcSession=rtcSession;
	session->receive=receive;
	session->turnStart=turnStart;
    session->agent.session=NULL;
	session->candidateType=(YangIceCandidateType)peerInfo->rtc.iceCandidateType;
	session->iceState=YangIceNew;
	session->gatherState=YangIceGatherNew;
	session->remoteIceMode=YangIceModeFull;
	session->gatherCandidateType=YangIceHost;
	session->sdpWithCandidate=yangfalse;
	session->isTurnAllocated=yangfalse;
	session->isTurnReady=yangfalse;
	session->localPort=peerInfo->rtc.rtcLocalPort;
	session->rtcSocketProtocol=peerInfo->rtc.rtcSocketProtocol;
	session->turnSocketProtocol=peerInfo->rtc.turnSocketProtocol;

	session->callback.context=callback->iceCallback.context;
	session->callback.onIceStateChange=callback->iceCallback.onIceStateChange;
	session->callback.onConnectionStateChange=callback->iceCallback.onConnectionStateChange;
	session->callback.onIceCandidate=callback->iceCallback.onIceCandidate;

	session->server.familyType=peerInfo->familyType;
	session->server.serverPort=peerInfo->rtc.iceServerPort;
	//session->server.stunRequestOk=yangfalse;
	yang_strcpy(session->server.serverIp,peerInfo->rtc.iceServerIP);
	yang_strcpy(session->server.username,peerInfo->rtc.iceUserName);
	yang_strcpy(session->server.password,peerInfo->rtc.icePassword);

	session->stunData.data=NULL;
	session->stunData.nb=0;

	ice->isStunRtp = yang_isStunRtp;
	ice->onStunRtp=yang_ice_onStunRtp;
	ice->sendRequestStun=yang_ice_sendRequestStun;
	ice->setRequestStunData=yang_ice_setRequestStunData;
	ice->startIceAgent=yang_ice_startIceAgent;
	ice->requestStunServer= yang_ice_requestStunServer;
	ice->onIceStateChange=ice_onIceStateChange;
	ice->addIceCandidate=yang_ice_addIceCandidateStr;
	ice->addIceCandidate2=yang_ice_addIceCandidate2;
	ice->addSdpCandidate=yang_ice_addSdpCandidate;
	ice->onStunResponse=yang_ice_onStunResponse;
}

void yang_destroy_ice(YangIce* ice){
	YangIceSession* session=NULL;
	if(ice==NULL)
		return;

	session=&ice->session;

	if(session->turnconn){
		yang_destroy_turnConnection(session->turnconn);
		yang_free(session->turnconn);
	}

	yang_destroy_iceAgent(&session->agent);

	yang_free(session->stunData.data);
	yang_destroy_stun(&session->stun);

}

