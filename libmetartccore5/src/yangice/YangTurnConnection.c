//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangice/YangTurnConnection.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangSocket.h>
#include <yangutil/sys/YangEndian.h>
#include <yangrtc/YangRtcConnection.h>
void g_yang_turn_receive_bind(char *data, int32_t size, void *user) {
	if (user == NULL)		return;
	YangTurnSession* request=(YangTurnSession*)user;

	int bt=(int)data[0];
	yang_trace("\nbt==================%d",bt);
//	if(size>0&&(bt==0x00||bt==0x01)){
		int32_t err = 0;

		YangStunPacket response;
		memset(&response,0,sizeof(YangStunPacket));
		if ((err = request->turn->decode(&response,data, size)) != 0) {
			printf("decode stun packet failed");
			return;
		}

		request->ip= yang_get_be32((uint8_t*)(&response.mapped_address));
		//request->port=ntohs(yang_get_be16((uint8_t*)(&response.mapped_port)));
		//request->port=ntohs(response.mapped_port);
		request->port=response.mapped_port;
		request->response=yangtrue;
		uint8_t* ip=(uint8_t*)&request->ip;
		yang_trace("\nstun ip=%u.%u.%u.%u,stunport=%d",ip[0],ip[1],ip[2],ip[3],request->port);
//	}

}

void g_yang_turn_receive_data(char *data, int32_t size, void *user) {
	if (user == NULL)		return;
	YangTurnSession* session=(YangTurnSession*)user;
	session->receive(session->receiveUser,data+4,size-4);
}

int32_t yang_rtc_turn_sendData(YangUdpSession *session, char *data, int32_t nb) {
	if (session == NULL || !session->isStart || session->notRemoteInit || data==NULL)	return ERROR_RTC_UDP;
	memcpy(session->turnBuffer+4,data,nb);
	return sendto(session->fd, session->turnBuffer, nb+4, 0, (struct sockaddr*) &session->remote_addr,sizeof(struct sockaddr)) > 0 ? Yang_Ok : 1;
}



int32_t yang_turn_handle(YangStunMessageType stuntype,YangTurnSession* request,YangBuffer* buf,char* tmp,char* icePwd){
	int32_t err=Yang_Ok;
	int32_t waitTime=0;
	yang_init_buffer(buf,tmp,1024);
	request->response=yangfalse;


	switch(stuntype){
		case BindingRequest:request->turn->request(buf,request->udp,request->server->username,icePwd);break;
		case TrunAllocate:request->turn->allocate(buf,request->udp,request->server->username,icePwd);break;
		case TrunRefresh:request->turn->refresh(buf,request->udp,request->server->username,icePwd);break;
		case TrunSend:break;
		case TrunData:break;
		case TurnPermission:request->turn->permission(buf,request->udp,request->server->username,icePwd);break;
		case ChannelBindingRequest:request->turn->channelRequest(buf,request->udp,request->server->username,icePwd);break;
		case BindingIndication:break;
		case SendIndication:request->turn->sendIndication(buf,request->udp,request->server->username,icePwd);break;
		case DataIndication:break;
		case BindingResponse:break;
		case BindingErrorResponse:break;
		case SharedSecretRequest:break;
		case SharedSecretResponse:break;
		case SharedSecretErrorResponse:break;
	}

	if((err=request->udp->sendData(&request->udp->session,buf->data, yang_buffer_pos(buf)))!=Yang_Ok){
		yang_error("stun server request fail");
		return 1;
	}
	//if(stuntype==TrunRefresh){
	//	yang_usleep(20*1000);
	//	return Yang_Ok;
	//}
	while(!request->response&&waitTime<500){
		yang_usleep(1000);
		waitTime++;
	}
	if(!request->response) return 1;
	return err;
}



int32_t yang_turn_binding(YangTurnSession* session){
	    char tmp[1024]={0};
		int32_t err=Yang_Ok;
		char* icePwd="fi1eke1561or19ht73gz448s52yt4270";

		char serverIp[64]={0};
		yang_getIp(session->server->serverIp,serverIp);
		session->udp->updateRemoteAddress(&session->udp->session,serverIp,session->server->serverPort);

		session->udp->session.receive=g_yang_turn_receive_bind;
		session->udp->session.user=session;
		session->udp->start(&session->udp->session);


		session->icePwd=icePwd;
		session->response=yangfalse;

		YangBuffer buf;


		if(yang_turn_handle(BindingRequest,session,&buf,tmp,icePwd)!=Yang_Ok){
			return yang_error_wrap(ERROR_RTC_TURN,"turn server request fail");

		}

		if(yang_turn_handle(TrunAllocate,session,&buf,tmp,icePwd)!=Yang_Ok){
			return yang_error_wrap(ERROR_RTC_TURN,"turn server allocate fail");

		}

		session->server->stunIp=session->ip;
		session->server->stunPort=session->port;

		//yang_turn_handle(TrunRefresh,&request,&buf,tmp,icePwd);
		if(yang_turn_handle(TrunRefresh,session,&buf,tmp,icePwd)!=Yang_Ok){
				return yang_error_wrap(ERROR_RTC_TURN,"turn server permission fail");

			}

		if(yang_turn_handle(TurnPermission,session,&buf,tmp,icePwd)!=Yang_Ok){
			return yang_error_wrap(ERROR_RTC_TURN,"turn server permission fail");

		}

		if(yang_turn_handle(SendIndication,session,&buf,tmp,icePwd)!=Yang_Ok){
			return yang_error_wrap(ERROR_RTC_TURN,"turn server request channelrequest fail");

		}

		if(yang_turn_handle(ChannelBindingRequest,session,&buf,tmp,icePwd)!=Yang_Ok){
			return yang_error_wrap(ERROR_RTC_TURN,"turn server request channelrequest fail");

		}

		return err;
}

int32_t yang_turn_start(YangTurnSession* session){
	int32_t err=Yang_Ok;
	if(session->udp->session.turnBuffer==NULL) session->udp->session.turnBuffer=(char*)calloc(1500,1);
	session->udp->session.receive=g_yang_turn_receive_data;
	session->udp->session.user=session;
	session->udp->sendData=yang_rtc_turn_sendData;

	return err;
}

void yang_create_turnConnection(YangTurnConnection* conn,YangIceServer* server,YangRtcUdp* udp,int32_t localport){
	YangTurnSession* session=&conn->session;
	session->udp=udp;
	session->turn=(YangRtcTurn*)calloc(sizeof(YangRtcTurn),1);
	yang_create_rtcudp(session->udp,localport);
	yang_create_turn(session->turn);
	session->server=server;

	conn->bindingTurnServer=yang_turn_binding;
	conn->start=yang_turn_start;
}
void yang_destroy_turnConnection(YangTurnConnection* conn){
	YangTurnSession* session=&conn->session;
	//session->udp->stop(&session->udp->session);
	//yang_destroy_udp(session->udp);
	//yang_free(session->udp);
	yang_destroy_turn(session->turn);
	yang_free(session->turn);
}

