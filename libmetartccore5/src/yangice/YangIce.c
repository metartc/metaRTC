//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangice/YangIce.h>
#include <yangutil/yangavtype.h>
#include <yangutil/sys/YangEndian.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangSocket.h>

typedef struct{
	YangRtcStun* stun;
	YangIceServer* server;
	uint32_t ip;
	int32_t port;
	yangbool response;
}YangIceStunRequest;

void g_yang_ice_stun_receive(char *data, int32_t size, void *user) {
	if (user == NULL)		return;
	YangIceStunRequest* request=(YangIceStunRequest*)user;

	uint8_t bt=(uint8_t)data[0];
	if(size>0&&(bt==0x00||bt==0x01)){
		int32_t err = 0;

		YangStunPacket response;
		memset(&response,0,sizeof(YangStunPacket));
		if ((err = request->stun->decodeStunServer(&response,data, size)) != 0) {
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
	}

}

int32_t yang_ice_stun_request(YangIceServer* server,int32_t localport){
	char tmp[1024]={0};
	int32_t err=Yang_Ok;
	YangRtcUdp* udp=(YangRtcUdp*)calloc(sizeof(YangRtcUdp),1);
	YangRtcStun* stun=(YangRtcStun*)calloc(sizeof(YangRtcStun),1);
	YangIceStunRequest request;
	request.server=server;
	request.stun=stun;
	request.response=yangfalse;

	yang_create_stun(stun);
	yang_create_rtcudp(udp,localport);
	char serverIp[64]={0};
	yang_getIp(server->serverIp,serverIp);
	udp->updateRemoteAddress(&udp->session,serverIp,server->serverPort);

	udp->session.receive=g_yang_ice_stun_receive;
	udp->session.user=&request;
	udp->start(&udp->session);
	int32_t waitTime=0;

	YangBuffer buf;
	yang_init_buffer(&buf,tmp,1024);
	stun->encodeStunServer(&buf,udp,server->username,"fi1eke1561or19ht73gz448s52yt4270");


	if((err=udp->sendData(&udp->session,buf.data, yang_buffer_pos(&buf)))!=Yang_Ok){
		yang_error("stun server request fail");
		goto fail;
	}

	while(!request.response&&waitTime<500){
		yang_usleep(1000);
		waitTime++;
	}
	if(!request.response) goto fail;
	server->stunIp=request.ip;
	server->stunPort=request.port;

	udp->stop(&udp->session);
	yang_destroy_rtcudp(udp);
	yang_free(udp);

	yang_destroy_stun(stun);
	yang_free(stun);
	return err;

	fail:
		udp->stop(&udp->session);
		yang_destroy_rtcudp(udp);
		yang_free(udp);
		yang_destroy_stun(stun);
		yang_free(stun);
		return 1;
}

int32_t yang_ice_turn_request(YangIceServer* server,int32_t localport){
	char tmp[1024]={0};
	int32_t err=Yang_Ok;
	YangRtcUdp* udp=(YangRtcUdp*)calloc(sizeof(YangRtcUdp),1);
	YangRtcStun* stun=(YangRtcStun*)calloc(sizeof(YangRtcStun),1);
	YangIceStunRequest request;
	request.server=server;
	request.stun=stun;
	request.response=yangfalse;

	yang_create_stun(stun);
	yang_create_rtcudp(udp,localport);
	char serverIp[64]={0};
	yang_getIp(server->serverIp,serverIp);
	udp->updateRemoteAddress(&udp->session,serverIp,server->serverPort);

	udp->session.receive=g_yang_ice_stun_receive;
	udp->session.user=&request;
	udp->start(&udp->session);
	int32_t waitTime=0;

	YangBuffer buf;
	yang_init_buffer(&buf,tmp,1024);
	stun->encodeStunServer(&buf,udp,server->username,"fi1eke1561or19ht73gz448s52yt4270");


	if((err=udp->sendData(&udp->session,buf.data, yang_buffer_pos(&buf)))!=Yang_Ok){
		yang_error("stun server request fail");
		goto fail;
	}

	while(!request.response&&waitTime<500){
		yang_usleep(1000);
		waitTime++;
	}
	if(!request.response) goto fail;
	server->stunIp=request.ip;
	server->stunPort=request.port;

	udp->stop(&udp->session);
	yang_destroy_rtcudp(udp);
	yang_free(udp);

	yang_destroy_stun(stun);
	yang_free(stun);
	return err;

	fail:
		udp->stop(&udp->session);
		yang_destroy_rtcudp(udp);
		yang_free(udp);
		yang_destroy_stun(stun);
		yang_free(stun);
		return 1;
}

void yang_create_ice(YangIce* ice,YangAVInfo* avinfo){
	yang_create_stun(&ice->stun);
	strcpy(ice->server.serverIp,avinfo->rtc.iceServerIP);
	ice->server.serverPort=avinfo->rtc.iceStunPort;
	strcpy(ice->server.username,"metartc");
	ice->requestStun=yang_ice_stun_request;
}

void yang_destroy_ice(YangIce* ice){
	yang_destroy_stun(&ice->stun);
}

