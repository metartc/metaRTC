//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangice/YangRtcTurn.h>
#include <yangice/YangRtcStunH.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangEndian.h>
#include <yangutil/sys/YangCString.h>
#include <yangssl/YangSsl.h>

yang_vector_impl2(YangTurnPeer)

int32_t yang_turn_encode_header(YangStunPacket* pkt, YangBuffer* stream,int32_t len){

	yang_write_2bytes(stream,pkt->message_type);
	yang_write_2bytes(stream,len);//
	yang_write_4bytes(stream,kStunMagicCookie);
	yang_cstr_random(YangStunTranscationIdLen,pkt->transcation_id);
	yang_write_bytes(stream,pkt->transcation_id,strlen(pkt->transcation_id));

	return Yang_Ok;
}


void yang_turn_addFingerprint(YangTurnSession* session,YangBuffer* stream,yangbool addFingerprint){
	uint32_t ret=0;
    stream->data[2] = ((yang_buffer_pos(stream) - 20 + 24) & 0x0000FF00) >> 8;
    stream->data[3] = ((yang_buffer_pos(stream) - 20 + 24) & 0x000000FF);

    char keystr[128]={0};
    char key[128]={0};
    sprintf(keystr,"%s:%s:%s",session->server->username,session->realm,session->server->password);

    yang_ssl_md5((const uint8_t*)keystr,strlen(keystr),(uint8_t*)key);
	//MessageIntegrity
	char *hmac = NULL;
	 ret = yang_stun_encode_hmac(stream, key, &hmac);
	yang_write_bytes(stream, hmac, ret);
	yang_free(hmac);

	if(addFingerprint){
		stream->data[2] = ((yang_buffer_pos(stream) - 20 + 8) & 0x0000FF00) >> 8;
		stream->data[3] = ((yang_buffer_pos(stream) - 20 + 8) & 0x000000FF);

		char *fingerprint = NULL;
		 ret = yang_stun_encode_fingerprint(stream,&fingerprint);
		yang_write_bytes(stream, fingerprint, ret);
		yang_free(fingerprint);
	}
}

int32_t yang_turn_encode_allocate(YangTurnSession* session,uint8_t protocol,yangbool beforCredential){

	uint32_t ret=0;
	char s[1024] = { 0 };
	YangBuffer stream;
	yang_init_buffer(&stream,s, 1024);

	YangStunPacket packet;
	memset(&packet,0,sizeof(YangStunPacket));
	packet.message_type=StunAllocate;

	memset(&session->response,0,sizeof(YangTurnResponseData));

	yang_turn_encode_header(&packet,&stream,0);



	char* transportStr=NULL;
	ret=yang_stun_encode_transport(protocol,&transportStr);
	yang_write_bytes(&stream,transportStr,ret);
	yang_free(transportStr);


	if(!beforCredential){
#if 0
		int32_t lifetime=session->lifetime;
		char* lifetimeStr=NULL;
		ret=yang_stun_encode_lifetime(lifetime,&lifetimeStr);
		yang_write_bytes(&stream,lifetimeStr,ret);
		yang_free(lifetimeStr);
#endif

		char* username=NULL;
		ret=yang_stun_encode_username(session->server->username,&username);
		yang_write_bytes(&stream,username,ret);
		yang_free(username);



		char* realm=NULL;
		ret=yang_stun_encode_realm(session->realm,&realm);
		yang_write_bytes(&stream,realm,ret);
		yang_free(realm);

		char* nonce=NULL;
		ret=yang_stun_encode_nonce((char*)session->nonce,session->nonceLen,&nonce);
		yang_write_bytes(&stream,nonce,ret);
		yang_free(nonce);

		yang_turn_addFingerprint(session,&stream,yangfalse);
	}



	stream.data[2] = ((yang_buffer_pos(&stream) - 20) & 0x0000FF00) >> 8;
	stream.data[3] = ((yang_buffer_pos(&stream) - 20) & 0x000000FF);

	return session->udp->sendData(&session->udp->session,s,yang_buffer_pos(&stream));
}


int32_t yang_turn_encode_refresh(YangTurnSession* session,yangbool isClose){

	uint32_t ret=0;
	char s[1024] = { 0 };
	YangBuffer stream;
	yang_init_buffer(&stream,s, 1024);

	YangStunPacket packet;
	memset(&packet,0,sizeof(YangStunPacket));
	packet.message_type=StunRefresh;

	memset(&session->response,0,sizeof(YangTurnResponseData));

	yang_turn_encode_header(&packet,&stream,0);

	if(isClose){
		int32_t lifetime=0;
		char* lifetimeStr=NULL;
		ret=yang_stun_encode_lifetime(lifetime,&lifetimeStr);
		yang_write_bytes(&stream,lifetimeStr,ret);
		yang_free(lifetimeStr);
	}

	char* username=NULL;
	ret=yang_stun_encode_username(session->server->username,&username);
	yang_write_bytes(&stream,username,ret);
	yang_free(username);

	char* realm=NULL;
	ret=yang_stun_encode_realm(session->realm,&realm);
	yang_write_bytes(&stream,realm,ret);
	yang_free(realm);

	char* nonce=NULL;
	ret=yang_stun_encode_nonce((char*)session->nonce,session->nonceLen,&nonce);
	yang_write_bytes(&stream,nonce,ret);
	yang_free(nonce);

	yang_turn_addFingerprint(session,&stream,yangfalse);

	stream.data[2] = ((yang_buffer_pos(&stream) - 20) & 0x0000FF00) >> 8;
	stream.data[3] = ((yang_buffer_pos(&stream) - 20) & 0x000000FF);

	return session->udp->sendData(&session->udp->session,s,yang_buffer_pos(&stream));
}

int32_t yang_turn_encode_permission(YangTurnSession* session,YangTurnPeer* peer){

	uint32_t ret=0;
	char s[1024] = { 0 };
	YangBuffer stream;
	yang_init_buffer(&stream,s, 1024);

	YangStunPacket packet;
	memset(&packet,0,sizeof(YangStunPacket));
	packet.message_type=StunPermission;

	memset(&session->response,0,sizeof(YangTurnResponseData));

	yang_turn_encode_header(&packet,&stream,0);

	char* hostStr=NULL;
	ret=yang_stun_encode_peer_address_ipv4(peer->address.mapAddress,peer->address.port,&hostStr);
	yang_write_bytes(&stream,hostStr,ret);
	yang_free(hostStr);


	char* username=NULL;
	ret=yang_stun_encode_username(session->server->username,&username);
	yang_write_bytes(&stream,username,ret);
	yang_free(username);

	char* realm=NULL;
	ret=yang_stun_encode_realm(session->realm,&realm);
	yang_write_bytes(&stream,realm,ret);
	yang_free(realm);

	char* nonce=NULL;
	ret=yang_stun_encode_nonce((char*)session->nonce,session->nonceLen,&nonce);
	yang_write_bytes(&stream,nonce,ret);
	yang_free(nonce);

	yang_turn_addFingerprint(session,&stream,yangfalse);

	stream.data[2] = ((yang_buffer_pos(&stream) - 20) & 0x0000FF00) >> 8;
	stream.data[3] = ((yang_buffer_pos(&stream) - 20) & 0x000000FF);
	return session->udp->sendData(&session->udp->session,s,yang_buffer_pos(&stream));
}


int32_t yang_turn_encode_bindChannel(YangTurnSession* session,YangTurnPeer* peer){

	uint32_t ret=0;
	char s[1024] = { 0 };
	YangBuffer stream;
	yang_init_buffer(&stream,s, 1024);

	YangStunPacket packet;
	memset(&packet,0,sizeof(YangStunPacket));
	packet.message_type=StunChannelBindingRequest;

	yang_turn_encode_header(&packet,&stream,0);

	memset(&session->response,0,sizeof(YangTurnResponseData));

	uint16_t channelNum=peer->channelNumber;
	char* chnnelNumStr=NULL;
	ret=yang_stun_encode_channelNumber(channelNum,&chnnelNumStr);
	yang_write_bytes(&stream,chnnelNumStr,ret);
	yang_free(chnnelNumStr);

	char* hostStr=NULL;
	ret=yang_stun_encode_peer_address_ipv4(peer->address.mapAddress,peer->address.port,&hostStr);
	yang_write_bytes(&stream,hostStr,ret);
	yang_free(hostStr);

	char* username=NULL;
	ret=yang_stun_encode_username(session->server->username,&username);
	yang_write_bytes(&stream,username,ret);
	yang_free(username);

	char* realm=NULL;
	ret=yang_stun_encode_realm(session->realm,&realm);
	yang_write_bytes(&stream,realm,ret);
	yang_free(realm);

	char* nonce=NULL;
	ret=yang_stun_encode_nonce((char*)session->nonce,session->nonceLen,&nonce);
	yang_write_bytes(&stream,nonce,ret);
	yang_free(nonce);

    yang_turn_addFingerprint(session,&stream,yangfalse);

	stream.data[2] = ((yang_buffer_pos(&stream) - 20) & 0x0000FF00) >> 8;
	stream.data[3] = ((yang_buffer_pos(&stream) - 20) & 0x000000FF);
	return session->udp->sendData(&session->udp->session,s,yang_buffer_pos(&stream));
}




int32_t yang_turn_encode_sendIndication(YangTurnSession* session,YangTurnPeer* peer,char* data){

	uint32_t ret=0;
	char s[1024] = { 0 };
	YangBuffer stream;
	yang_init_buffer(&stream,s, 1024);

	YangStunPacket packet;
	memset(&packet,0,sizeof(YangStunPacket));
	packet.message_type=StunSendIndication;

	memset(&session->response,0,sizeof(YangTurnResponseData));

	yang_turn_encode_header(&packet,&stream,0);

	char* hostStr=NULL;
	ret=yang_stun_encode_peer_address_ipv4(peer->address.mapAddress,peer->address.port,&hostStr);
	yang_write_bytes(&stream,hostStr,ret);
	yang_free(hostStr);


	char* tdata=NULL;
	ret=yang_stun_encode_data(data,&tdata);
	yang_write_bytes(&stream,tdata,ret);
	yang_free(tdata);

	stream.data[2] = ((yang_buffer_pos(&stream) - 20) & 0x0000FF00) >> 8;
	stream.data[3] = ((yang_buffer_pos(&stream) - 20) & 0x000000FF);

	return session->udp->sendData(&session->udp->session,s,yang_buffer_pos(&stream));
}


int32_t yang_turn_encode_request(YangTurnSession* session){
	char tmp[1024]={0};


	YangBuffer stream;
	yang_init_buffer(&stream,tmp,1024);

	 yang_stun_encode_request(&stream,session->udp,session->server->username,session->server->password);
	 return session->udp->sendData(&session->udp->session,stream.data,yang_buffer_pos(&stream));

}



int32_t yang_turn_decode(YangTurnSession* session,char* buf, const int32_t nb_buf)
{
	int32_t err = Yang_Ok;

	YangBuffer stream ;
	yang_init_buffer(&stream,buf, nb_buf);


	if (yang_buffer_left(&stream) < 20) {
		return yang_error_wrap(ERROR_RTC_STUN, "ERROR_RTC_STUN invalid stun packet, size=%d", stream.size);
	}

	//pkt->message_type = yang_read_2bytes(&stream);
	yang_buffer_skip(&stream,2);
	uint16_t message_len = yang_read_2bytes(&stream);
	char magic_cookie[4] ;
	yang_read_bytes(&stream,magic_cookie,4);


	//yang_read_bytes(&stream,pkt->transcation_id,12);
	yang_buffer_skip(&stream,12);
	if (nb_buf != 20 + message_len) {
		return yang_error_wrap(ERROR_RTC_STUN, "ERROR_RTC_STUN invalid stun packet, message_len=%d, nb_buf=%d", message_len, nb_buf);
	}

	while (yang_buffer_left(&stream) >= 4) {
		uint16_t type = yang_read_2bytes(&stream);
		uint16_t len = yang_read_2bytes(&stream);

		if (yang_buffer_left(&stream)  < len) {
			return yang_error_wrap(ERROR_RTC_STUN, "invalid stun packet");
		}

		char* val=(char*)calloc(len+1,1);
		yang_read_bytes(&stream,val,len);
		// padding
		if (len % 4 != 0) {
			yang_buffer_skip(&stream,4 - (len % 4));
		}

		switch (type) {
		case StunErrorCode:
		{
			uint8_t stunClass=*(val+2);
			uint8_t stunCode=*(val+3);
			session->response.errorCode=100*stunClass+stunCode;
			break;
		}
		case StunXorRelayedAddress:{
			session->response.mappedPort=yang_get_be16((uint8_t*)val+2)^ ((uint16_t)(kStunMagicCookie >> 16));
			session->response.mappedAddress=yang_get_be32((uint8_t*)val+4)^ kStunMagicCookie;
			break;
		}
		case StunLifetime: {
			session->response.lifetime = yang_get_be32((uint8_t*)val);
			break;
		}
		case StunRealm: {
			if(strlen(session->realm)) break;
			memset(session->realm,0,sizeof(session->realm));
			memcpy(session->realm,val,len);
			break;
		}
		case StunNonce: {
			if(session->nonceLen) break;
			memset(session->response.nonce,0,sizeof(session->response.nonce));
			session->response.nonceLen=len;
			memcpy(session->response.nonce,val,len);
			break;
		}
		case StunData: {
			yang_trace("recv turn data=%s",val);
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


