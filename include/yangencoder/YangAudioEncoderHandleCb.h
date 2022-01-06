

#ifndef YANGENCODER_INCLUDE_YANGAUDIOENCODERHANDLECB_H_
#define YANGENCODER_INCLUDE_YANGAUDIOENCODERHANDLECB_H_

#include "stdint.h"

#include <vector>

#include "yangutil/buffer/YangAudioEncoderBuffer.h"
#include "yangutil/buffer/YangAudioPlayBuffer.h"
#include "yangutil/sys/YangIni.h"
#include "yangutil/sys/YangThread.h"
//#include "yangutil/YangVideoContext.h"
#include "yangencoder/YangAudioEncoder.h"
using namespace std;
class YangAudioEncoderHandleCb:public YangThread
{
public:
	YangAudioEncoderHandleCb(YangAudioInfo *pcontext);
	~YangAudioEncoderHandleCb(void);

private:
	int32_t m_isInit;

public:
	void init();
	void stop();
	void setInAudioBuffer(YangAudioBuffer *pbuf);
	//void setOutAudioBuffer(YangAudioEncoderBuffer *pbuf);
	void setCallback(YangEncoderCallback* pcb);
	//void setRoomState(int32_t pst);
	//void onVideoData(uint8_t* p,int32_t plen,int64_t timestamp,int32_t pframetype,int32_t puid);
	//void onAudioData(uint8_t* p,int32_t plen,int32_t puid);
	int32_t m_isStart;
	int32_t m_uid;



protected:
	void run();
	void stopLoop();
	void startLoop();

void saveFile(char *fileName, uint8_t *pBuffer, long BufferLen);

private:
	YangEncoderCallback* m_cb;
	YangAudioEncoder *m_enc;
	int32_t m_isConvert;
	//int32_t m_roomState;
	YangAudioBuffer *m_in_audioBuffer;
	//YangAudioEncoderBuffer *m_out_audioBuffer;
	YangAudioInfo *m_context;
	void Encoder(int32_t isIframe,uint8_t*src,int32_t  p_buflen,uint8_t *dest,int32_t *p_destLen);

};
#endif /* YANGENCODER_INCLUDE_YANGAUDIOENCODERHANDLECB_H_ */
