//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YangMouseListenWidget_H
#define YangMouseListenWidget_H

#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include "yangplayer/YangWinPlayFactroy.h"
#include <map>
using namespace std;

#define ATTRIB_VERTEX 3
#define ATTRIB_TEXTURE 4

class YangMouseListenWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    YangMouseListenWidget(QWidget* parent);
    virtual ~YangMouseListenWidget();
     void playVideo(unsigned char *p,int wid,int hei);


protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) override ;
    void mouseDoubleClickEvent(QMouseEvent *event) override ;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;


private:
    GLuint textureUniformY;
    GLuint textureUniformU;
    GLuint textureUniformV;
    GLuint id_y;
    GLuint id_u;
    GLuint id_v;
    QOpenGLTexture* m_textureY;
    QOpenGLTexture* m_textureU;
    QOpenGLTexture* m_textureV;
    QOpenGLShader *m_vshader;
    QOpenGLShader *m_fshader;
    QOpenGLShaderProgram *m_shaderProgram;
    int m_nVideoW;
    int m_nVideoH;
    unsigned char* m_pBufYuv420p;

    map<int,uint8_t> m_keyboardMap;//

    void initKeybord();
    void sendevent(string type, string direction, int x, int y);

};

#endif // CPLAYWIDGET_H
