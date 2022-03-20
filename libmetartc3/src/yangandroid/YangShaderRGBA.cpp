//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangandroid/YangShaderRGBA.h>
#include <yangutil/sys/YangLog.h>
YangShaderRGBA::YangShaderRGBA(YangGlobalContexts *global_context) {
    gl_program = -1;
    gl_uMatrix = -1;
    gl_position = -1;
    gl_textCoord = -1;
    this->context = global_context;
    setImageSize(this->context->gl_image_width, this->context->gl_image_height);
    setWindowSize(this->context->gl_window_width, this->context->gl_window_height);
    initDefMatrix();
}

YangShaderRGBA::~YangShaderRGBA() {
    if(NULL != vertex_shader_graphical) {
        delete vertex_shader_graphical;
        vertex_shader_graphical = NULL;
    }
    if(NULL != fragment_shader_graphical) {
        delete fragment_shader_graphical;
        fragment_shader_graphical = NULL;
    }

    glDeleteTextures(1, &gl_texture_id);
    glDeleteProgram(gl_program);
}

GLuint YangShaderRGBA::LoadShader(GLenum type, const char *shaderSrc) {
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

GLuint YangShaderRGBA::LoadProgram(const char *vShaderStr, const char *fShaderStr) {
    yang_trace("LoadProgram in");
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint mProgram;

    //eglMakeCurrent()函数来将当前的上下文切换，这样opengl的函数才能启动作用
    if(EGL_TRUE != eglMakeCurrent(context->eglDisplay,
                                  context->eglSurface, context->eglSurface,
                                  context->eglContext)) {
        yang_trace("eglMakeCurrent failed");
        return -1;
    }

    // Load the vertex/fragment shaders
    vertexShader = LoadShader(GL_VERTEX_SHADER, vShaderStr);
    if(0 == vertexShader) {
        yang_trace("LoadShader vertexShader failed");
        return -1;
    }

    fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fShaderStr);
    if(0 == fragmentShader) {
        yang_trace("LoadShader vertexShader failed");
        return -1;
    }

    // Create the program object
    mProgram = glCreateProgram();

    context->mProgram = mProgram;
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
        yang_trace("glLinkProgram failed : %s", log);
        glDeleteProgram (mProgram);
        return -1;
    }
    yang_trace("glLinkProgram success");

    glDeleteShader (vertexShader);
    glDeleteShader (fragmentShader);

    //获取顶点着色器和片段着色器句柄句柄
    gl_position = glGetAttribLocation(mProgram, "a_position");
    context->gl_position = gl_position;
    glEnableVertexAttribArray(gl_position);
    yang_trace("LoadProgram gl_position : %d", gl_position);
    glVertexAttribPointer(gl_position, 3, GL_FLOAT, GL_FALSE, 0, vertex_coords);

    gl_textCoord = glGetAttribLocation(mProgram, "a_texCoord");
    context->gl_textCoord = gl_textCoord;
    glEnableVertexAttribArray(gl_textCoord);
    yang_trace("LoadProgram gl_textCoord : %d", gl_textCoord);

    glVertexAttribPointer(gl_textCoord, 2, GL_FLOAT, GL_FALSE, 0, fragment_coords);

    glUniform1i(glGetUniformLocation(gl_program, "uTexture"), 0);

    gl_uMatrix = glGetUniformLocation(mProgram, "uMatrix");
    yang_trace("LoadProgram gl_uMatrix : %d", gl_uMatrix);

    //创建若干个纹理对象，并且得到纹理ID
    glGenTextures(1, &gl_texture_id);

    //将纹理目标和纹理绑定后，对纹理目标所进行的操作都反映到对纹理上
    glBindTexture(GL_TEXTURE_2D, gl_texture_id);

    glTexImage2D(GL_TEXTURE_2D,
                 0,//细节基本 默认0
                 GL_RGBA,
                 context->gl_image_width,//加载的纹理宽度
                 context->gl_image_height,//加载的纹理高度
                 0,//纹理边框
                 GL_RGBA,//数据的像素格式, RGBA数据格式
                 GL_UNSIGNED_BYTE,//像素点存储的数据类型
                 NULL //纹理的数据（先不传）
    );

    //过滤器
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //纹理环绕方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, context->gl_window_width, context->gl_window_height);

    glUseProgram(gl_program);
    yang_trace("LoadProgram out");
    return gl_program;
}

int YangShaderRGBA::createProgram() {
    yang_trace("CreateProgram in : ");
    //可以读Assets文件
    vertex_shader_graphical = YangShaderUtils::openAssetsFile(this->context->assetManager, "vertex_texture_display.glsl");
    fragment_shader_graphical = YangShaderUtils::openAssetsFile(this->context->assetManager, "fragment_texture_display.glsl");
    gl_program = LoadProgram(vertex_shader_graphical->c_str(), fragment_shader_graphical->c_str());

    yang_trace("CreateProgram : gl_program: %d  image_width: %d, image_height： %d, window_width：%d, window_height：%d",
         gl_program, gl_image_width, gl_image_height, gl_window_width, gl_window_height);
    return gl_program;
}

void YangShaderRGBA::render(unsigned char *data) {
    this->rgba_data = data;

    glActiveTexture(GL_TEXTURE0);
    //绑定对应的纹理
    glBindTexture(GL_TEXTURE_2D, gl_texture_id);

    //替换纹理，比重新使用glTexImage2D性能高多
    glTexSubImage2D(GL_TEXTURE_2D,
                    0,//细节基本 默认0
                    0, //相对原来的纹理的offset
                    0,//相对原来的纹理的offset
                    context->gl_image_width,
                    context->gl_image_height,//加载的纹理宽度、高度。最好为2的次幂
                    GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    this->rgba_data);

    glEnableVertexAttribArray(gl_uMatrix);
    glUniformMatrix4fv(gl_uMatrix, 1, false, matrix_scale);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    //窗口显示，交换双缓冲区
    eglSwapBuffers(context->eglDisplay, context->eglSurface);
}

void YangShaderRGBA::setImageSize(int width, int height) {
    this->gl_image_width = width;
    this->gl_image_height = height;
}

void YangShaderRGBA::setWindowSize(int width, int height) {
    this->gl_window_width = width;
    this->gl_window_height = height;
}

void YangShaderRGBA::initDefMatrix() {
    yang_trace("gl_image_width: %d, gl_image_height: %d, gl_window_width: %d, gl_window_height: %d",
         gl_image_width, gl_image_height, gl_window_width, gl_window_height);
    float originRatio = (float) gl_image_width / gl_image_height;
    float worldRatio = (float) gl_window_width / gl_window_height;

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

void YangShaderRGBA::orthoM(float *m, int mOffset, float left, float right, float bottom, float top,
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
