//
// Copyright (c) 2019-2023 yanggaofeng
//

#ifndef SRC_YANGMETAPLAYER_H_
#define SRC_YANGMETAPLAYER_H_
#include "yangrecordthread.h"
#include "yangplayer/YangPlayerHandle.h"
#include "yangstream/YangStreamType.h"

#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangSysMessageI.h>
class YangMetaplayer:public YangSysMessageI {
public:
	YangMetaplayer();
	virtual ~YangMetaplayer();

	YangRecordThread *m_videoThread;
	YangPlayerHandle *m_player;
	YangStreamConfig m_conf;
	void initVideoThread(YangRecordThread *prt);

	void success();
	void failure(int32_t errcode);
	void on_m_b_play_clicked();
protected:
	YangContext* m_context;
private:
	bool m_isStartplay;

};

#endif /* SRC_YANGMETAPLAYER_H_ */
