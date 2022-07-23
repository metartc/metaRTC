//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangice/YangRtcTurn.h>
#include <yangice/YangRtcStunH.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangEndian.h>
int32_t yang_turn_encode_request(YangBuffer* stream,void* pudp,char* username,char* ice_pwd){
	return yang_stun_encode(BindingRequest,stream,pudp,username,ice_pwd);
}

int32_t yang_turn_encode_allocate(YangBuffer* stream,void* pudp,char* username,char* ice_pwd){
	return yang_stun_encode(TrunAllocate,stream,pudp,username,ice_pwd);
}

int32_t yang_turn_encode_channelRequest(YangBuffer* stream,void* pudp,char* username,char* ice_pwd){
	return yang_stun_encode(ChannelBindingRequest,stream,pudp,username,ice_pwd);
}

int32_t yang_turn_encode_send(YangBuffer* stream,void* pudp,char* username,char* ice_pwd){
	return yang_stun_encode(TrunSend,stream,pudp,username,ice_pwd);
}

int32_t yang_turn_encode_sendIndication(YangBuffer* stream,void* pudp,char* username,char* ice_pwd){
	return yang_stun_encode(SendIndication,stream,pudp,username,ice_pwd);
}

int32_t yang_turn_encode_dataIndication(YangBuffer* stream,void* pudp,char* username,char* ice_pwd){
	return yang_stun_encode(DataIndication,stream,pudp,username,ice_pwd);
}

int32_t yang_turn_encode_permission(YangBuffer* stream,void* pudp,char* username,char* ice_pwd){
	return yang_stun_encode(TurnPermission,stream,pudp,username,ice_pwd);
}

int32_t yang_turn_encode_refresh(YangBuffer* stream,void* pudp,char* username,char* ice_pwd){
	return yang_stun_encode(TrunRefresh,stream,pudp,username,ice_pwd);
}


int32_t yang_decode_turn(YangStunPacket* pkt,char* buf, const int32_t nb_buf)
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

void yang_create_turn(YangRtcTurn* turn){
	turn->request=yang_turn_encode_request;
	turn->allocate=yang_turn_encode_allocate;
	turn->channelRequest=yang_turn_encode_channelRequest;
	turn->channelSend=yang_turn_encode_send;
	turn->dataIndication=yang_turn_encode_dataIndication;
	turn->permission=yang_turn_encode_permission;
	turn->refresh=yang_turn_encode_refresh;
	turn->sendIndication=yang_turn_encode_sendIndication;

	turn->decode=yang_decode_turn;
}
void yang_destroy_turn(YangRtcTurn* turn){

}
