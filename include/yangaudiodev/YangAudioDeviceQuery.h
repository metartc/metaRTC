//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef INCLUDE_YANGAUDIODEV_YANGAUDIODEVICEQUERY_H_
#define INCLUDE_YANGAUDIODEV_YANGAUDIODEVICEQUERY_H_
#include <vector>
#include <string>

using namespace std;
struct YangAudioDeivce{
	string name;
	string deviceName;
	string subName;
	int32_t aIndex;
	int32_t aSubIndex;
	int32_t aIdx;
};
class YangAudioDeviceQuery {
public:
	YangAudioDeviceQuery();
	virtual ~YangAudioDeviceQuery();
  	vector<YangAudioDeivce>* getCaptureDeviceList();
    vector<YangAudioDeivce>* getPlayDeviceList();
private:
    vector<YangAudioDeivce> m_captureDeviceList;
    vector<YangAudioDeivce> m_playDeviceList;
    void getDeviceList(int32_t stream,vector<YangAudioDeivce>* plist);

};

#endif /* INCLUDE_YANGAUDIODEV_YANGAUDIODEVICEQUERY_H_ */
