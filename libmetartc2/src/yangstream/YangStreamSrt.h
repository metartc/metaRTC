
#ifndef YANGSTREAM_SRC_YANGSTREAMSRT_H_
#define YANGSTREAM_SRC_YANGSTREAMSRT_H_
#include <yangstream/YangStreamHandle.h>
#include "yangsrt/YangSrtBase.h"
#include "yangsrt/YangTsMuxer.h"
#include "yangsrt/YangTsdemux.h"
class YangStreamSrt :public YangStreamHandle,public ts_media_data_callback_I {
public:
	YangStreamSrt(int32_t puid,YangContext* pcontext);
	virtual ~YangStreamSrt();
	int32_t disConnectMediaServer();
	int32_t connectMediaServer();
	int32_t receiveData(int32_t *plen);
	void on_data_callback(SRT_DATA_MSG_PTR data_ptr,
							uint32_t  media_type, uint64_t dts, uint64_t pts);


public:
	int32_t isconnected();
	int32_t getConnectState();
	int32_t sendPmt();
	int32_t reconnect();
	int32_t publishVideoData(YangStreamCapture* videoFrame);
	int32_t publishAudioData(YangStreamCapture* audioFrame);
private:
	YangTsdemux demux;
	YangSrtBase *m_srt;
	YangTsMuxer m_ts;
	char *m_buffer;
	int32_t m_bufLen;
	int32_t m_bufReceiveLen,m_bufRemainLen;
	YangFrame m_audioFrame;
	YangFrame m_videoFrame;
};

#endif /* YANGSTREAM_SRC_YANGSTREAMSRT_H_ */
