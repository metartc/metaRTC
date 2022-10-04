//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGDECODER_INCLUDE_YANGDECODER_H_
#define YANGDECODER_INCLUDE_YANGDECODER_H_

class YangDecoderCallback{
public:
	YangDecoderCallback(){};
	virtual ~YangDecoderCallback(){};
	virtual void onAudioData(YangFrame* pframe)=0;
	virtual void onVideoData(YangFrame* pframe)=0;
};



#endif /* YANGDECODER_INCLUDE_YANGDECODER_H_ */
