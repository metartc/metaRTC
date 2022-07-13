//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangice/YangRtcStun.h>
#include <yangice/YangRtcStunH.h>
#include <yangrtc/YangRtcConnection.h>
#include <yangrtc/YangRtcUdp.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangutil/sys/YangSsl.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangCString.h>
#include <yangutil/sys/YangSocket.h>
#include <yangutil/sys/YangEndian.h>
#include <yangutil/yangavinfotype.h>
#include <yangutil/buffer/YangBuffer.h>
#include <yangrtc/YangRtcRtcp.h>
#include <stdint.h>
#include <openssl/dh.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/ssl.h>

#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#define be32toh ntohl
#else
#include <arpa/inet.h>
#endif


uint32_t yang_stun_encode_hmac(char* hmac_buf, const int32_t hmac_buf_len,char** str)
{
	char buf[1460];
	uint32_t ret=0;
	YangBuffer stream ;
	yang_init_buffer(&stream,buf, sizeof(buf));

	yang_write_2bytes(&stream,MessageIntegrity);
	yang_write_2bytes(&stream,hmac_buf_len);
	yang_write_bytes(&stream,hmac_buf, hmac_buf_len);
	ret=yang_buffer_pos(&stream);
	*str=(char*)calloc(1,ret+1);
	memcpy(*str,stream.data, ret);
	return ret;

}

uint32_t yang_stun_encode_fingerprint(uint32_t crc32,char** str)
{
	char buf[1460];
	uint32_t ret=0;
	YangBuffer stream ;
	yang_init_buffer(&stream,buf, sizeof(buf));

	yang_write_2bytes(&stream,(int16_t)Fingerprint);
	yang_write_2bytes(&stream,4);
	yang_write_4bytes(&stream,crc32);
	ret=yang_buffer_pos(&stream);
	*str=(char*)calloc(1,ret+1);
	memcpy(*str,stream.data, ret);
	return ret;

}
uint32_t yang_stun_encode_username(YangStunPacket* pkt,char** str)
{
	char buf[1460];
	uint32_t ret=0;
	YangBuffer stream ;
	yang_init_buffer(&stream,buf, sizeof(buf));
	char username[128];
	memset(username,0,128);
	sprintf(username,"%s:%s",pkt->remote_ufrag,pkt->local_ufrag);


	yang_write_2bytes(&stream,Username);
	yang_write_2bytes(&stream,strlen(username));
	yang_write_bytes(&stream,username,strlen(username));


	if (yang_buffer_pos(&stream) % 4 != 0) {
		static char padding[4] = {0};
		yang_write_bytes(&stream,padding, 4 - (yang_buffer_pos(&stream) % 4));
	}
	ret=yang_buffer_pos(&stream);
	*str=(char*)calloc(1,ret+1);

	memcpy(*str,stream.data, ret);
	return ret;

}

uint32_t yang_stun_encode_mapped_address_ipv4(YangStunPacket* pkt,char** str)
{
	char buf[1460];
	uint32_t ret=0;
	YangBuffer stream ;
	yang_init_buffer(&stream,buf, sizeof(buf));
	yang_write_2bytes(&stream,XorMappedAddress);
	yang_write_2bytes(&stream,8);
	yang_write_1bytes(&stream,0); // ignore this bytes
	yang_write_1bytes(&stream,1); // ipv4 family
	yang_write_2bytes(&stream,pkt->mapped_port ^ (kStunMagicCookie >> 16));
	yang_write_4bytes(&stream,pkt->mapped_address ^ kStunMagicCookie);
	ret=yang_buffer_pos(&stream);
	*str=(char*)calloc(1,ret+1);
	memcpy(*str,stream.data, ret);
	return ret;

}

uint32_t yang_stun_encode_mapped_address_ipv6(YangStunPacket* pkt,struct sockaddr_in6* addr,char** str)
{
	char buf[1460];
	uint32_t ret=0;
	YangBuffer stream ;
	yang_init_buffer(&stream,buf, sizeof(buf));
	yang_write_2bytes(&stream,XorMappedAddress);
	yang_write_2bytes(&stream,20);
	yang_write_1bytes(&stream,0); // ignore this bytes
	yang_write_1bytes(&stream,2); // ipv4 family
	yang_write_2bytes(&stream,pkt->mapped_port ^ (kStunMagicCookie >> 16));
	char cookie[4]={0x21,0x12,0xA4,0x42};
	uint8_t* p=(uint8_t*)(&addr->sin6_addr.s6_addr);

	int32_t i=0;
	for(i=0;i<4;i++){
		yang_write_1bytes(&stream,p[i]^ cookie[i]);
	}
	for(i=0;i<12;i++){
		yang_write_1bytes(&stream,p[i+4]^ pkt->transcation_id[i]);
	}
	ret=yang_buffer_pos(&stream);
	*str=(char*)calloc(1,ret+1);
	memcpy(*str,stream.data, ret);
	return ret;

}

int32_t yang_stun_encode_binding_request(YangStunPacket* pkt, YangBuffer* stream,char* pwd,char* username,uint32_t username_len)
{
	int32_t err = Yang_Ok;

	yang_write_2bytes(stream,BindingRequest);
	yang_write_2bytes(stream,username_len);// +mapped_address_len);
	yang_write_4bytes(stream,kStunMagicCookie);
	yang_write_bytes(stream,pkt->transcation_id,strlen(pkt->transcation_id));
	yang_write_bytes(stream,username,username_len);

	char* randstr=(char*)calloc(1,32);//[16];

	//Priority
	yang_cint32_random(30,randstr+1);
	randstr[0]=9;
	uint32_t randint=atoi(randstr);
	yang_write_2bytes(stream,Priority);
	yang_write_2bytes(stream,4);
	yang_write_4bytes(stream,randint);
	//UseCandidate
	yang_write_2bytes(stream,UseCandidate);
	yang_write_2bytes(stream,0);

	//IceControlling
	memset(randstr,0,32);
	yang_cstr_random(8, randstr);
	yang_write_2bytes(stream,(int16_t)IceControlling);
	yang_write_2bytes(stream,8);
	yang_write_bytes(stream,randstr,8);
	yang_free(randstr);

	stream->data[2] = ((yang_buffer_pos(stream) - 20 + 20 + 4 ) & 0x0000FF00) >> 8;
	stream->data[3] = ((yang_buffer_pos(stream) - 20 + 20 + 4 ) & 0x000000FF);
#if Yang_HaveDtls
	char hmac_buf[20] = {0};
	uint32_t  hmac_buf_len = 0;
	if ((err = hmac_encode("sha1", pwd, strlen(pwd), stream->data, yang_buffer_pos(stream), hmac_buf, &hmac_buf_len)) != Yang_Ok) {
		return yang_error_wrap(err, "hmac encode failed");
	}

#else
	char hmac_buf[20]={0x07,0xd4,0x3d,0x32,0xa1,0xd4,0xc1,0xb1,0x9d,0xf5,0xb5,0x56,0xb5,0x56,0x6d,0x20,0x5a,0xda,0xa1,0xac};
	uint32_t  hmac_buf_len = 20;
#endif

	//MessageIntegrity
	char *hmac = NULL;
	uint32_t hmac_len = yang_stun_encode_hmac(hmac_buf, hmac_buf_len, &hmac);
	yang_write_bytes(stream, hmac, hmac_len);
	yang_free(hmac);

	stream->data[2] = ((yang_buffer_pos(stream) - 20 + 8) & 0x0000FF00) >> 8;
	stream->data[3] = ((yang_buffer_pos(stream) - 20 + 8) & 0x000000FF);

	//Fingerprint
	uint32_t crc32 = yang_crc32_ieee(stream->data, yang_buffer_pos(stream), 0)^ 0x5354554E;

	char *fingerprint = NULL;
	uint32_t fingerprint_len = yang_stun_encode_fingerprint(crc32,&fingerprint);
	yang_write_bytes(stream, fingerprint, fingerprint_len);
	yang_free(fingerprint);

	stream->data[2] = ((yang_buffer_pos(stream) - 20) & 0x0000FF00) >> 8;
	stream->data[3] = ((yang_buffer_pos(stream) - 20) & 0x000000FF);


	return err;
}

int32_t yang_decode_stun2( char *buf, const int32_t nb_buf) {
	int32_t err = Yang_Ok;

	YangBuffer stream;
	yang_init_buffer(&stream, (char*) (buf), nb_buf);

	if (yang_buffer_left(&stream) < 20) {
		return yang_error_wrap(ERROR_RTC_STUN,
				"ERROR_RTC_STUN invalid stun packet, size=%d", stream.size);
	}


	yang_buffer_skip(&stream, 2);
	uint16_t message_len = yang_read_2bytes(&stream);

	yang_buffer_skip(&stream, 4);

	yang_buffer_skip(&stream, 12);
	if (nb_buf != 20 + message_len) {
		return yang_error_wrap(ERROR_RTC_STUN,
				"ERROR_RTC_STUN invalid stun packet, message_len=%d, nb_buf=%d",
				message_len, nb_buf);
	}

	while (yang_buffer_left(&stream) >= 4) {
		//uint16_t type = yang_read_2bytes(&stream);
		yang_buffer_skip(&stream, 2);
		uint16_t len = yang_read_2bytes(&stream);

		if (yang_buffer_left(&stream) < len) {
			return yang_error_wrap(ERROR_RTC_STUN, "invalid stun packet");
		}


		yang_buffer_skip(&stream, len);
		// padding
		if (len % 4 != 0) {

			yang_buffer_skip(&stream, 4 - (len % 4));
		}

	}

	return err;
}

void yang_stun_setSessionStun(YangRtcSession *session, char *data, int32_t nb) {
	if (session == NULL)
		return;
	if (session->context.stun.data == NULL)
		session->context.stun.data = (char*) calloc(1, nb);
	memcpy(session->context.stun.data, data, nb);
	session->context.stun.nb = nb;
}



int32_t yang_stun_encode_binding_response(YangStunPacket* pkt,char* pwd, YangBuffer* stream)
{
	int32_t err = Yang_Ok;

	char* property_username=NULL;
	uint32_t property_username_len=yang_stun_encode_username(pkt,&property_username);
	char* mapped_address=NULL;
	uint32_t mapped_address_len=yang_stun_encode_mapped_address_ipv4(pkt,&mapped_address);


	yang_write_2bytes(stream,BindingResponse);
	yang_write_2bytes(stream,property_username_len + mapped_address_len);
	yang_write_4bytes(stream,kStunMagicCookie);
	yang_write_bytes(stream,pkt->transcation_id,12);
	yang_write_bytes(stream,property_username,property_username_len);
	yang_write_bytes(stream,mapped_address,mapped_address_len);

	yang_free(property_username);
	yang_free(mapped_address);
	stream->data[2] = ((yang_buffer_pos(stream) - 20 + 20 + 4) & 0x0000FF00) >> 8;
	stream->data[3] = ((yang_buffer_pos(stream) - 20 + 20 + 4) & 0x000000FF);

#if Yang_HaveDtls
	char hmac_buf[20] = {0};
	uint32_t  hmac_buf_len = 0;
	if ((err = hmac_encode("sha1", pwd, strlen(pwd), stream->data, yang_buffer_pos(stream), hmac_buf, &hmac_buf_len)) != Yang_Ok) {
		return yang_error_wrap(err, "hmac encode failed");
	}
#else
	char hmac_buf[20]={0x07,0xd4,0x3d,0x32,0xa1,0xd4,0xc1,0xb1,0x9d,0xf5,0xb5,0x56,0xb5,0x56,0x6d,0x20,0x5a,0xda,0xa1,0xac};
	uint32_t  hmac_buf_len = 20;
#endif

	char* hmac =NULL;
	uint32_t hmac_len=yang_stun_encode_hmac(hmac_buf, hmac_buf_len,&hmac);
	yang_write_bytes(stream,hmac,hmac_len);
	yang_free(hmac);

	stream->data[2] = ((yang_buffer_pos(stream) - 20 + 8) & 0x0000FF00) >> 8;
	stream->data[3] = ((yang_buffer_pos(stream) - 20 + 8) & 0x000000FF);

	uint32_t crc32 = yang_crc32_ieee(stream->data, yang_buffer_pos(stream), 0) ^ 0x5354554E;

	char* fingerprint =NULL;
	uint32_t fingerprint_len=yang_stun_encode_fingerprint(crc32,&fingerprint);
	yang_write_bytes(stream,fingerprint,fingerprint_len);
	yang_free(fingerprint);

	stream->data[2] = ((yang_buffer_pos(stream) - 20) & 0x0000FF00) >> 8;
	stream->data[3] = ((yang_buffer_pos(stream) - 20) & 0x000000FF);

	return err;
}

int32_t yang_stun_createResponseStunPacket(YangStunPacket* request,void* psession){
	YangRtcSession* session=(YangRtcSession*)psession;
	char s[1024] = { 0 };
	YangBuffer stream;
	yang_init_buffer(&stream,s, 1024);
	YangStunPacket packet;
	memset(&packet,0,sizeof(YangStunPacket));

#ifdef _WIN32
	packet.mapped_address=session->context.udp->session.remote_addr.sin_addr.S_un.S_addr;
#else
	packet.mapped_address=session->context.udp->session.remote_addr.sin_addr.s_addr;
#endif

	packet.mapped_port=session->context.udp->session.remote_addr.sin_port;
	memcpy(packet.transcation_id,request->transcation_id,12);
	strcpy(packet.local_ufrag,session->local_ufrag);
	strcpy(packet.remote_ufrag,session->remote_ufrag);
	yang_stun_encode_binding_response(&packet,session->localIcePwd,&stream);

	return session->context.udp->sendData(&session->context.udp->session,stream.data, yang_buffer_pos(&stream));
}

uint32_t yang_stun_encode_username2(YangStunPacket* pkt,char** str,char* username)
{
	char buf[1460];
	uint32_t ret=0;
	YangBuffer stream ;
	yang_init_buffer(&stream,buf, sizeof(buf));

	yang_write_2bytes(&stream,Username);
	yang_write_2bytes(&stream,strlen(username));
	yang_write_bytes(&stream,username,strlen(username));


	if (yang_buffer_pos(&stream) % 4 != 0) {
		static char padding[4] = {0};
		yang_write_bytes(&stream,padding, 4 - (yang_buffer_pos(&stream) % 4));
	}
	ret=yang_buffer_pos(&stream);
	*str=(char*)calloc(1,ret+1);

	memcpy(*str,stream.data, ret);
	return ret;

}


int32_t yang_decode_stun(YangStunPacket* pkt,char* buf, const int32_t nb_buf)
{
	int32_t err = Yang_Ok;

	YangBuffer stream ;
	yang_init_buffer(&stream,buf, nb_buf);


	if (yang_buffer_left(&stream) < 20) {
		return yang_error_wrap(ERROR_RTC_STUN, "ERROR_RTC_STUN invalid stun packet, size=%d", stream.size);
	}

	pkt->message_type = yang_read_2bytes(&stream);
	uint16_t message_len = yang_read_2bytes(&stream);
	char magic_cookie[4] ;
	yang_read_bytes(&stream,magic_cookie,4);


	yang_read_bytes(&stream,pkt->transcation_id,12);

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
			// yang_read_string(&stream,4 - (len % 4));
			yang_buffer_skip(&stream,4 - (len % 4));
			//yang_read_bytes(&stream,4 - (len % 4));
		}

		switch (type) {
		case Username: {
			memcpy(pkt->username,val,len);
			char* p = strtok(val,":");
			if(p) {
				strcpy(pkt->local_ufrag,p);
				p=strtok(NULL,":");
				if(p) {
					strcpy(pkt->remote_ufrag ,p);
				}
			}
			break;
		}

		case UseCandidate: {
			pkt->use_candidate = true;
			//yang_trace("stun use-candidate");
			break;
		}
		// @see: https://tools.ietf.org/html/draft-ietf-ice-rfc5245bis-00#section-5.1.2
		// One agent full, one lite:  The full agent MUST take the controlling
		// role, and the lite agent MUST take the controlled role.  The full
		// agent will form check lists, run the ICE state machines, and
		// generate connectivity checks.
		case IceControlled: {
			pkt->ice_controlled = true;
			//yang_trace("stun ice-controlled");
			break;
		}

		case IceControlling: {
			pkt->ice_controlling = true;
			//yang_trace("stun ice-controlling");
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


int32_t yang_decode_rtcstun(YangStunPacket* pkt,char* buf, const int32_t nb_buf)
{
	int32_t err = Yang_Ok;

	YangBuffer stream ;
	yang_init_buffer(&stream,buf, nb_buf);


	if (yang_buffer_left(&stream) < 20) {
		return yang_error_wrap(ERROR_RTC_STUN, "ERROR_RTC_STUN invalid stun packet, size=%d", stream.size);
	}

	pkt->message_type = yang_read_2bytes(&stream);
	uint16_t message_len = yang_read_2bytes(&stream);
	char magic_cookie[4] ;
	yang_read_bytes(&stream,magic_cookie,4);

	yang_read_bytes(&stream,pkt->transcation_id,12);

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
			// yang_read_string(&stream,4 - (len % 4));
			yang_buffer_skip(&stream,4 - (len % 4));
			//yang_read_bytes(&stream,4 - (len % 4));
		}


		switch (type) {
		case Username: {
			memcpy(pkt->username,val,len);
			char* p = strtok(val,":");
			if(p) {
				strcpy(pkt->local_ufrag,p);
				p=strtok(NULL,":");
				if(p) {
					strcpy(pkt->remote_ufrag ,p);
				}
			}
			break;
		}

		case UseCandidate: {
			pkt->use_candidate = true;
			//yang_trace("stun use-candidate");
			break;
		}
		case XorMappedAddress:{
			//family=*(val+1)==1?IPv4:IPv6;
			pkt->mapped_port=yang_get_be16((uint8_t*)val+2)^ ((uint16_t)(kStunMagicCookie >> 16));
			pkt->mapped_address=yang_get_be32((uint8_t*)val+4)^ kStunMagicCookie;
			break;
		}
		// @see: https://tools.ietf.org/html/draft-ietf-ice-rfc5245bis-00#section-5.1.2
		// One agent full, one lite:  The full agent MUST take the controlling
		// role, and the lite agent MUST take the controlled role.  The full
		// agent will form check lists, run the ICE state machines, and
		// generate connectivity checks.
		case IceControlled: {
			pkt->ice_controlled = true;
			//yang_trace("stun ice-controlled");
			break;
		}

		case IceControlling: {
			pkt->ice_controlling = true;
			//yang_trace("stun ice-controlling");
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



int32_t yang_stun_encode_binding_request2(YangStunPacket* pkt, YangBuffer* stream,YangRtcUdp* udp,char* pwd,char* username,uint32_t username_len){
	int32_t err = Yang_Ok;

	yang_write_2bytes(stream,BindingRequest);
	yang_write_2bytes(stream,username_len);// +mapped_address_len);
	yang_write_4bytes(stream,kStunMagicCookie);
	yang_write_bytes(stream,pkt->transcation_id,strlen(pkt->transcation_id));
	yang_write_bytes(stream,username,username_len);

	char* randstr=(char*)calloc(1,32);//[16];

	//Priority
	yang_cint32_random(30,randstr+1);
	randstr[0]=9;
	uint32_t randint=atoi(randstr);
	yang_write_2bytes(stream,Priority);
	yang_write_2bytes(stream,4);
	yang_write_4bytes(stream,randint);

	//IceControlling
	memset(randstr,0,32);
	yang_cstr_random(8, randstr);
	yang_write_2bytes(stream,(int16_t)IceControlling);
	yang_write_2bytes(stream,8);
	yang_write_bytes(stream,randstr,8);
	yang_free(randstr);

	stream->data[2] = ((yang_buffer_pos(stream) - 20 + 20 + 4 ) & 0x0000FF00) >> 8;
	stream->data[3] = ((yang_buffer_pos(stream) - 20 + 20 + 4 ) & 0x000000FF);

	char hmac_buf[20] = {0};
	uint32_t  hmac_buf_len = 0;
	if ((err = hmac_encode("sha1", pwd, strlen(pwd), stream->data, yang_buffer_pos(stream), hmac_buf, &hmac_buf_len)) != Yang_Ok) {
		return yang_error_wrap(err, "hmac encode failed");
	}



	//MessageIntegrity
	char *hmac = NULL;
	uint32_t hmac_len = yang_stun_encode_hmac(hmac_buf, hmac_buf_len, &hmac);
	yang_write_bytes(stream, hmac, hmac_len);
	yang_free(hmac);

	stream->data[2] = ((yang_buffer_pos(stream) - 20 + 8) & 0x0000FF00) >> 8;
	stream->data[3] = ((yang_buffer_pos(stream) - 20 + 8) & 0x000000FF);

	//Fingerprint
	uint32_t crc32 = yang_crc32_ieee(stream->data, yang_buffer_pos(stream), 0)^ 0x5354554E;

	char *fingerprint = NULL;
	uint32_t fingerprint_len = yang_stun_encode_fingerprint(crc32,&fingerprint);
	yang_write_bytes(stream, fingerprint, fingerprint_len);
	yang_free(fingerprint);

	stream->data[2] = ((yang_buffer_pos(stream) - 20) & 0x0000FF00) >> 8;
	stream->data[3] = ((yang_buffer_pos(stream) - 20) & 0x000000FF);


	return err;
}

int32_t yang_stun_createRequestStunPacket(void* psession,char* ice_pwd){
	YangRtcSession* session=(YangRtcSession*)psession;
	char s[1024] = { 0 };
	YangBuffer stream;
	yang_init_buffer(&stream,s, 1024);
	char tid[13];
	memset(tid,0,13);
	yang_cstr_random(12,tid);
	if(strlen(session->context.avinfo->sys.localIp)==0){
		memset(session->context.avinfo->sys.localIp,0,sizeof(session->context.avinfo->sys.localIp));
		yang_getLocalInfo(session->context.avinfo->sys.localIp);
	}
	uint32_t addr = be32toh(inet_addr(session->context.avinfo->sys.localIp));
	YangStunPacket packet;
	memset(&packet,0,sizeof(YangStunPacket));
	packet.message_type=BindingRequest;
	strcpy(packet.local_ufrag,session->local_ufrag);
	strcpy(packet.remote_ufrag,session->remote_ufrag);

	strcpy(packet.transcation_id,tid);
	packet.mapped_address=addr;
	packet.mapped_port=session->context.udp->session.local_addr.sin_port;//session->context.streamConf->localPort;
	char* property_username=NULL;
	uint32_t property_username_len=yang_stun_encode_username(&packet,&property_username);
	yang_stun_encode_binding_request(&packet,&stream,ice_pwd,property_username,property_username_len);
	yang_free(property_username);
	//add session stun data
	yang_stun_setSessionStun(session,stream.data,yang_buffer_pos(&stream));

	return Yang_Ok;
}

int32_t yang_stun_encode(YangStunMessageType stunType,YangBuffer* stream,void* pudp,char* username,char* ice_pwd){
	YangRtcUdp* udp=(YangRtcUdp*)pudp;

	char tid[13];
	memset(tid,0,13);
	yang_cstr_random(12,tid);
	char localIp[128]={0};
	yang_getLocalInfo(localIp);

	uint32_t addr = be32toh(inet_addr(localIp));
	YangStunPacket packet;
	memset(&packet,0,sizeof(YangStunPacket));
	packet.message_type=stunType;//BindingRequest;


	strcpy(packet.transcation_id,tid);
	packet.mapped_address=addr;
	packet.mapped_port=udp->session.local_addr.sin_port;

	char* property_username=NULL;
	uint32_t property_username_len=yang_stun_encode_username(&packet,&property_username);
	yang_stun_encode_binding_request2(&packet,stream,udp,ice_pwd,property_username,property_username_len);
	yang_free(property_username);

	return Yang_Ok;
}


int32_t yang_stun_encode_request(YangBuffer* stream,void* pudp,char* username,char* ice_pwd){
	return yang_stun_encode(BindingRequest,stream,pudp,username,ice_pwd);
}


void yang_create_stun(YangRtcStun* stun){
	stun->encodeStunServer=yang_stun_encode_request;
	stun->decodeStunServer=yang_decode_rtcstun;
	stun->decode=yang_decode_stun;
	stun->createResponseStunPacket=yang_stun_createResponseStunPacket;
	stun->createRequestStunPacket=yang_stun_createRequestStunPacket;
	stun->decode2=yang_decode_stun2;

}
void yang_destroy_stun(YangRtcStun* stun){

}



