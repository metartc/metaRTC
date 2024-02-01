//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef SRC_YANGRTC_YANGSTREAMURL_H_
#define SRC_YANGRTC_YANGSTREAMURL_H_

#include <yangrtc/YangRtcContext.h>
int32_t yang_stream_parseUrl(char* url,YangStreamConfig* stream,YangAVInfo* avinfo,YangRtcDirection opt);
int32_t yang_stream_parseUrl2(char* url,YangStreamConfig* stream,YangAVInfo* avinfo,YangRtcDirection opt);
#endif /* SRC_YANGRTC_YANGSTREAMURL_H_ */
