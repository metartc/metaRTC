//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangutil/sys/YangLog.h>
#include <string.h>
#include <yangcapture/linux/YangX11Screen.h>
#ifndef _WIN32
#include <X11/Xutil.h>
#include <X11/extensions/Xfixes.h>

#include <yangutil/sys/YangTime.h>



YangX11Screen::YangX11Screen() {

	m_x = 0;
	m_y = 0;

	m_isDrawmouse=yangfalse;

	m_display = NULL;
	m_image = NULL;
	m_shm_info.shmseg = 0;
	m_shm_info.shmid = -1;
	m_shm_info.shmaddr = (char*) -1;
	m_shm_info.readOnly = false;
	m_shm_server_attached = false;

	if(init()!=Yang_Ok){

		yang_error("init Error");
	}

}

YangX11Screen::~YangX11Screen() {


	clearImage();
	if(m_display != NULL) {
		XCloseDisplay(m_display);
		m_display = NULL;
	}

}

void YangX11Screen::clearImage(){
	if(m_shm_server_attached) {
		XShmDetach(m_display, &m_shm_info);
		m_shm_server_attached = false;
	}
	if(m_shm_info.shmaddr != (char*) -1) {
		shmdt(m_shm_info.shmaddr);
		m_shm_info.shmaddr = (char*) -1;
	}
	if(m_shm_info.shmid != -1) {
		shmctl(m_shm_info.shmid, IPC_RMID, NULL);
		m_shm_info.shmid = -1;
	}
	if(m_image != NULL) {
		XDestroyImage(m_image);
		m_image = NULL;
	}
}


int32_t YangX11Screen::init() {

	m_display = XOpenDisplay(NULL);

	if(m_display == NULL) {
		yang_error("x11_display create fail");
		return 1;
	}
	m_width=DisplayWidth(m_display,0);
	m_height=DisplayHeight(m_display,0);

	m_screen = DefaultScreen(m_display);
	m_root = RootWindow(m_display, m_screen);
	m_visual = DefaultVisual(m_display, m_screen);
	m_depth = DefaultDepth(m_display, m_screen);
	m_use_shm = XShmQueryExtension(m_display);

	if(m_use_shm) {
		yang_info("Using X11 shared memory.");
	} else {
		yang_info("Not using X11 shared memory.");
	}



	return Yang_Ok;

}



int32_t YangX11Screen::initImage() {

	if(m_shm_server_attached && m_image->width == (int) m_width && m_image->height == (int) m_height) {
		return 1; // reuse existing image
	}
	clearImage();
	m_image = XShmCreateImage(m_display, m_visual, m_depth, ZPixmap, NULL, &m_shm_info, m_width, m_height);
	if(m_image == NULL) {
		yang_error("Can't create shared image!");
		return 1;
	}
	m_shm_info.shmid = shmget(IPC_PRIVATE, m_image->bytes_per_line * m_image->height, IPC_CREAT | 0700);
	if(m_shm_info.shmid == -1) {
		yang_error("Can't get shared memory!");
		return 1;
	}
	m_shm_info.shmaddr = (char*) shmat(m_shm_info.shmid, NULL, SHM_RND);
	if(m_shm_info.shmaddr == (char*) -1) {
		yang_error("Can't attach to shared memory!");
		return 1;
	}
	m_image->data = m_shm_info.shmaddr;
	if(!XShmAttach(m_display, &m_shm_info)) {
		yang_error("Can't attach server to shared memory!");
		return 1;
	}
	m_shm_server_attached = true;
	return Yang_Ok;
}




uint8_t* YangX11Screen::captureFrame(){

	// get the image
	if(m_use_shm) {
		initImage();
		if(!XShmGetImage(m_display, m_root, m_image, 0, 0, AllPlanes)) {
			yang_error("Error: Can't get image (using shared memory)!");

		}
	} else {
		if(m_image != NULL) {
			XDestroyImage(m_image);
			m_image = NULL;
		}
		m_image = XGetImage(m_display, m_root, 0, 0, m_width, m_height, AllPlanes, ZPixmap);
		if(m_image == NULL) {
			yang_error(" Can't get image (not using shared memory)!");

		}
	}


	return (uint8_t*)m_image->data;
}

#endif
