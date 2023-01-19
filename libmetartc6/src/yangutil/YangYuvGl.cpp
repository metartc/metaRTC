//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangutil/sys/YangYuvGl.h>
#include <yangutil/sys/YangLog.h>

#ifdef __ANDROID__
YangYuvGl::YangYuvGl() {
	m_program = -1;

	m_texture_id[0] = 0;
	m_texture_id[1] = 0;
	m_texture_id[2] = 0;

	m_textCoord=0;
	m_position=0;
	m_textCoord=0;
	m_width=640;
	m_height=480;
	m_context.window_width=0;
	m_context.window_height=0;
}

YangYuvGl::~YangYuvGl() {
	glDeleteTextures(3, m_texture_id);
	glDeleteProgram(m_program);
}
int32_t YangYuvGl::createProgram(){
	const char *vsrc = R"(precision highp float;
		attribute highp vec4 vertexIn; 
	    attribute highp vec2 textureIn; 
	    varying highp vec2 textureOut;  
	    void main(void)           
	    {     
	        textureOut = textureIn; 
			gl_Position = vertexIn; 
	    })";

	const char *fsrc = R"(precision highp float;
	    varying   highp vec2 textureOut; 
	    uniform lowp sampler2D tex_y; 
	    uniform lowp sampler2D tex_u; 
	    uniform lowp sampler2D tex_v; 
	    void main(void) 
	    { 
			mediump vec3 yuv;
      		lowp    vec3 rgb;
			yuv.x = (texture2D(tex_y, textureOut).r - (16.0 / 255.0));
			yuv.y = (texture2D(tex_u, textureOut).r - 0.5);
			yuv.z = (texture2D(tex_v, textureOut).r - 0.5);
	   
			rgb = mat3( 1.164,  1.164,  1.164,
    					0.0,   -0.213,  2.112,
    					1.793, -0.533, 0.0 ) * yuv;
	        gl_FragColor = vec4(rgb, 1.0); 
	    })";


	m_program = LoadProgram(vsrc, fsrc);

	yang_trace("CreateProgram : video_width: %d, video_height： %d, window_width：%d, window_height：%d",
			m_width, m_height, m_context.window_width, m_context.window_height);
	return Yang_Ok;
}

GLuint YangYuvGl::LoadShader(GLenum type, const char *shaderSrc) {
	yang_trace("LoadShader type : %d", type);
	GLuint shader;
	shader = glCreateShader(type);
	if (shader == 0) {
		return 0;
	}
	glShaderSource(shader, 1, &shaderSrc, NULL);
	glCompileShader(shader);
	GLint status = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		GLint length = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		GLchar log[length + 1];
		glGetShaderInfoLog(shader, length, &length, log);
		yang_trace("glCompileShader fail: %s", log);
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}

GLuint YangYuvGl::LoadProgram(const char *vShaderStr, const char *fShaderStr) {

	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint mProgram;

	//eglMakeCurrent()函数来将当前的上下文切换，这样opengl的函数才能启动作用
	if(EGL_TRUE != eglMakeCurrent(m_context.eglDisplay,
			m_context.eglSurface, m_context.eglSurface,
			m_context.eglContext)) {
		yang_error("eglMakeCurrent failed");
		return -1;
	}

	// Load the vertex/fragment shaders
	vertexShader = LoadShader(GL_VERTEX_SHADER, vShaderStr);
	fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fShaderStr);

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	//  glEnable(GL_CULL_FACE);
	// glCullFace(GL_BACK);
	// glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, m_context.window_width,m_context.window_height);
	// Create the program object
	mProgram = glCreateProgram();
	yang_trace("glCreateProgram mProgram : %d", mProgram);

	m_program = mProgram;

	// Attaches a shader object to a program object
	glAttachShader(mProgram, vertexShader);
	glAttachShader(mProgram, fragmentShader);

	// Link the program object
	glLinkProgram(mProgram);
	GLint status = 0;
	glGetProgramiv(mProgram, GL_LINK_STATUS, &status);

	if (status == EGL_FALSE) {
		GLint length = 0;
		glGetProgramiv(mProgram, GL_INFO_LOG_LENGTH, &length);
		GLchar log[length + 1];
		glGetProgramInfoLog(mProgram, length, &length, log);
		yang_error("glLinkProgram failed : %s", log);
		glDeleteProgram (mProgram);
		return -1;
	}

	yang_trace("glLinkProgram success");

	glDeleteShader (vertexShader);
	glDeleteShader (fragmentShader);
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glUseProgram(mProgram);

	// 获取顶点着色器的位置的句柄
	m_position = glGetAttribLocation(mProgram, "vertexIn");
	glVertexAttribPointer(m_position, 2, GL_FLOAT, GL_FALSE, 0, vertex_coords);
	glEnableVertexAttribArray(m_position);

	m_textCoord = glGetAttribLocation(mProgram, "textureIn");
	glVertexAttribPointer(m_textCoord, 2, GL_FLOAT, GL_FALSE, 0, fragment_coords);
	glEnableVertexAttribArray(m_textCoord);

	yang_trace("LoadProgram out gl_position : %d gl_textCoord: %d", m_position, m_textCoord);
	//创建若干个纹理对象，并且得到纹理ID
	glGenTextures(3, m_texture_id);
	m_sample[0]=glGetUniformLocation(mProgram, "tex_y");
	m_sample[1]=glGetUniformLocation(mProgram, "tex_u");
	m_sample[2]=glGetUniformLocation(mProgram, "tex_v");
	//将纹理目标和纹理绑定后，对纹理目标所进行的操作都反映到对纹理上
	for(int i=0;i<3;i++){
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_texture_id[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glUniform1i(m_sample[i], i);
	}
	return mProgram;
}


void YangYuvGl::initDefMatrix() {
	float originRatio = (float) m_width / m_height;
	float worldRatio = (float) m_context.window_width / m_context.window_height;
	if (worldRatio > 1) {
		if (originRatio > worldRatio) {
			float actualRatio = originRatio / worldRatio;
			orthoM(
					matrix_scale, 0,
					-1, 1,
					-actualRatio, actualRatio,
					-1, 3
			);
		} else {// 原始比例小于窗口比例，缩放高度度会导致高度超出，因此，高度以窗口为准，缩放宽度
			float actualRatio = worldRatio / originRatio;
			orthoM(
					matrix_scale, 0,
					-actualRatio, actualRatio,
					-1, 1,
					-1, 3
			);
		}
	} else {
		if (originRatio > worldRatio) {
			float actualRatio = originRatio / worldRatio;
			orthoM(
					matrix_scale, 0,
					-1, 1,
					-actualRatio, actualRatio,
					-1, 3
			);
		} else {// 原始比例小于窗口比例，缩放高度会导致高度超出，因此，高度以窗口为准，缩放宽度
			float actualRatio = worldRatio / originRatio;
			orthoM(
					matrix_scale, 0,
					-actualRatio, actualRatio,
					-1, 1,
					-1, 3
			);
		}
	}
}


void YangYuvGl::orthoM(float *m, int mOffset, float left, float right, float bottom, float top,
		float near, float far) {
	float r_width  = 1 / (right - left);
	float r_height = 1 / (top - bottom);
	float r_depth  = 1 / (far - near);
	float x =  2 * (r_width);
	float y =  2 * (r_height);
	float z = -2 * (r_depth);
	float tx = -(right + left) * r_width;
	float ty = -(top + bottom) * r_height;
	float tz = -(far + near) * r_depth;
	m[mOffset + 0] = x;
	m[mOffset + 5] = y;
	m[mOffset + 10] = z;
	m[mOffset + 12] = tx;
	m[mOffset + 13] = ty;
	m[mOffset + 14] = tz;
	m[mOffset + 15] = 1;
	m[mOffset + 1] = 0;
	m[mOffset + 2] = 0;
	m[mOffset + 3] = 0;
	m[mOffset + 4] = 0;
	m[mOffset + 6] = 0;
	m[mOffset + 7] = 0;
	m[mOffset + 8] = 0;
	m[mOffset + 9] = 0;
	m[mOffset + 11] = 0;
}

//由libyuv处理旋转
void YangYuvGl::changeVideoRotation() {
	glVertexAttribPointer(m_textCoord, 2, GL_FLOAT, GL_FALSE, 0, fragment_coords);
	yang_trace("changeVideoRotation in");
}


void YangYuvGl::render(uint8_t *data) {
	//激活第一层纹理，绑定到创建的纹理
	int yuvlen=m_width*m_height;

	glBindTexture(GL_TEXTURE_2D, m_texture_id[0]);

	glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_LUMINANCE,
			m_width,
			m_height,
			0,
			GL_LUMINANCE,
			GL_UNSIGNED_BYTE,
			data);

	glBindTexture(GL_TEXTURE_2D, m_texture_id[1]);
	glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_LUMINANCE,
			m_width >> 1,
			m_height >> 1,
			0,
			GL_LUMINANCE,
			GL_UNSIGNED_BYTE,
			data+yuvlen
	);

	glBindTexture(GL_TEXTURE_2D, m_texture_id[2]);
	glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_LUMINANCE,
			m_width >> 1,
			m_height >> 1,
			0,
			GL_LUMINANCE,
			GL_UNSIGNED_BYTE,
			data+(yuvlen*5/4)
	);

	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	//窗口显示，交换双缓冲区
	eglSwapBuffers(m_context.eglDisplay, m_context.eglSurface);
}


int YangYuvGl::eglOpen() {

	m_context.eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (m_context.eglDisplay == EGL_NO_DISPLAY ) {
		yang_trace("eglGetDisplay failure : %d", eglGetError());
		return -1;
	}
	yang_trace("eglGetDisplay ok");

	EGLint majorVersion;//主版本号
	EGLint minorVersion;//次版本号
	EGLBoolean success = eglInitialize(m_context.eglDisplay, &majorVersion, &minorVersion);

	if (!success) {
		yang_trace("eglInitialize failure: %d", eglGetError());
		return -1;
	}
	yang_trace("eglInitialize ok");

	EGLint numConfigs;
	EGLConfig config;
	const EGLint CONFIG_ATTRIBS[] = {
			EGL_RENDERABLE_TYPE,    EGL_OPENGL_ES2_BIT,
			EGL_SURFACE_TYPE,       EGL_WINDOW_BIT,
			EGL_BLUE_SIZE,          8,
			EGL_GREEN_SIZE,         8,
			EGL_RED_SIZE,           8,
			EGL_NONE
	};

	success = eglChooseConfig(m_context.eglDisplay, CONFIG_ATTRIBS, &config, 1, &numConfigs);
	if (!success) {
		yang_trace("eglChooseConfig failure: %d", eglGetError());
		return -1;
	}
	yang_trace("eglChooseConfig ok");

	EGLSurface eglSurface = eglCreateWindowSurface(m_context.eglDisplay, config,
			m_context.nativeWindow, 0);
	if (EGL_NO_SURFACE == eglSurface) {
		yang_trace("eglCreateWindowSurface failure: %d", eglGetError());
		return -1;
	}

	yang_trace("eglCreateWindowSurface ok");
	m_context.eglSurface = eglSurface;

	const EGLint attribs[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
	EGLContext elgContext = eglCreateContext(m_context.eglDisplay, config, EGL_NO_CONTEXT,
			attribs);
	if (elgContext == EGL_NO_CONTEXT ) {
		yang_trace("eglCreateContext failure, error is %d", eglGetError());
		return -1;
	}
	yang_trace("eglCreateContext ok");
	m_context.eglContext = elgContext;
	return 0;
}

int YangYuvGl::eglClose() {
	EGLBoolean success = eglDestroySurface(m_context.eglDisplay, this->m_context.eglSurface);
	if (!success) {
		yang_trace("eglDestroySurface failure.");
	}

	success = eglDestroyContext(m_context.eglDisplay,m_context.eglContext);
	if (!success) {
		yang_trace("eglDestroyContext failure.");
	}

	success = eglTerminate(m_context.eglDisplay);
	if (!success) {
		yang_trace("eglTerminate failure.");
	}

	m_context.eglSurface = NULL;
	m_context.eglContext = NULL;
	m_context.eglDisplay = NULL;
	return 0;
}
#endif
