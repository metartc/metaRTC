//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef INCLUDE_YANGUTIL_SYS_YANGYUVGL_H_
#define INCLUDE_YANGUTIL_SYS_YANGYUVGL_H_
#include <yangutil/yangtype.h>
#if Yang_OS_ANDROID
#include <EGL/egl.h>
#include <GLES2/gl2.h>

typedef struct{
	GLint window_width;
	GLint window_height;
    EGLDisplay eglDisplay;
    EGLSurface eglSurface;
    EGLContext eglContext;
    ANativeWindow * nativeWindow;
}YangEglContext;
class YangYuvGl {
public:
	YangYuvGl();
	virtual ~YangYuvGl();
	int createProgram();
	void render(uint8_t *data);
	int eglOpen();
	int eglClose();
	YangEglContext m_context;
	GLint m_width;
	GLint m_height;
private:
	GLint m_program;
	GLint m_position;
	GLint m_textCoord;
	GLuint m_texture_id[3];
	GLuint m_sample[3];
	float matrix_scale[16];
	float vertex_coords[8] = {//世界坐标
			-1.0f, -1.0f,
			 1.0f, -1.0f,
			-1.0f,  1.0f,
			 1.0f,	1.0f
	};

	float fragment_coords[8] = {//纹理坐标
			0.0f, 1.0f,
			1.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 0.0f
	};
private:
	void initDefMatrix();
	void orthoM(float *m, int mOffset, float left, float right, float bottom, float top,
			float near, float far);
	void changeVideoRotation();
	GLuint LoadProgram(const char *vShaderStr, const char *fShaderStr);
	GLuint LoadShader(GLenum type, const char *shaderSrc);

};
#endif
#endif /* INCLUDE_YANGUTIL_SYS_YANGYUVGL_H_ */
