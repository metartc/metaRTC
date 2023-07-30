//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef INCLUDE_YANGPUSH_YANGPUSHHANDLE_H_
#define INCLUDE_YANGPUSH_YANGPUSHHANDLE_H_
#include <yangutil/buffer/YangVideoBuffer.h>
#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangSysMessageI.h>
#include <yangpush/YangPushCommon.h>
#include <yangpush/YangSendVideoI.h>
#include <string>
class YangPushHandle {
public:
    YangPushHandle();
    virtual ~YangPushHandle();
    virtual int publish(char* url,yangbool isWhip)=0;
    virtual void disconnect()=0;

    virtual void init()=0;
    virtual void changeSrc(int videoSrcType,bool pisinit)=0;
    virtual YangVideoBuffer* getPreVideoBuffer()=0;

    virtual YangSendVideoI* getSendVideo()=0;

};



#endif /* INCLUDE_YANGPUSH_YANGPUSHHANDLE_H_ */
