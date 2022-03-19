//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YUVPLAYER_SHADERYUV_H
#define YUVPLAYER_SHADERYUV_H


#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <math.h>
#include <yangandroid/YangGlobalContexts.h>
#include <yangandroid/YangShaderUtils.h>


class YangShaderYUV {
public:
    YangShaderYUV(YangGlobalContexts *global_context);
    ~YangShaderYUV();
    GLuint LoadShader(GLenum type, const char *shaderSrc);
    GLuint LoadProgram(const char *vShaderStr, const char *fShaderStr);
    int createProgram();
    void changeVideoRotation();
    void render(uint8_t *data);
    void setVideoSize(int width, int height);
    void setWindowSize(int width, int height);
    void initDefMatrix();
    void orthoM(float m[], int mOffset,
                float left, float right, float bottom, float top,
                float near, float far);

    float vertex_coords[12] = {//世界坐标
            -1, -1, 0, // left bottom
            1, -1, 0, // right bottom
            -1, 1, 0,  // left top
            1, 1, 0,   // right top
    };

    float fragment_coords[8] = {//纹理坐标
            0, 1,//left bottom
            1, 1,//right bottom
            0, 0,//left top
            1, 0,//right top
    };

    float matrix_scale[16];
    GLint gl_program;
    GLint gl_position;
    GLint gl_textCoord;
    GLint gl_uMatrix;
    GLint gl_video_width;
    GLint gl_video_height;
    GLint gl_window_width;
    GLint gl_window_height;
    GLuint gl_texture_id[3];
    YangGlobalContexts *context;
    uint8_t *y_data;
    uint8_t *u_data;
    uint8_t *v_data;

    std::string *vertex_shader_graphical = NULL;
    std::string *fragment_shader_graphical = NULL;
};


#endif //YUVPLAYER_SHADERYUV_H
