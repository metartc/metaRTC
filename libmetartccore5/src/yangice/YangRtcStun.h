//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YangStunPacket_H__
#define YangStunPacket_H__

#include <stdint.h>
#include <string.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangutil/buffer/YangBuffer.h>
#include <yangutil/sys/YangSsrc.h>
/**

**/
// @see: https://tools.ietf.org/html/rfc5389
// The magic cookie field MUST contain the fixed value 0x2112A442 in network byte order
#define  kStunMagicCookie  (uint32_t)0x2112A442

typedef enum YangStunMessageType
{
	// see @ https://tools.ietf.org/html/rfc3489#section-11.1	
    BindingRequest            = 0x0001,
	TrunAllocate              = 0x0003,
	TrunRefresh               = 0x0004,
	TrunSend                  = 0x0006,
	TrunData                  = 0x0007,
    TurnPermission            = 0x0008,
	ChannelBindingRequest     = 0x0009,
    BindingIndication 		  = 0x0011,
	SendIndication            = 0x0016,
    DataIndication            = 0x0017,
    BindingResponse           = 0x0101,
    BindingErrorResponse      = 0x0111,

    SharedSecretRequest       = 0x0002,
    SharedSecretResponse      = 0x0102,
    SharedSecretErrorResponse = 0x0112,
}YangStunMessageType;

typedef enum
{
    // see @ https://tools.ietf.org/html/rfc3489#section-11.2
	MappedAddress     = 0x0001,
   	ResponseAddress   = 0x0002,
   	ChangeRequest     = 0x0003,
   	SourceAddress     = 0x0004,
   	ChangedAddress    = 0x0005,
   	Username          = 0x0006,
   	Password          = 0x0007,
   	MessageIntegrity  = 0x0008,
   	ErrorCode         = 0x0009,
   	UnknownAttributes = 0x000A,
   	ReflectedFrom     = 0x000B,

    // see @ https://tools.ietf.org/html/rfc5389#section-18.2
    Realm             = 0x0014,
    Nonce             = 0x0015,
    XorMappedAddress  = 0x0020,
    Software          = 0x8022,
    AlternateServer   = 0x8023,
    Fingerprint      = 0x8028,

    Priority          = 0x0024,
    UseCandidate      = 0x0025,
    IceControlled     = 0x8029,
    IceControlling    = 0x802A,
}YangStunMessageAttribute;

typedef struct{
	uint16_t message_type;
	uint16_t mapped_port;
	int16_t use_candidate;
	int16_t ice_controlled;
	int32_t ice_controlling;
	uint32_t mapped_address;
	char local_ufrag[16];
	char remote_ufrag[32];
	char transcation_id[13];
	char username[32];
}YangStunPacket;

typedef struct{
	YangStunPacket packet;
	int32_t (*encodeStunServer)(YangBuffer* stream,void* pudp,char* username,char* ice_pwd);
	int32_t (*decodeStunServer)(YangStunPacket* pkt,char* buf, const int32_t nb_buf);
	int32_t (*decode)(YangStunPacket* pkt, char* buf,  int32_t nb_buf);
	int32_t (*decode2)( char* buf,  int32_t nb_buf);
	int32_t (*createResponseStunPacket)(YangStunPacket* request,void* session);
	int32_t (*createRequestStunPacket)(void* psession,char* ice_pwd);
}YangRtcStun;

void yang_create_stun(YangRtcStun* stun);
void yang_destroy_stun(YangRtcStun* stun);

#endif
