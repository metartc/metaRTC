/*
 * YangSdp.h
 *
 *  Created on: 2022年1月7日
 *      Author: yang
 */

#ifndef SRC_YANGSDP_YANGSDP_H_
#define SRC_YANGSDP_YANGSDP_H_
#include <yangutil/yangavtype.h>
#include <yangwebrtc/YangRtcSessionH.h>
int32_t yang_sdp_genLocalSdp(YangRtcSession* session,int32_t localport, char**sdp,YangStreamOptType role);

#endif /* SRC_YANGSDP_YANGSDP_H_ */
