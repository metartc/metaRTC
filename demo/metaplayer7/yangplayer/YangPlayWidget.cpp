//
// Copyright (c) 2019-2022 yanggaofeng
//
#include "YangPlayWidget.h"
#include <QOpenGLTexture>
#include <QOpenGLBuffer>


YangPlayWidget::YangPlayWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    textureUniformY = 0;
    textureUniformU = 0;
    textureUniformV = 0;
    id_y = 0;
    id_u = 0;
    id_v = 0;
    m_pBufYuv420p = NULL;
    m_vshader = NULL;
    m_fshader = NULL;
    m_shaderProgram = NULL;
    m_textureY = NULL;
    m_textureU = NULL;
    m_textureV = NULL;

    m_nVideoH = 0;
    m_nVideoW = 0;
#if defined (__APPLE__)
   // QSurfaceFormat format;
   // format.setVersion(2,0);
   // format.setProfile(QSurfaceFormat::CoreProfile);
    //QSurfaceFormat::setDefaultFormat(format);
#endif

   // QPalette pal(palette());
   // pal.setColor(QPalette::Background, Qt::black);
   // setAutoFillBackground(true);
   // setPalette(pal);

}

YangPlayWidget::~YangPlayWidget()
{
    if(m_textureY){
        makeCurrent();
         m_textureY->destroy();
         m_textureU->destroy();
         m_textureV->destroy();
         doneCurrent();
         m_pBufYuv420p=nullptr;
         m_vshader = NULL;
         m_fshader = NULL;
         m_textureY = NULL;
         m_textureU = NULL;
         m_textureV = NULL;
    }
}



void YangPlayWidget::playVideo(unsigned char *p,int wid,int hei)
{


    if(m_nVideoW != wid)
    {
        m_nVideoW = wid;
        m_nVideoH = hei;
     }

    m_pBufYuv420p=p;
    update();
   // m_pBufYuv420p=NULL;
    return;
}

void YangPlayWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    m_vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
#if !defined (__APPLE__)
    const char *vsrc = R"(attribute highp vec4 vertexIn;
            attribute highp vec2 textureIn;
            varying highp vec2 textureOut;
            void main(void)
            {
                textureOut = textureIn;
                gl_Position = vertexIn;
            })";

        const char *fsrc = R"(varying   highp vec2 textureOut;
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


#else
    const char *vsrc = "attribute vec4 vertexIn; \
    attribute vec2 textureIn; \
    varying vec2 textureOut;  \
    void main(void)           \
    {                         \
        gl_Position = vertexIn; \
        textureOut = textureIn; \
    }";

    const char *fsrc = "varying vec2 textureOut; \
    uniform sampler2D tex_y; \
    uniform sampler2D tex_u; \
    uniform sampler2D tex_v; \
    void main(void) \
    { \
        vec3 yuv; \
        vec3 rgb; \
        yuv.x = texture2D(tex_y, textureOut).r; \
        yuv.y = texture2D(tex_u, textureOut).r - 0.5; \
        yuv.z = texture2D(tex_v, textureOut).r - 0.5; \
        rgb = mat3( 1,       1,         1, \
                    0,       -0.34414,  1.772, \
                    1.402, -0.71414,  0) * yuv; \
        gl_FragColor = vec4(rgb, 1); \
    }";
#endif
    bool bCompile = m_vshader->compileSourceCode(vsrc);
    if(!bCompile)
    {
        yang_error("opengl compile source vsrc fail!");
        qDebug()<<"opengl compile source vsrc fail!";
    }else{
        qDebug()<<"opengl compile source vsrc success!";
    }

    m_fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);

    //将glsl源码送入编译器编译着色器程序
    bCompile = m_fshader->compileSourceCode(fsrc);
    if(!bCompile)
    {
        yang_error("opengl compile source fsrc fail!");
        qDebug()<<"opengl compile source fsrc fail!";
    }else{
        qDebug()<<"opengl compile source fsrc success!";
    }
#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

    m_shaderProgram = new QOpenGLShaderProgram();
    m_shaderProgram->addShader(m_fshader);
    m_shaderProgram->addShader(m_vshader);
    m_shaderProgram->bindAttributeLocation("vertexIn", ATTRIB_VERTEX);
    m_shaderProgram->bindAttributeLocation("textureIn", ATTRIB_TEXTURE);
    m_shaderProgram->link();
    m_shaderProgram->bind();

    textureUniformY = m_shaderProgram->uniformLocation("tex_y");
    textureUniformU =  m_shaderProgram->uniformLocation("tex_u");
    textureUniformV =  m_shaderProgram->uniformLocation("tex_v");

    static const GLfloat vertexVertices[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
         -1.0f, 1.0f,
         1.0f, 1.0f,
    };

    static const GLfloat textureVertices[] = {
        0.0f,  1.0f,
        1.0f,  1.0f,
        0.0f,  0.0f,
        1.0f,  0.0f,
    };

    glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, 0, 0, vertexVertices);
    glEnableVertexAttribArray(ATTRIB_VERTEX);

    glVertexAttribPointer(ATTRIB_TEXTURE, 2, GL_FLOAT, 0, 0, textureVertices);
    glEnableVertexAttribArray(ATTRIB_TEXTURE);

#if defined(__APPLE__)
    glGenTextures(1, &id_y);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, id_y);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glUniform1i(textureUniformY, 0);

    glGenTextures(1, &id_u);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, id_u);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glUniform1i(textureUniformU, 1);

    glGenTextures(1, &id_v);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, id_v);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glUniform1i(textureUniformV, 2);
#else
    m_textureY = new QOpenGLTexture(QOpenGLTexture::Target2D);
    m_textureU = new QOpenGLTexture(QOpenGLTexture::Target2D);
    m_textureV = new QOpenGLTexture(QOpenGLTexture::Target2D);
    m_textureY->create();
    m_textureU->create();
    m_textureV->create();
    id_y = m_textureY->textureId();
    id_u = m_textureU->textureId();
    id_v = m_textureV->textureId();
#endif
    glClearColor(0.3,0.3,0.3,0.0);//设置背景色
}

void YangPlayWidget::resizeGL(int w, int h)
{
    if(h == 0)// 防止被零除
    {
        h = 1;// 将高设为1
    }

    glViewport(0,0, w,h);
}

void YangPlayWidget::paintGL()
{
    //if(m_pBufYuv420p==NULL) return;
#if defined (__APPLE__)
    glBindTexture(GL_TEXTURE_2D, id_y);

    glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE,m_nVideoW,m_nVideoH,
                 0,GL_LUMINANCE, GL_UNSIGNED_BYTE,m_pBufYuv420p);

    glBindTexture(GL_TEXTURE_2D, id_u);
    glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE, m_nVideoW >> 1,m_nVideoH >> 1,
                 0,GL_LUMINANCE,GL_UNSIGNED_BYTE,m_pBufYuv420p+m_nVideoW*m_nVideoH);

    glBindTexture(GL_TEXTURE_2D, id_v);
    glTexImage2D(GL_TEXTURE_2D,0, GL_LUMINANCE, m_nVideoW >> 1,m_nVideoH >> 1,
                 0,GL_LUMINANCE,GL_UNSIGNED_BYTE,m_pBufYuv420p+m_nVideoW*m_nVideoH*5/4);

    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
#else
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, id_y);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, m_nVideoW, m_nVideoH, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, m_pBufYuv420p);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, id_u);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, m_nVideoW/2, m_nVideoH/2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, m_pBufYuv420p+m_nVideoW*m_nVideoH);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, id_v);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, m_nVideoW/2, m_nVideoH/2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, m_pBufYuv420p+m_nVideoW*m_nVideoH*5/4);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glUniform1i(textureUniformY, 0);
    glUniform1i(textureUniformU, 1);
    glUniform1i(textureUniformV, 2);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
#endif

    return;
 }

