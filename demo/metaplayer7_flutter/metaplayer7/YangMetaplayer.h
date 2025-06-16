//
// Copyright (c) 2019-2023 yanggaofeng
//

#ifndef SRC_YANGMETAPLAYER_H_
#define SRC_YANGMETAPLAYER_H_

#include "yangplayer/YangPlayerHandle.h"
#include "yangstream/YangStreamType.h"

#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangSysMessageI.h>
class YangMetaplayer:public YangSysMessageI {
public:
	YangMetaplayer();
	virtual ~YangMetaplayer();
	YangContext* m_context;
	uint8_t* m_buffer;
	uint8_t* m_rgbaBuffer;
	YangPlayerHandle *m_player;
	YangStreamConfig m_conf;
	YangFrame m_frame;
	int32_t m_srcWidth,m_dstWidth;
	int32_t m_srcHeight,m_dstHeight;
	
	void success();
	void failure(int32_t errcode);
	void playRtc(char* url);
	void close();
	void setSize(int32_t width,int32_t height);
protected:
	
private:
	bool m_isStartplay;
	


};

#endif /* SRC_YANGMETAPLAYER_H_ */
