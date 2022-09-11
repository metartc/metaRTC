//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YangStreamType_YangStreamType_H_
#define YangStreamType_YangStreamType_H_

#include <yangutil/yangavtype.h>

#ifdef __cplusplus

class YangMediaConfigCallback {
public:
	YangMediaConfigCallback() {};
	virtual ~YangMediaConfigCallback() {};
	virtual void setMediaConfig(int32_t puid, YangAudioParam *audio,
			YangVideoParam *video)=0;
};

class YangSendRequestCallback {
public:
	YangSendRequestCallback() {};
	virtual ~YangSendRequestCallback() {};
	virtual void sendRequest(int32_t puid, uint32_t ssrc,
			YangRequestType req)=0;
};

#endif

#endif
