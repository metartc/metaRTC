//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangYuvGl.h>
#include <string>
#ifdef __ANDROID__
YangYuvGl::YangYuvGl() {
    gl_program = -1;
   // gl_uMatrix = -1;
    gl_texture_id[0] = 0;
    gl_texture_id[1] = 0;
    gl_texture_id[2] = 0;

    y_data = NULL;
    u_data = NULL;
    v_data = NULL;

    m_width=640;
    m_height=480;
    m_context.window_width=0;
    m_context.window_height=0;
}

YangYuvGl::~YangYuvGl() {


    glDeleteTextures(3, gl_texture_id);
    glDeleteProgram(gl_program);
}
int32_t YangYuvGl::createProgram(){
/*
 *
 * attribute vec4 vertexIn;
    attribute vec2 textureIn; 
    varying vec2 textureOut;  
    void main(void)           
    {                         
        gl_Position = vertexIn; 
        textureOut = textureIn; 
    }
     attribute       vec4 aPosition;//输入的顶点坐标，会在程序指定将数据输入到该字段
        attribute
        vec2 aTextCoord;//输入的纹理坐标，会在程序指定将数据输入到该字段
        varying
        vec2 vTextCoord;//输出的纹理坐标
        void main() {
            //这里其实是将上下翻转过来（因为安卓图片会自动上下翻转，所以转回来）
            vTextCoord = vec2(aTextCoord.x, 1.0 - aTextCoord.y);
            //直接把传入的坐标值作为传入渲染管线。gl_Position是OpenGL内置的
            gl_Position = aPosition;
        }
 * */
    const char *vsrc = R"(attribute vec4 vertexIn; 
    attribute vec2 textureIn; 
    varying vec2 textureOut;  
    void main(void)           
    {     
        textureOut = vec2(textureIn.x,1.0-textureIn.y); 
		gl_Position = vertexIn; 
    })";

    const char *fsrc = R"(precision mediump float;
    varying vec2 textureOut; 
    uniform sampler2D tex_y; 
    uniform sampler2D tex_u; 
    uniform sampler2D tex_v; 
    void main(void) 
    { 
        vec3 yuv; 
        vec3 rgb; 
        yuv.x = texture2D(tex_y, textureOut).r; 
        yuv.y = texture2D(tex_u, textureOut).r - 0.5; 
        yuv.z = texture2D(tex_v, textureOut).r - 0.5; 
        rgb = mat3(    1.0, 1.0, 1.0,
                    0.0, -0.39465, 2.03211,
                    1.13983, -0.5806, 0.0) * yuv; 
        gl_FragColor = vec4(rgb, 1.0); 
    })";

    /*

                     1,       1,         1,
                    0,       -0.34414,  1.772,
                    1.402, -0.71414,  0
     * */
	/**
	 const char *vsrc = "attribute vec4 vertexIn; \
	attribute vec2 textureIn; \
	varying vec2 textureOut; \
	void main() { \
	    gl_Position = vertexIn; \
	    textureOut = textureIn; \
	}";

	 const char *fsrc = "precision mediump float;\
varying vec2 textureOut; \
uniform sampler2D tex_y; \
uniform sampler2D tex_u; \
uniform sampler2D tex_v; \
void main() { \
    vec3 yuv; \
      vec3 rgb; \
      yuv.r = texture2D(tex_y, textureOut).r - 0.0625; \
      yuv.g = texture2D(tex_u, textureOut).r - 0.5; \
      yuv.b = texture2D(tex_v, textureOut).r - 0.5; \
      rgb = mat3( \
      1.164, 1.164, 1.164,  \
      0.0, -0.392, 2.016,  \
      1.596, -0.812, 0.0   \
      ) * yuv; \
      gl_FragColor= vec4(rgb, 1); \
}";
**/
    gl_program = LoadProgram(vsrc, fsrc);

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
//#define ATTRIB_VERTEX 3
//#define ATTRIB_TEXTURE 4
GLuint YangYuvGl::LoadProgram(const char *vShaderStr, const char *fShaderStr) {
   // initDefMatrix();
  //  yang_trace("LoadProgram in width : %d  height: %d", context->m_width, context->m_height);
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
    glViewport(0, 0, m_context.window_width, m_context.window_height);
    // Create the program object
    mProgram = glCreateProgram();
    yang_trace("glCreateProgram mProgram : %d", mProgram);

    gl_program = mProgram;

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

    glUseProgram(mProgram);

    // 获取顶点着色器的位置的句柄
    gl_position = glGetAttribLocation(mProgram, "vertexIn");

    glEnableVertexAttribArray(gl_position);
    gl_textCoord = glGetAttribLocation(mProgram, "textureIn");


    glEnableVertexAttribArray(gl_textCoord);

    glVertexAttribPointer(gl_position, 3, GL_FLOAT, GL_FALSE, 0, vertex_coords);
    glVertexAttribPointer(gl_textCoord, 2, GL_FLOAT, GL_FALSE, 0, fragment_coords);

    yang_trace("LoadProgram out gl_position : %d gl_textCoord: %d", gl_position, gl_textCoord);


    glUniform1i(glGetUniformLocation(mProgram, "tex_y"), 0);
    glUniform1i(glGetUniformLocation(mProgram, "tex_u"), 1);
    glUniform1i(glGetUniformLocation(mProgram, "tex_v"), 2);

    //创建若干个纹理对象，并且得到纹理ID
    glGenTextures(3, gl_texture_id);

    //将纹理目标和纹理绑定后，对纹理目标所进行的操作都反映到对纹理上
    glBindTexture(GL_TEXTURE_2D, gl_texture_id[0]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    glTexImage2D(GL_TEXTURE_2D,
                 0,//细节基本 默认0
                 GL_LUMINANCE,
				 m_width,//加载的纹理宽度, 最好为8的倍数, 否则会出现画面错乱，绿线，失真
                 m_height,//加载的纹理高度, 最好为2的次幂
                 0,//纹理边框
                 GL_LUMINANCE,
                 GL_UNSIGNED_BYTE,//像素点存储的数据类型
                 NULL //纹理的数据（先不传）
    );

    //绑定纹理
    glBindTexture(GL_TEXTURE_2D, gl_texture_id[1]);
    //缩小的过滤器
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    //设置纹理的格式和大小
    glTexImage2D(GL_TEXTURE_2D,
                 0,//细节基本 默认0
                 GL_LUMINANCE,
                 m_width >> 1,
                 m_height >> 1,
                 0,//边框
                 GL_LUMINANCE,
                 GL_UNSIGNED_BYTE,//像素点存储的数据类型
                 NULL //纹理的数据（先不传）
    );

    //绑定纹理
    glBindTexture(GL_TEXTURE_2D, gl_texture_id[2]);
    //缩小的过滤器
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    //设置纹理的格式和大小
    glTexImage2D(GL_TEXTURE_2D,
                 0,//细节基本 默认0
                 GL_LUMINANCE,
                 m_width >> 1,
                 m_height >> 1,
                 0,//边框
                 GL_LUMINANCE,
                 GL_UNSIGNED_BYTE,//像素点存储的数据类型
                 NULL //纹理的数据（先不传）
    );



    return mProgram;
}

/**
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
    yang_trace("changeVideoRotation");
    glVertexAttribPointer(gl_textCoord, 2, GL_FLOAT, GL_FALSE, 0, fragment_coords);
    yang_trace("changeVideoRotation in");
}
**/

void YangYuvGl::render(uint8_t *data) {
    //激活第一层纹理，绑定到创建的纹理
	int yuvlen=m_width*m_height;
    y_data = data;
    u_data = data+yuvlen;
    v_data = data+(yuvlen+yuvlen>>2);
    glActiveTexture(GL_TEXTURE0);
    //绑定y对应的纹理
    glBindTexture(GL_TEXTURE_2D, gl_texture_id[0]);
    //替换纹理，比重新使用glTexImage2D性能高多
    glTexSubImage2D(
            GL_TEXTURE_2D,
            0,
            0,
            0,//相对原来的纹理的offset
            m_width,
            m_height,//加载的纹理宽度、高度。最好为2的次幂
            GL_LUMINANCE,
            GL_UNSIGNED_BYTE,
            y_data);

    //激活第二层纹理，绑定到创建的纹理
    glActiveTexture(GL_TEXTURE1);
    //绑定u对应的纹理
    glBindTexture(GL_TEXTURE_2D, gl_texture_id[1]);
    //替换纹理，比重新使用glTexImage2D性能高
    glTexSubImage2D(
            GL_TEXTURE_2D,
            0,
            0,
            0,
            m_width >> 1,
            m_height >> 1,
            GL_LUMINANCE,
            GL_UNSIGNED_BYTE,
            u_data
            );

    //激活第三层纹理，绑定到创建的纹理
    glActiveTexture(GL_TEXTURE2);
    //绑定v对应的纹理
    glBindTexture(GL_TEXTURE_2D, gl_texture_id[2]);
    //替换纹理，比重新使用glTexImage2D性能高
    glTexSubImage2D(
            GL_TEXTURE_2D,
            0,
            0,
            0,
            m_width >> 1,
            m_height >> 1,
            GL_LUMINANCE,
            GL_UNSIGNED_BYTE,
            v_data
            );

    //glEnableVertexAttribArray(gl_uMatrix);
   // glUniformMatrix4fv(gl_uMatrix, 1, false, matrix_scale);

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
   // this->global_context->eglDisplay = eglDisplay;

    EGLint majorVersion;//主版本号
    EGLint minorVersion;//次版本号
   EGLBoolean success = eglInitialize(m_context.eglDisplay, &majorVersion, &minorVersion);
  //  EGLBoolean success = eglInitialize(m_context.eglDisplay, 0, 0);
    if (!success) {
        yang_trace("eglInitialize failure: %d", eglGetError());
        return -1;
    }
    yang_trace("eglInitialize ok");

    EGLint numConfigs;
    EGLConfig config;
    const EGLint CONFIG_ATTRIBS[] = { EGL_RED_SIZE, 8,
                                      EGL_GREEN_SIZE, 8,
                                      EGL_BLUE_SIZE, 8,
                                      EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                                      EGL_NONE // the end
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

   // const EGLint attribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
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
