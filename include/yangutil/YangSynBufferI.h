
#ifndef INCLUDE_YANGUTIL_YANGAVSYNI_H_
#define INCLUDE_YANGUTIL_YANGAVSYNI_H_

class YangSynBufferI{
public:
        YangSynBufferI(){};
        virtual ~YangSynBufferI(){};
	virtual int32_t playAudioFrame(int64_t pts)=0;
	virtual int32_t playVideoFrame(int64_t pts)=0;
};



#endif /* INCLUDE_YANGUTIL_YANGAVSYNI_H_ */
