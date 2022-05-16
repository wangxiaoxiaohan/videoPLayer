#ifndef GLTEST_H
#define GLTEST_H


#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QFile>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#define ATTRIB_VERTEX 3
#define ATTRIB_TEXTURE 4
class glTest:public QOpenGLWidget,protected QOpenGLFunctions
{
    Q_OBJECT
public:

    glTest(QWidget* parent);
    ~glTest();
protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
private:
  QOpenGLShaderProgram *m_program;
  QOpenGLBuffer vertexBuffer, vertexIndexBuffer;
};
#endif
