#include "YangMouseListenWidget.h"
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QMouseEvent>

YangMouseListenWidget::YangMouseListenWidget(QWidget *parent) : QOpenGLWidget(parent)
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

    initKeybord();
    grabKeyboard();

}

YangMouseListenWidget::~YangMouseListenWidget()
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

void YangMouseListenWidget::initKeybord()
{
    m_keyboardMap[Qt::Key_Left]=0x25;
        m_keyboardMap[Qt::Key_Up]=0x26;
        m_keyboardMap[Qt::Key_Right]=0x27;
        m_keyboardMap[Qt::Key_Down]=0x28;
        m_keyboardMap[Qt::Key_Backspace]=0x08;
        m_keyboardMap[Qt::Key_Tab]=0x09;
        m_keyboardMap[Qt::Key_Clear]=0x0C;
        m_keyboardMap[Qt::Key_Return]=0x0D;
        m_keyboardMap[Qt::Key_Enter]=0x0D;
        m_keyboardMap[Qt::Key_Shift]=0x10;
        m_keyboardMap[Qt::Key_Control]=0x11;
        m_keyboardMap[Qt::Key_Alt]=0x12;
        m_keyboardMap[Qt::Key_Pause]=0x13;
        m_keyboardMap[Qt::Key_CapsLock]=0x14;
        m_keyboardMap[Qt::Key_Escape]=0x1B;
        m_keyboardMap[Qt::Key_Space]=0x20;
        m_keyboardMap[Qt::Key_PageUp]=0x21;
        m_keyboardMap[Qt::Key_PageDown]=0x22;
        m_keyboardMap[Qt::Key_End]=0x23;
        m_keyboardMap[Qt::Key_Home]=0x24;
        m_keyboardMap[Qt::Key_Select]=0x29;
        m_keyboardMap[Qt::Key_Print]=0x2A;
        m_keyboardMap[Qt::Key_Execute]=0x2B;
        m_keyboardMap[Qt::Key_Printer]=0x2C;
        m_keyboardMap[Qt::Key_Insert]=0x2D;
        m_keyboardMap[Qt::Key_Delete]=0x2E;
        m_keyboardMap[Qt::Key_Help]=0x2F;
        m_keyboardMap[Qt::Key_0]=0x30;
        m_keyboardMap[Qt::Key_ParenRight]=0x30; // )
        m_keyboardMap[Qt::Key_1]=0x31;
        m_keyboardMap[Qt::Key_Exclam]=0x31; // !
        m_keyboardMap[Qt::Key_2]=0x32;
        m_keyboardMap[Qt::Key_At]=0x32; // @
        m_keyboardMap[Qt::Key_3]=0x33;
        m_keyboardMap[Qt::Key_NumberSign]=0x33; // #
        m_keyboardMap[Qt::Key_4]=0x34;
        m_keyboardMap[Qt::Key_Dollar]=0x34; // $
        m_keyboardMap[Qt::Key_5]=0x35;
        m_keyboardMap[Qt::Key_Percent]=0x35; // %
        m_keyboardMap[Qt::Key_6]=0x36;
        m_keyboardMap[Qt::Key_AsciiCircum]=0x36; // ^
        m_keyboardMap[Qt::Key_7]=0x37;
        m_keyboardMap[Qt::Key_Ampersand]=0x37; // &
        m_keyboardMap[Qt::Key_8]=0x38;
        m_keyboardMap[Qt::Key_Asterisk]=0x38; // *
        m_keyboardMap[Qt::Key_9]=0x39;
        m_keyboardMap[Qt::Key_ParenLeft]=0x39; // (
        m_keyboardMap[Qt::Key_A]=0x41;
        m_keyboardMap[Qt::Key_B]=0x42;
        m_keyboardMap[Qt::Key_C]=0x43;
        m_keyboardMap[Qt::Key_D]=0x44;
        m_keyboardMap[Qt::Key_E]=0x45;
        m_keyboardMap[Qt::Key_F]=0x46;
        m_keyboardMap[Qt::Key_G]=0x47;
        m_keyboardMap[Qt::Key_H]=0x48;
        m_keyboardMap[Qt::Key_I]=0x49;
        m_keyboardMap[Qt::Key_J]=0x4A;
        m_keyboardMap[Qt::Key_K]=0x4B;
        m_keyboardMap[Qt::Key_L]=0x4C;
        m_keyboardMap[Qt::Key_M]=0x4D;
        m_keyboardMap[Qt::Key_N]=0x4E;
        m_keyboardMap[Qt::Key_O]=0x4F;
        m_keyboardMap[Qt::Key_P]=0x50;
        m_keyboardMap[Qt::Key_Q]=0x51;
        m_keyboardMap[Qt::Key_R]=0x52;
        m_keyboardMap[Qt::Key_S]=0x53;
        m_keyboardMap[Qt::Key_T]=0x54;
        m_keyboardMap[Qt::Key_U]=0x55;
        m_keyboardMap[Qt::Key_V]=0x56;
        m_keyboardMap[Qt::Key_W]=0x57;
        m_keyboardMap[Qt::Key_X]=0x58;
        m_keyboardMap[Qt::Key_Y]=0x59;
        m_keyboardMap[Qt::Key_Z]=0x5A;
        m_keyboardMap[Qt::Key_multiply]=0x6A;
        m_keyboardMap[Qt::Key_F1]=0x70;
        m_keyboardMap[Qt::Key_F2]=0x71;
        m_keyboardMap[Qt::Key_F3]=0x72;
        m_keyboardMap[Qt::Key_F4]=0x73;
        m_keyboardMap[Qt::Key_F5]=0x74;
        m_keyboardMap[Qt::Key_F6]=0x75;
        m_keyboardMap[Qt::Key_F7]=0x76;
        m_keyboardMap[Qt::Key_F8]=0x77;
        m_keyboardMap[Qt::Key_F9]=0x78;
        m_keyboardMap[Qt::Key_F10]=0x79;
        m_keyboardMap[Qt::Key_F11]=0x7A;
        m_keyboardMap[Qt::Key_F12]=0x7B;
        m_keyboardMap[Qt::Key_F13]=0x7C;
        m_keyboardMap[Qt::Key_F14]=0x7D;
        m_keyboardMap[Qt::Key_F15]=0x7E;
        m_keyboardMap[Qt::Key_F16]=0x7F;
        m_keyboardMap[Qt::Key_F17]=0x80;
        m_keyboardMap[Qt::Key_F18]=0x81;
        m_keyboardMap[Qt::Key_F19]=0x82;
        m_keyboardMap[Qt::Key_F20]=0x83;
        m_keyboardMap[Qt::Key_F21]=0x84;
        m_keyboardMap[Qt::Key_F22]=0x85;
        m_keyboardMap[Qt::Key_F23]=0x86;
        m_keyboardMap[Qt::Key_F24]=0x87;
        m_keyboardMap[Qt::Key_NumLock]=0x90;
        m_keyboardMap[Qt::Key_ScrollLock]=0x91;
        m_keyboardMap[Qt::Key_VolumeDown]=0xAE;
        m_keyboardMap[Qt::Key_VolumeUp]=0xAF;
        m_keyboardMap[Qt::Key_VolumeMute]=0xAD;
        m_keyboardMap[Qt::Key_MediaStop]=0xB2;
        m_keyboardMap[Qt::Key_MediaPlay]=0xB3;
        m_keyboardMap[Qt::Key_Plus]=0xBB; // +
        m_keyboardMap[Qt::Key_Minus]=0xBD; // -
        m_keyboardMap[Qt::Key_Underscore]=0xBD; // _
        m_keyboardMap[Qt::Key_Equal]=0xBB; // =
        m_keyboardMap[Qt::Key_Semicolon]=0xBA; // ;
        m_keyboardMap[Qt::Key_Colon]=0xBA; // :
        m_keyboardMap[Qt::Key_Comma]=0xBC; // ]=
        m_keyboardMap[Qt::Key_Less]=0xBC; // <
        m_keyboardMap[Qt::Key_Period]=0xBE; // .
        m_keyboardMap[Qt::Key_Greater]=0xBE; // >
        m_keyboardMap[Qt::Key_Slash]=0xBF;  // /
        m_keyboardMap[Qt::Key_Question]=0xBF; // ?
        m_keyboardMap[Qt::Key_BracketLeft]=0xDB; // [
        m_keyboardMap[Qt::Key_BraceLeft]=0xDB; // {
        m_keyboardMap[Qt::Key_BracketRight]=0xDD; // ]
        m_keyboardMap[Qt::Key_BraceRight]=0xDD; // }
        m_keyboardMap[Qt::Key_Bar]=0xDC; // |
        m_keyboardMap[Qt::Key_Backslash]=0xDC;
        m_keyboardMap[Qt::Key_Apostrophe]=0xDE; // '
        m_keyboardMap[Qt::Key_QuoteDbl]=0xDE; // "
        m_keyboardMap[Qt::Key_QuoteLeft]=0xC0; // `
        m_keyboardMap[Qt::Key_AsciiTilde]=0xC0; // ~
}
void YangMouseListenWidget::sendevent(string type, string direction, int x, int y)
{
    char msg[256]={0};
    if(direction == "key")
    {
        sprintf(msg,"%s,%s,%d,%d",type.c_str(),direction.c_str(),x,y);
        qDebug()<<"msg==="<<msg;
        // scocket->sendmsg(msg,strlen(msg));
        return;
    }

    float width = rect().width();
    float height = rect().height();

    float fx = x/width;
    float fy = y/height;
    sprintf(msg,"%s,%s,%f,%f",type.c_str(),direction.c_str(),fx,fy);
 qDebug()<<"msg==="<<msg;
     // scocket->sendmsg(msg,strlen(msg));

}
void YangMouseListenWidget::playVideo(unsigned char *p,int wid,int hei)
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

void YangMouseListenWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    m_vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);

    const char *vsrc = "attribute vec4 vertexIn; \
    attribute vec2 textureIn; \
    varying vec2 textureOut;  \
    void main(void)           \
    {                         \
        gl_Position = vertexIn; \
        textureOut = textureIn; \
    }";

    bool bCompile = m_vshader->compileSourceCode(vsrc);
    if(!bCompile)
    {
    }

    m_fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
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
    //将glsl源码送入编译器编译着色器程序
    bCompile = m_fshader->compileSourceCode(fsrc);
    if(!bCompile)
    {
    }
#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

    m_shaderProgram = new QOpenGLShaderProgram;
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
    glVertexAttribPointer(ATTRIB_TEXTURE, 2, GL_FLOAT, 0, 0, textureVertices);

    glEnableVertexAttribArray(ATTRIB_VERTEX);
    glEnableVertexAttribArray(ATTRIB_TEXTURE);

    m_textureY = new QOpenGLTexture(QOpenGLTexture::Target2D);
    m_textureU = new QOpenGLTexture(QOpenGLTexture::Target2D);
    m_textureV = new QOpenGLTexture(QOpenGLTexture::Target2D);
    m_textureY->create();
    m_textureU->create();
    m_textureV->create();

    id_y = m_textureY->textureId();
    id_u = m_textureU->textureId();
    id_v = m_textureV->textureId();
    glClearColor(0.3,0.3,0.3,0.0);//设置背景色

}

void YangMouseListenWidget::resizeGL(int w, int h)
{
    if(h == 0)// 防止被零除
    {
        h = 1;// 将高设为1
    }

    glViewport(0,0, w,h);
}

void YangMouseListenWidget::paintGL()
{
    //if(m_pBufYuv420p==NULL) return;
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
    return;
 }

void YangMouseListenWidget::mousePressEvent(QMouseEvent *event)
{
    string direction = "";
    if(event->button()==Qt::LeftButton)
    {
        direction = "left";
    }
    else if(event->button()==Qt::RightButton)
    {
        direction = "right";
    }

    QPoint pt = event->pos();
    int x=  pt. x();
    int y = pt.y();

    sendevent("mousePress",direction,x,y);

}

void YangMouseListenWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    string direction = "";

    if(event->button()==Qt::LeftButton)
    {
        direction = "left";
    }
    else if(event->button()==Qt::RightButton)
    {
        direction = "right";
    }

    QPoint pt = event->pos();
    int x=  pt. x();
    int y = pt.y();
    sendevent("mouseDouble",direction,x,y);
}

void YangMouseListenWidget::mouseReleaseEvent(QMouseEvent *event)
{
    string direction = "";
    if(event->button()==Qt::LeftButton)
    {
        direction = "left";
    }
    else if(event->button()==Qt::RightButton)
    {
        direction = "right";
    }
    QPoint pt = event->pos();
    int x=  pt. x();
    int y = pt.y();

    sendevent("mouseRelease",direction,x,y);
}

void YangMouseListenWidget::mouseMoveEvent(QMouseEvent *event)
{
    string direction = "";
    if(event->button()==Qt::LeftButton)
    {
        direction = "left";
    }
    else if(event->button()==Qt::RightButton)
    {
        direction = "right";
    }
    QPoint pt = event->pos();
    int x=  pt. x();
    int y = pt.y();

    sendevent("mouseMove",direction,x,y);
}
void YangMouseListenWidget::wheelEvent(QWheelEvent *event){
           QPoint pt = event->pos();
         int x=  pt.x();
         int y = pt.y();
         if (event->orientation() == Qt::Vertical)     sendevent("wheel",to_string(event->delta()),x,y);


}
void YangMouseListenWidget::keyPressEvent(QKeyEvent *event)
{

    qDebug()<<"keyPressEvent==="<<event->key();
    map<int,uint8_t>::iterator it=m_keyboardMap.find( event->key());
    if(it!=m_keyboardMap.end()){
     sendevent("keyPressEvent","key",0,it->second);
    }
}

void YangMouseListenWidget::keyReleaseEvent(QKeyEvent *event)
{
    qDebug()<<"keyReleaseEvent==="<<event->key();
    map<int,uint8_t>::iterator it=m_keyboardMap.find( event->key());
    if(it!=m_keyboardMap.end()){
     sendevent("keyReleaseEvent","key",0,it->second);
    }
}
