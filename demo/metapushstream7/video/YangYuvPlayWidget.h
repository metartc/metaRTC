//
// Copyright (c) 2019-2023 yanggaofeng
//
#ifndef YangYuvPlayWidget_H
#define YangYuvPlayWidget_H
#if defined(__APPLE__)
#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>

class YangYuvPlayWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    YangYuvPlayWidget(QWidget* parent);
     ~YangYuvPlayWidget();
    void playVideo(unsigned char *p,int wid,int hei);
protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    GLuint createProgram();
private:
    GLuint m_shader;
    GLuint m_vertexBuffer;
    GLuint textureUniformY;
    GLuint textureUniformU;
    GLuint textureUniformV;
    GLuint vertextAttribute;
    GLuint textureAttribute;

    GLuint id_y;
    GLuint id_u;
    GLuint id_v;

    int m_nVideoW;
    int m_nVideoH;

    int m_nViewW;
    int m_nViewH;

    uint8_t* m_pBufYuv420p;

};
#endif
#endif // CPLAYWIDGET_H

