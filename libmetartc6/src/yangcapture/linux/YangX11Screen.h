
#include <yangutil/yangtype.h>
#ifndef _WIN32
#include <X11/Xlib.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>


class YangX11Screen  {

public:
	YangX11Screen();
	~YangX11Screen();
	yangbool m_isDrawmouse;
	uint32_t m_x, m_y, m_width, m_height;
	uint8_t* captureFrame();


private:
	int32_t init();


private:
	int32_t initImage();
	void clearImage();

private:

    Display *m_display;
    int m_screen;
    Window m_root;
    Visual *m_visual;
    int m_depth;
    bool m_use_shm;
    XImage *m_image;
    XShmSegmentInfo m_shm_info;
    bool m_shm_server_attached;



};
#endif
