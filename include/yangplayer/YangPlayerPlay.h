/*
 * YangPlayBase.h
 *
 *  Created on: 2019年10月11日
 *      Author: yang
 */

#ifndef YANGAPP_YANGPLAYAPP_H_
#define YANGAPP_YANGPLAYAPP_H_
#include <yangdecoder/YangVideoDecoderHandles.h>
#include <yangaudiodev/linux/YangAudioPlayAlsa.h>
#include <yangaudiodev/win/YangWinAudioApiRender.h>

#include <vector>


#include "yangavutil/audio/YangAecBase.h"
#include "yangutil/buffer/YangAudioPlayBuffer.h"
#include "yangutil/buffer/YangVideoBuffer.h"
#include "yangutil/sys/YangIni.h"

using namespace std;
class YangPlayerPlay {
public:
	YangPlayerPlay();
	virtual ~YangPlayerPlay();
    void initAudioPlay(YangContext* paudio);
	void startAudioPlay();
	void setInAudioList(YangAudioPlayBuffer *paudioList);
	void stopAll();
private:
     #ifdef _WIN32
        YangWinAudioApiRender *m_audioPlay;
#else
	YangAudioPlayAlsa *m_audioPlay;
#endif
	int32_t vm_audio_player_start;

};

#endif /* YANGAPP_YANGPLAYAPP_H_ */
