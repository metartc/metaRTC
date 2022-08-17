//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGSDP_YANGSDP_H_
#define SRC_YANGSDP_YANGSDP_H_
#include <yangsdp/YangRtcSdp.h>
#include <yangrtc/YangRtcSession.h>
#include <yangutil/yangavtype.h>
int32_t yang_sdp_genLocalSdp(YangRtcSession* session,int32_t localport, char**sdp,YangStreamOptType role);
int32_t yang_sdp_genLocalSdp2(YangRtcSession *session, int32_t localport,char *sdp, YangStreamOptType role);
int32_t yang_sdp_parseRemoteSdp(YangRtcSession* session,YangSdp* sdp);
#endif /* SRC_YANGSDP_YANGSDP_H_ */
