/*
 * YangPlayerHandle.h
 *
 *  Created on: 2021年5月26日
 *      Author: yang
 */

#ifndef INCLUDE_YANGPLAYER_YANGPLAYERHANDLE_H_
#define INCLUDE_YANGPLAYER_YANGPLAYERHANDLE_H_
#include <string>
#include <yangutil/buffer/YangVideoBuffer.h>
#include <yangutil/sys/YangSysMessageI.h>
#include <yangutil/yangavinfotype.h>
class YangPlayerHandle {
public:
	YangPlayerHandle(){};
	virtual ~YangPlayerHandle(){};
	virtual YangVideoBuffer* getVideoBuffer()=0;
	virtual int play(std::string url,int32_t localport)=0;
	virtual int32_t playRtc(int32_t puid,std::string localIp,int32_t localPort, std::string server, int32_t pport,std::string app,std::string stream)=0;
    virtual void stopPlay()=0;
    static YangPlayerHandle* createPlayerHandle(YangContext* pcontext,YangSysMessageI* pmessage);
};

#endif /* INCLUDE_YANGPLAYER_YANGPLAYERHANDLE_H_ */
