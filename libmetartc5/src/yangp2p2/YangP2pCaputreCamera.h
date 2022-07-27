//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef SRC_YANGP2P_YANGP2PCAPUTRECAMERA_H_
#define SRC_YANGP2P_YANGP2PCAPUTRECAMERA_H_
#include "YangP2pCapture.h"

#include <yangcapture/YangMultiVideoCapture.h>

class YangP2pCaputreCamera:public YangP2pCapture {
public:
	YangP2pCaputreCamera(YangContext *pcontext);
	virtual ~YangP2pCaputreCamera();
public:
	void startVideoSource();
	void stopVideoSource();

	void startVideoCaptureState();
	void stopVideoCaptureState();

	int32_t initVideo();

	void startVideoCapture();


	int32_t initAudio(YangPreProcess *pp);
	int32_t initAudio2(YangPreProcess *pp);
	void startAudioCapture();
	YangAudioBuffer* getOutAudioBuffer();
	void stopAudioCaptureState();
	void startAudioCaptureState();
	void setAec(YangRtcAec *paec);
	void setInAudioBuffer(vector<YangAudioPlayBuffer*> *pbuf);

	int32_t change(int32_t st);
	void stopAll();
private:
	YangAudioCapture *m_audioCapture;
	YangMultiVideoCapture *m_videoCapture;
	YangRtcAec *m_aec;
};

#endif /* SRC_YANGP2P_YANGP2PCAPUTRECAMERA_H_ */
