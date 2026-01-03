//
// Copyright (c) 2019-2026 yanggaofeng
//

#ifndef SRC_YANGRTC_YANGPUSHVIDEO_H_
#define SRC_YANGRTC_YANGPUSHVIDEO_H_
#include <yangrtc/YangPushData.h>
#include <yangrtc/YangRtcConnection.h>

int32_t yang_push_encodeVideo(YangRtcContext *context, uint32_t videoSSrc,YangPushDataSession *dataBuffer);

#endif /* SRC_YANGRTC_YANGPUSHVIDEO_H_ */
