//
// Copyright (c) 2019-2026 yanggaofeng
//

#ifndef SRC_YANGRTC_YANGPUSHAUDIO_H_
#define SRC_YANGRTC_YANGPUSHAUDIO_H_
#include <yangrtc/YangPushData.h>
#include <yangrtc/YangRtcSession.h>

int32_t yang_push_encodeAudio(YangRtcContext* context, uint32_t audioSsrc,YangPushDataSession *dataSession,uint8_t* tmp);


#endif /* SRC_YANGRTC_YANGPUSHAUDIO_H_ */
