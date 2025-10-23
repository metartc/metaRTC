//
// Copyright (c) 2019-2024 yanggaofeng
//

#ifndef INCLUDE_YANGCODEC_YANGCODEC_H_
#define INCLUDE_YANGCODEC_YANGCODEC_H_
#include <yangutil/yangavinfo.h>
typedef struct{
	void* session;
	void (*onAudioData)(void* session,YangFrame* pframe);
	void (*onVideoData)(void* session,YangFrame* pframe);
}YangCodecCallback;

typedef struct{
	void* session;
	yangbool (*enable)(void* session);
}YangCodecEnable;
//typedef yangbool (*yang_codec_enable)();
#endif /* INCLUDE_YANGCODEC_YANGCODEC_H_ */
