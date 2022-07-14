//
// Copyright (c) 2019-2022 yanggaofeng
//


#ifndef SRC_YANGANDROID_YANGPLAYERANDROID_H_
#define SRC_YANGANDROID_YANGPLAYERANDROID_H_
#include <yangplayer/YangPlayerHandle.h>
#include <yangstream/YangStreamType.h>
#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangSysMessageI.h>
#include <stdint.h>
#include <string>

class YangPlayerAndroid :public YangSysMessageI{
public:
	YangPlayerAndroid(YangContext* pcontext);
	virtual ~YangPlayerAndroid();
	int32_t startPlay(std::string url);
	int32_t stopPlay();
	uint8_t* getVideoRef(int64_t* timestamp);
	void success();
	void failure(int32_t errcode);

	int32_t getWidth();
	int32_t getHeight();
	bool m_isStartplay;
private:
	 YangPlayerHandle *m_player;
	 YangContext* m_context;

	 int32_t m_localPort;
	 YangFrame m_audioFrame,m_videoFrame;

};

#endif /* SRC_YANGANDROID_YANGPLAYERANDROID_H_ */
