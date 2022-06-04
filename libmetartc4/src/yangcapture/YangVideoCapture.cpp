//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangcapture/YangVideoCapture.h>
#include <yangutil/sys/YangString.h>
YangVideoCapture::YangVideoCapture() {
	cameraIndex=0;
	m_isStart=0;
	m_para=NULL;
}

YangVideoCapture::~YangVideoCapture() {
	m_para=NULL;
}

void YangVideoCapture::run() {
	m_isStart = 1;
	startLoop();
	m_isStart = 0;
}
void YangVideoCapture::stop() {
	stopLoop();

}

void yang_get_camera_indexs(std::vector<int> *pvs,std::string pcamindex){
	vector<string> res=yang_split(pcamindex,',');
	for(size_t i=0;i<res.size();i++){
		pvs->push_back(atoi(res[i].c_str()));
	}
}
