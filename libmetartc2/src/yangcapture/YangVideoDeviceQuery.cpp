#include <yangcapture/YangVideoDeviceQuery.h>
#ifndef _WIN32
#include "linux/videodev2.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#endif
#include "yangutil/sys/YangLog.h"

YangVideoDeviceQuery::YangVideoDeviceQuery() {

}

YangVideoDeviceQuery::~YangVideoDeviceQuery() {

}

struct YangVideoDevice{
	string name;
	int32_t vindex;
};
#ifndef _WIN32
int32_t YangVideoDeviceQuery::getVideoDeviceList() {
	vector<YangVideoDevice>* pv;
	struct v4l2_capability cap;
	struct v4l2_fmtdesc fmt;
	memset(&fmt, 0, sizeof(struct v4l2_fmtdesc));
	fmt.index = 0;
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	int32_t vet = 0;
	int32_t m_vd_id=0;
	//v4l2_device dd;
	if (ioctl(m_vd_id, VIDIOC_QUERYCAP, &cap) != 0) {
		yang_error("\n VIDIOC_QUERYCAP error!");
		return ERROR_SYS_Linux_NoVideoDriver;
	}
	yang_trace("\ndriver name %s card = %s cap = %0x\n", cap.driver, cap.card,
			cap.capabilities);

	while ((vet = ioctl(m_vd_id, VIDIOC_ENUM_FMT, &fmt)) != -1) {
		pv->push_back(YangVideoDevice());
		pv->back().name=string((char*)fmt.description);
		pv->back().vindex=fmt.index;
		fmt.index++;

		//if(fmt.==m_para->width)
		yang_trace("\n{ pixelformat = ''%c%c%c%c'', description = ''%s'' }\n",
				fmt.pixelformat & 0xFF, (fmt.pixelformat >> 8) & 0xFF,
				(fmt.pixelformat >> 16) & 0xFF, (fmt.pixelformat >> 24) & 0xFF,
				fmt.description);
		struct v4l2_frmsizeenum frmsize;
		frmsize.pixel_format = fmt.pixelformat;
		frmsize.index = 0;
		while (!ioctl(m_vd_id, VIDIOC_ENUM_FRAMESIZES, &frmsize)) {
			if (frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE) {
				//if (m_para->width == (int) frmsize.discrete.width&& m_para->height == (int) frmsize.discrete.height) {
					yang_trace("DISCRETE: line-%d %d: {%d*%d}\n", __LINE__,
							frmsize.index, frmsize.discrete.width,
							frmsize.discrete.height);
				//	setReselution(fmt.pixelformat, 2);
				//}

			}

			else if (frmsize.type == V4L2_FRMSIZE_TYPE_STEPWISE) {
				yang_trace("\nSTEPWISE: line: %d %d: {%d*%d}\n", __LINE__,
						frmsize.index, frmsize.stepwise.max_width,
						frmsize.stepwise.max_height);
			}

			enum_camera_frmival(m_vd_id, &frmsize);
			frmsize.index++;
		}
	}
	return 1;
}

int32_t YangVideoDeviceQuery::enum_camera_frmival(int32_t fd, struct v4l2_frmsizeenum *framesize) {
	struct v4l2_frmivalenum frmival;
	memset(&frmival, 0, sizeof(frmival));
	frmival.pixel_format = framesize->pixel_format;
	frmival.width = framesize->discrete.width;
	frmival.height = framesize->discrete.height;
	//frmival.type = V4L2_FRMIVAL_TYPE_DISCRETE;
	frmival.index = 0;
	//yang_trace("the frame intervals enum");

	while (ioctl(fd, VIDIOC_ENUM_FRAMEINTERVALS, &frmival) == 0) {
		//输出分数，即帧间隔
		//if(frmival.width==m_para->width) {
			yang_trace("%d.frameinterval:%u/%u\n ", frmival.index,
				frmival.discrete.numerator, frmival.discrete.denominator);
		//}
		frmival.index++;
	}
	//yang_trace(".........................................\n");
	return 0;
}

#endif
