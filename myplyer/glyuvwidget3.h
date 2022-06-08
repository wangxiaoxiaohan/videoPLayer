#ifndef glyuvwidget2_H
#define glyuvwidget2_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QMatrix4x4>
//QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)
//QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)

class glyuvwidget2 : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    glyuvwidget2(QWidget *parent);
    ~glyuvwidget2();

public slots:
    void slotShowYuv(uchar *ptr_y,uchar *ptr_u,uchar *ptr_v,uint width, uint height); //显示一帧Yuv图像
protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;

private:
    QOpenGLShaderProgram *m_pShaderProgram;
    //QOpenGLBuffer vbo;
    GLuint textureUniformY,textureUniformU,textureUniformV; //opengl中y、u、v分量位置
   // QOpenGLTexture *textureY = nullptr,*textureU = nullptr,*textureV = nullptr;
    GLuint id_y,id_u,id_v; //自己创建的纹理对象ID，创建错误返回0
    uint m_nVideoW,m_nVideoH;
    uchar *m_pBufYuv420p_y = nullptr;
    uchar *m_pBufYuv420p_u = nullptr;
    uchar *m_pBufYuv420p_v = nullptr;
    QMatrix4x4 projection;
    GLfloat vertices[8];//GLfloat
};

#endif // glyuvwidget_H
