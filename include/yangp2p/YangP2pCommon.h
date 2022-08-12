//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef INCLUDE_YANGP2p_YANGP2pCOMMON_H_
#define INCLUDE_YANGP2p_YANGP2pCOMMON_H_
#include <yangutil/yangavtype.h>
#include <yangutil/buffer/YangAudioEncoderBuffer.h>
#include <yangutil/buffer/YangVideoEncoderBuffer.h>

enum YangP2pMessageType {
	YangM_P2p_StartAudioCapture,
	YangM_P2p_StartVideoCapture,
	YangM_P2p_StartScreenCapture,
    YangM_P2p_StartOutCapture,
	YangM_P2p_Connect,
	YangM_P2p_Disconnect,
	YangM_P2p_Play_Start,
	YangM_P2p_Play_Stop,
	YangM_P2p_Sendkeyframe

};

class YangP2pRtcBuffer{
public:
	YangP2pRtcBuffer(){};
	virtual ~YangP2pRtcBuffer(){};
	virtual void setInVideoMetaData(YangVideoMeta *pvmd)=0;
	virtual void setInAudioList(YangAudioEncoderBuffer *pbuf)=0;
	virtual void setInVideoList(YangVideoEncoderBuffer *pbuf)=0;
};


#endif /* INCLUDE_YANGPUSH_YANGPUSHCOMMON_H_ */
