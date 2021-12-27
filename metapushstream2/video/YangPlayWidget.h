#ifndef YangPlayWidget_H
#define YangPlayWidget_H

#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>


#define ATTRIB_VERTEX 3
#define ATTRIB_TEXTURE 4

class YangPlayWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    YangPlayWidget(QWidget* parent);
    ~YangPlayWidget();
    void PlayOneFrame(unsigned char *p,int wid,int hei);
protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
private:
    GLuint textureUniformY; //y纹理数据位置
    GLuint textureUniformU; //u纹理数据位置
    GLuint textureUniformV; //v纹理数据位置
    GLuint id_y; //y纹理对象ID
    GLuint id_u; //u纹理对象ID
    GLuint id_v; //v纹理对象ID
    QOpenGLTexture* m_pTextureY;  //y纹理对象
    QOpenGLTexture* m_pTextureU;  //u纹理对象
    QOpenGLTexture* m_pTextureV;  //v纹理对象
    QOpenGLShader *m_pVSHader;  //顶点着色器程序对象
    QOpenGLShader *m_pFSHader;  //片段着色器对象
    QOpenGLShaderProgram *m_pShaderProgram; //着色器程序容器
    int m_nVideoW; //视频分辨率宽
    int m_nVideoH; //视频分辨率高
    unsigned char* m_pBufYuv420p;

};

#endif // CPLAYWIDGET_H
