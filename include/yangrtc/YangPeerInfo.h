//
// Copyright (c) 2019-2025 yanggaofeng
//

#ifndef INCLUDE_YANGRTC_YANGPEERINFO_H_
#define INCLUDE_YANGRTC_YANGPEERINFO_H_
#include <yangutil/yangavinfo.h>

#ifdef __cplusplus
extern "C"{
#endif

void yang_init_peerInfo(YangPeerInfo* peerInfo);
void yang_avinfo_initPeerInfo(YangPeerInfo* peerInfo,YangAVInfo* avinfo);

#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_YANGRTC_YANGPEERINFO_H_ */
