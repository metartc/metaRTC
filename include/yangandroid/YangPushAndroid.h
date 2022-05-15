//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef SRC_YANGPUSHANDROID_H_
#define SRC_YANGPUSHANDROID_H_
#include <yangstream/YangStreamType.h>
#include <yangutil/yangavinfotype.h>
#include <yangpush/YangPushHandle.h>
class YangPushAndroid:public YangSysMessageI {
public:
	YangPushAndroid(YangContext* pcontext,void* pwindow);
	virtual ~YangPushAndroid();
	int32_t startPush(std::string url);
	int32_t stopPush();
	void startCamera();
	YangContext* m_context;
	void success();
	void failure(int32_t errcode);
private:
	YangPushHandle* m_push;

	 bool m_isStartplay;
	 int32_t m_localPort;
	 YangFrame m_audioFrame,m_videoFrame;

};

#endif /* SRC_YANGPUSHANDROID_H_ */
