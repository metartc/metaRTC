//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef INCLUDE_YANGUTIL_SYS_YANGYUVGL_H_
#define INCLUDE_YANGUTIL_SYS_YANGYUVGL_H_
#ifdef __ANDROID__
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


	  GLint gl_program;
	    GLint gl_position;
	    GLint gl_textCoord;
	    GLuint gl_texture_id[3];

	    uint8_t *y_data;
	    uint8_t *u_data;
	    uint8_t *v_data;

	    float matrix_scale[16];

	    /**  float vertex_coords[12] = {//世界坐标
	            -1, -1, 0, // left bottom
	            1, -1, 0, // right bottom
	            -1, 1, 0,  // left top
	            1, 1, 0,   // right top
	    };**/

	     float vertex_coords[12] = {//世界坐标
	    		1.0f, -1.0f, 0.0f,
	    		-1.0f, -1.0f, 0.0f,
	    		1.0f, 1.0f, 0.0f,
	    		-1.0f, 1.0f, 0.0f
		    };

	     /**   float fragment_coords[8] = {//纹理坐标
	              0, 1,//left bottom
	              1, 1,//right bottom
	              0, 0,//left top
	              1, 0,//right top
	      };**/
	   float fragment_coords[8] = {//纹理坐标
	    		  1.0f, 0.0f,
	    			               0.0f, 0.0f,
	    			               1.0f, 1.0f,
	    			               0.0f, 1.0f
		      };

private:
		//void initDefMatrix();
	   // void orthoM(float *m, int mOffset, float left, float right, float bottom, float top,
	   //                        float near, float far);
	    //void changeVideoRotation();
	    GLuint LoadProgram(const char *vShaderStr, const char *fShaderStr);
	   	GLuint LoadShader(GLenum type, const char *shaderSrc);

};
#endif
#endif /* INCLUDE_YANGUTIL_SYS_YANGYUVGL_H_ */
