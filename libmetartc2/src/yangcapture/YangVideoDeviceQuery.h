#ifndef YANGCAPTURE_SRC_YANGVIDEODEVICEQUERY_H_
#define YANGCAPTURE_SRC_YANGVIDEODEVICEQUERY_H_
#include <string>
#include <string.h>
#include <vector>
using namespace std;
class YangVideoDeviceQuery {
public:
	YangVideoDeviceQuery();
	virtual ~YangVideoDeviceQuery();
	int32_t getVideoDeviceList();
	int32_t enum_camera_frmival(int32_t fd, struct v4l2_frmsizeenum *framesize);
};

#endif /* YANGCAPTURE_SRC_YANGVIDEODEVICEQUERY_H_ */
