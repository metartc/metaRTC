//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGICE_YANGRTCSTUNH_H_
#define SRC_YANGICE_YANGRTCSTUNH_H_
#include <yangice/YangRtcTurn.h>


int32_t yang_stun_encode(YangStunMessageType stunType,YangBuffer* stream,void* pudp,char* username,char* ice_pwd);
uint32_t yang_stun_encode_hmac2(char* hmac_buf, const int32_t hmac_buf_len,char** str);
uint32_t yang_stun_encode_fingerprint2(uint32_t crc32,char** str);

uint32_t yang_stun_encode_username(char* username,char** str);
uint32_t yang_stun_encode_password(char* password,char** str);
uint32_t yang_stun_encode_transport(uint8_t protocol,char** str);
uint32_t yang_stun_encode_realm(char* realm,char** str);
uint32_t yang_stun_encode_nonce(char* nonce,uint16_t len,char** str);
uint32_t yang_stun_encode_lifetime(int32_t lifetime,char** str);
uint32_t yang_stun_encode_channelNumber(uint16_t channelNum,char** str);
uint32_t yang_stun_encode_peer_address_ipv4(uint32_t address,uint16_t port,char** str);
uint32_t yang_stun_encode_hmac(YangBuffer *pstream,char* pwd,char** str);
uint32_t yang_stun_encode_fingerprint(YangBuffer *pstream,char** str);

int32_t yang_turn_decode(YangTurnSession* session,char* buf, const int32_t nb_buf);


uint32_t yang_stun_encode_mapped_address_ipv4(YangStunPacket* pkt,char** str);
//uint32_t yang_stun_encode_mapped_address_ipv6(YangStunPacket* pkt,struct sockaddr_in6* addr,char** str);
int32_t yang_stun_encode_request(YangBuffer* stream,void* pudp,char* username,char* ice_pwd);

int32_t yang_turn_encode_allocate(YangTurnSession* session,uint8_t protocol,yangbool beforCredential);
int32_t yang_turn_encode_refresh(YangTurnSession* session);
int32_t yang_turn_encode_permission(YangTurnSession* session,YangTurnPeer* peer);
int32_t yang_turn_encode_bindChannel(YangTurnSession* session,YangTurnPeer* peer);
int32_t yang_turn_encode_request(YangTurnSession* session);

#endif /* SRC_YANGICE_YANGRTCSTUNH_H_ */
