//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef INCLUDE_YANGUTIL_YANGERRORCODE_H_
#define INCLUDE_YANGUTIL_YANGERRORCODE_H_

#define Yang_Ok 0
#define Yang_LeaveRoom 1

#define ERROR_CODEC_Encode 25
#define ERROR_CODEC_Encode_Video 26
#define ERROR_CODEC_Decode 31
#define ERROR_CODEC_Decode_Audio 32
#define ERROR_CODEC_Decode_Audio_NotInit 33

#define ERROR_SYS_NoAudioDevice 110
#define ERROR_SYS_NoVideoDevice 111
#define ERROR_SYS_NoAudioCaptureDevice 112
#define ERROR_SYS_NoAudioPlayDevice 113
#define ERROR_SYS_AudioCapture 114
#define ERROR_SYS_AudioRender 115
#define ERROR_SYS_VideoCapture 116

#define ERROR_SYS_Linux_VideoDeveceOpenFailure 120
#define ERROR_SYS_Linux_NoVideoDriver 122
#define ERROR_SYS_Linux_NoVideoCatpureInterface 124

#define ERROR_SYS_Win_VideoDeveceOpenFailure 150
#define ERROR_SYS_Win_NoVideoDriver 151
#define ERROR_SYS_Win_NoVideoCatpureInterface 152

#define ERROR_SYS_Linux_ScreenDeviceOpenFailure 163
#define ERROR_SYS_Linux_ScreenCaptureFailure 164
#define ERROR_SYS_Win_ScreenCaptureFailure 165


#define ERROR_SOCKET 201
#define ERROR_SOCKET_Timeout 202
#define ERROR_SOCKET_Close 203
#define ERROR_SOCKET_Close_Wr 204
#define ERROR_SOCKET_Negotiation 205
#define ERROR_CONNECT_Handshaked 206
#define ERROR_CONNECT_Fail 207
#define ERROR_REQUEST_Close 208
#define ERROR_SERVER_Reject 209
#define ERROR_SERVER_Logout 210
#define ERROR_SERVER_ConnectFailure 211

#define ERROR_TLS 300

#define ERROR_STRING               300

#define ERROR_JSON					320

#define ERROR_SSL 330




#define ERROR_RTMP_ConnectFailure 2100
#define ERROR_RTMP_PubFailure 2101
#define ERROR_RTMP_UnPubFailure 2102
#define ERROR_RTMP_PlayFailure 2103
#define ERROR_RTMP_UnPlayFailure 2104
#define ERROR_RTMP_SendCommandFailure 2105
#define ERROR_RTMP_UserInfosResult 2106

#define Yang_SRTS_SocketBase 3000
#define ERROR_SRT_NotInit 3001
#define Yang_SRTS_CONNECTING 3004
#define Yang_SRTS_BROKEN 3006
#define Yang_SRTS_CLOSING 3007
#define Yang_SRTS_CLOSED 3008
#define Yang_SRTS_NONEXIST 3009
#define Yang_SRTS_NONSRTSERVER 3010
#define ERROR_SRT_PushFailure 3100
#define ERROR_SRT_PullFailure 3101
#define ERROR_SRT_StreamIdSetFailure 3102
#define ERROR_SRT_EpollCreateFailure 3103
#define ERROR_SRT_EpollSetFailure 3104
#define ERROR_SRT_EpollSelectFailure 3105
#define ERROR_SRT_SocketConnectCreate 3201
#define ERROR_SRT_SocketConnect 3202
#define ERROR_SRT_WriteSocket 3210
#define ERROR_SRT_ReadSocket 3211

#define ERROR_P2P                           4000
#define ERROR_P2P_SERVER                    4001

#define ERROR_RTC_ICE                       4100
#define ERROR_RTC_STUN                      4101
#define ERROR_RTC_TURN                      4102
#define ERROR_RTC_Whip						4103
#define ERROR_RTC_SDP						4200
#define ERROR_RTC_SDP_EXCHANGE              4218
#define ERROR_RTC_PORT                      5000
#define ERROR_RTP_PACKET_CREATE             5001
#define ERROR_OpenSslCreateSSL              5002
#define ERROR_OpenSslBIOReset               5003
#define ERROR_OpenSslBIOWrite               5004
#define ERROR_OpenSslBIONew                 5005
#define ERROR_RTC_RTP                       5006
#define ERROR_RTC_RTCP                      5007
#define ERROR_RTC_DTLS                      5009
#define ERROR_RTC_SOCKET                    5010
#define ERROR_RTC_RTP_MUXER                 5011
#define ERROR_RTC_SDP_DECODE                5012
#define ERROR_RTC_SRTP_INIT                 5013
#define ERROR_RTC_SRTP_PROTECT              5014
#define ERROR_RTC_SRTP_UNPROTECT            5015
#define ERROR_RTC_RTCP_CHECK                5016
#define ERROR_RTC_SOURCE_CHECK              5017
#define ERROR_RTC_API_BODY                  5019
#define ERROR_RTC_SOURCE_BUSY               5020
#define ERROR_RTC_DISABLED                  5021
#define ERROR_RTC_NO_SESSION                5022
#define ERROR_RTC_INVALID_PARAMS            5023
#define ERROR_RTC_DUMMY_BRIDGER             5024
#define ERROR_RTC_STREM_STARTED             5025
#define ERROR_RTC_TRACK_CODEC               5026
#define ERROR_RTC_NO_PLAYER                 5027
#define ERROR_RTC_NO_PUBLISHER              5028
#define ERROR_RTC_DUPLICATED_SSRC           5029
#define ERROR_RTC_NO_TRACK                  5030
#define ERROR_RTC_RTCP_EMPTY_RR             5031
#define ERROR_RTC_NACK_BUFFER               5032
#define ERROR_RTC_PLAY                      5033
#define ERROR_RTC_PUBLISH                   5034
#define ERROR_RTC_PEERCONNECTION            6000
#define ERROR_RTC_CONNECT                   6001
#define ERROR_RTC_CALLBACK                  6002
#endif /* INCLUDE_YANGUTIL_YANGERRORCODE_H_ */
