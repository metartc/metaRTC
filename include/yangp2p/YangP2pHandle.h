//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef INCLUDE_YANGPUSH_YangP2pHandle_H_
#define INCLUDE_YANGPUSH_YangP2pHandle_H_
#include <yangutil/buffer/YangVideoBuffer.h>
#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangSysMessageI.h>
#include <yangpush/YangPushCommon.h>


class YangP2pHandle {
public:
    YangP2pHandle(){};
    virtual ~YangP2pHandle(){};
    virtual int32_t startRtc(char* remoteIp,char* sdp,char* response)=0;
    virtual int32_t connectRtc(char* url)=0;
    virtual void disconnect()=0;
    virtual void init()=0;
    virtual YangVideoBuffer* getPreVideoBuffer()=0;
    virtual std::vector<YangVideoBuffer*>* getPlayVideoBuffer()=0;
};



#endif /* INCLUDE_YANGPUSH_YANGPUSHHANDLE_H_ */
