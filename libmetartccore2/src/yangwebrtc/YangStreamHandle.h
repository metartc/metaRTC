/*
 * YangStreamHandle.h
 *
 *  Created on: 2022年1月16日
 *      Author: yang
 */

#ifndef SRC_YANGWEBRTC_YANGSTREAMHANDLE_H_
#define SRC_YANGWEBRTC_YANGSTREAMHANDLE_H_

#include <yangwebrtc/YangRtcContext.h>
int32_t yang_stream_parseUrl(char* url,YangStreamConfig* stream,YangAVContext* context,YangStreamOptType opt);
#endif /* SRC_YANGWEBRTC_YANGSTREAMHANDLE_H_ */
