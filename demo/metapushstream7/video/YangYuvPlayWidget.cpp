//
// Copyright (c) 2019-2022 yanggaofeng
//
#include "YangYuvPlayWidget.h"
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#if defined(__APPLE__)
#define ATTRIB_VERTEX 3
#define ATTRIB_TEXTURE 4

YangYuvPlayWidget::YangYuvPlayWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    setAutoFillBackground(false);

    textureUniformY = 0;
    textureUniformU = 0;
    textureUniformV = 0;
    id_y = 0;
    id_u = 0;
    id_v = 0;
    m_pBufYuv420p = NULL;


    m_nVideoH = 0;
    m_nVideoW = 0;
    m_nViewW=0;
    m_nViewH=0;


}

YangYuvPlayWidget::~YangYuvPlayWidget()
{

}




void YangYuvPlayWidget::playVideo(unsigned char *p,int wid,int hei)
{


    if(m_nVideoW != wid)
    {
        m_nVideoW = wid;
        m_nVideoH = hei;
    }

    m_pBufYuv420p=p;
    update();

    return;
}




GLuint YangYuvPlayWidget::createProgram()
{
    const char *vsrc =
            "#version 410\n"
            "in vec4 vertexIn;\n"
            "in vec2 textureIn;\n"
            "out vec2 textureOut;\n"
            "void main(void)\n"
            "{\n"
            "    gl_Position = vertexIn;\n"
            "    textureOut = textureIn;\n"
            "}";

    const char *fsrc =
            "#version 410\n"
            "in vec2 textureOut;\n"
            "out vec4 fragColor;\n"
            "uniform sampler2D tex_y;\n"
            "uniform sampler2D tex_u;\n"
            "uniform sampler2D tex_v;\n"
            "void main(void)\n"
            "{\n"
            "    vec3 yuv;\n"
            "    vec3 rgb;\n"
            "    yuv.x = texture(tex_y, textureOut).r;\n"
            "    yuv.y = texture(tex_u, textureOut).r - 0.5;\n"
            "    yuv.z = texture(tex_v, textureOut).r - 0.5;\n"
            "    rgb = mat3( 1,       1,         1,\n"
            "               0,       -0.21482,  2.12798,\n"
            "               1.28033, -0.38059,  0) * yuv;\n"
            "    fragColor = vec4(rgb, 1);\n"
            "}";
    GLuint program = glCreateProgram();

    //vertex shader
    GLuint vertexShader = glCreateShader( GL_VERTEX_SHADER);
    glShaderSource( vertexShader, 1, (const GLchar**)&vsrc, NULL );
    glCompileShader( vertexShader );
    GLint  compiled;
    glGetShaderiv( vertexShader, GL_COMPILE_STATUS, &compiled );
    if ( !compiled ) {
        GLint  logSize;
        glGetShaderiv( vertexShader, GL_INFO_LOG_LENGTH, &logSize );
        char* logMsg = new char[logSize];
        glGetShaderInfoLog( vertexShader, logSize, NULL, logMsg );
        qWarning() << logMsg;
        delete [] logMsg;
        exit( EXIT_FAILURE );
    }
    glAttachShader( program, vertexShader );

    //fragment shader
    GLuint fragmentShader = glCreateShader( GL_FRAGMENT_SHADER);
    glShaderSource( fragmentShader, 1, (const GLchar**)&fsrc, NULL );
    glCompileShader( fragmentShader );
    glGetShaderiv( fragmentShader, GL_COMPILE_STATUS, &compiled );
    if ( !compiled ) {
        GLint  logSize;
        glGetShaderiv( vertexShader, GL_INFO_LOG_LENGTH, &logSize );
        char* logMsg = new char[logSize];
        glGetShaderInfoLog( fragmentShader, logSize, NULL, logMsg );
        qWarning() << logMsg;
        delete [] logMsg;
        exit( EXIT_FAILURE );
    }
    glAttachShader( program, fragmentShader );

    /* Link output */
    glBindFragDataLocation(program, 0, "fragColor");

    /* link  and error check */
    glLinkProgram(program);

    GLint  linked;
    glGetProgramiv( program, GL_LINK_STATUS, &linked );
    if ( !linked ) {
        qWarning() << "Shader program failed to link";
        GLint  logSize;
        glGetProgramiv( program, GL_INFO_LOG_LENGTH, &logSize);
        char* logMsg = new char[logSize];
        glGetProgramInfoLog( program, logSize, NULL, logMsg );
        qWarning() << logMsg ;
        delete [] logMsg;

        exit( EXIT_FAILURE );
    }


    glUseProgram(program);

    return program;
}


void YangYuvPlayWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    m_shader = createProgram();

    textureUniformY = glGetUniformLocation(m_shader, "tex_y");
    textureUniformU = glGetUniformLocation(m_shader, "tex_u");
    textureUniformV = glGetUniformLocation(m_shader, "tex_v");



    float vertexPoints[] ={
        -1.0f, -1.0f,0.0f, 1.0f,
        1.0f, -1.0f,1.0f, 1.0f,
        -1.0f, 1.0f,0.0f, 0.0f,
        1.0f, 1.0f,1.0f, 0.0f,
    };

    glGenVertexArrays(1, &m_vertexBuffer);
    glBindVertexArray(m_vertexBuffer);

    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 4 * 4 * sizeof(float), vertexPoints, GL_STATIC_DRAW);
    vertextAttribute = glGetAttribLocation(m_shader, "vertexIn");
    textureAttribute = glGetAttribLocation(m_shader, "textureIn");
    glEnableVertexAttribArray(vertextAttribute);
    glVertexAttribPointer(vertextAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (const GLvoid *)0);
    glEnableVertexAttribArray(textureAttribute);
    glVertexAttribPointer(textureAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (const GLvoid *)(sizeof(float)*2));


    //Init Texture
    glGenTextures(1, &id_y);
    glBindTexture(GL_TEXTURE_2D, id_y);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenTextures(1, &id_u);
    glBindTexture(GL_TEXTURE_2D, id_u);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenTextures(1, &id_v);
    glBindTexture(GL_TEXTURE_2D, id_v);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

}

void YangYuvPlayWidget::resizeGL(int w, int h)
{
    if(h == 0)// 防止被零除
    {
        h = 1;// 将高设为1
    }
    m_nViewW = w;
    m_nViewH = h;

    glViewport(0,0, w,h);
}

void YangYuvPlayWidget::paintGL()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    float x,y;
    float wRatio = (float)m_nViewW/m_nVideoW;
    float hRatio = (float)m_nViewH/m_nVideoH;
    float minRatio = qMin(wRatio, hRatio);
    y = m_nVideoH * minRatio/m_nViewH;
    x = m_nVideoW * minRatio/m_nViewW;

    float vertexPoints[] ={
        -x, -y,0.0f, 1.0f,
        x, -y,1.0f, 1.0f,
        -x, y,0.0f, 0.0f,
        x, y,1.0f, 0.0f,
    };
    glBufferData(GL_ARRAY_BUFFER, 4 * 4 * sizeof(float), vertexPoints, GL_STATIC_DRAW);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, id_y);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_nVideoW, m_nVideoH, 0, GL_RED, GL_UNSIGNED_BYTE, m_pBufYuv420p);
    glUniform1i(textureUniformY, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, id_u);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_nVideoW / 2, m_nVideoH / 2, 0, GL_RED, GL_UNSIGNED_BYTE, (char*)m_pBufYuv420p + m_nVideoW*m_nVideoH);
    glUniform1i(textureUniformU, 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, id_v);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_nVideoW / 2, m_nVideoH / 2, 0, GL_RED, GL_UNSIGNED_BYTE, (char*)m_pBufYuv420p + m_nVideoW*m_nVideoH * 5 / 4);
    glUniform1i(textureUniformV, 2);
    // Draw stuff
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );



    return;
}
#endif
