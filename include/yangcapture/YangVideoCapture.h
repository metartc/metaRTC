//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef INCLUDE_YANGCAPTURE_YANGVIDEOCAPTURE_H_
#define INCLUDE_YANGCAPTURE_YANGVIDEOCAPTURE_H_

#include <yangutil/buffer/YangVideoBuffer.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangThread2.h>
#include <yangutil/yangavinfotype.h>
#include <string>
#include <vector>

void yang_get_camera_indexs(std::vector<int> *pvs,std::string pcamindex);
class YangVideoCapture :public YangThread {
public:
	YangVideoCapture();
	virtual ~YangVideoCapture();
public:
     int32_t cameraIndex;
     virtual int32_t init()=0;
     virtual void setVideoCaptureStart()=0;
     virtual void setVideoCaptureStop()=0;
     virtual void setOutVideoBuffer(YangVideoBuffer *pbuf)=0;
     virtual void setPreVideoBuffer(YangVideoBuffer *pbuf)=0;
     virtual int32_t getVideoCaptureState()=0;
     virtual void initstamp()=0;

     int32_t m_isStart;
     void stop();
protected:
    void run();
    virtual void startLoop()=0;
    virtual void stopLoop()=0;
    YangVideoInfo *m_para;
};

#endif /* INCLUDE_YANGCAPTURE_YANGVIDEOCAPTURE_H_ */
