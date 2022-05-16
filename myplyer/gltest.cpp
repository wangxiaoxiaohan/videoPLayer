#include "gltest.h"
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QMouseEvent>

glTest::glTest(QWidget *parent):QOpenGLWidget(parent)
{
 m_program = NULL;
}
glTest::~glTest()
{
}
static const char *vertexShaderSource =
    "attribute vec2 position;\n"
    "varying vec2 texcoord;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(position, 0.0, 1.0);\n"
    "    texcoord = position * vec2(0.5) + vec2(0.5);\n"
    "}\n";

static const char *fragmentShaderSource =
    /* Precision is available but no-op on GLSL 130 (see [3], section 4.5.2)
     * and mandatory for OpenGL ES. It was first reserved on GLSL 120 and
     * didn't exist in GLSL <= 110 (= OpenGL 2.0). Since it's a no-op, the
     * easiest solution is to never use it for OpenGL code.
     *
     * [1]: https://www.khronos.org/registry/OpenGL/specs/gl/GLSLangSpec.1.10.pdf
     * [2]: https://www.khronos.org/registry/OpenGL/specs/gl/GLSLangSpec.1.20.pdf
     * [3]: https://www.khronos.org/registry/OpenGL/specs/gl/GLSLangSpec.1.30.pdf
     */
    "#ifdef GL_ES\n"
    "precision highp float;\n"
    "#endif\n"

    "\n"
    "uniform sampler2D texture;\n"
    "\n"
    "varying vec2 texcoord;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    gl_FragColor = texture2D(texture, texcoord);\n"
    "}\n";

/*
 * Data used to seed our vertex array and element array buffers:
 */
static const GLfloat  g_vertex_buffer_data[] = {
    -1.0f, -1.0f,
     1.0f, -1.0f,
    -1.0f,  1.0f,
     1.0f,  1.0f
};
static const GLfloat  trigle_vertex_buffer_data[] = {
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.0f, 0.5f, 0.0f
};

static const GLushort g_element_buffer_data[] = { 0, 1, 2, 3 };
void glTest::initializeGL() {
    vertexBuffer.create();
    vertexBuffer.bind();
    vertexBuffer.allocate(g_vertex_buffer_data, sizeof(g_vertex_buffer_data));

    vertexIndexBuffer.create();
    vertexIndexBuffer.bind();
    vertexIndexBuffer.allocate(g_element_buffer_data, sizeof(g_element_buffer_data));

    m_program = new QOpenGLShaderProgram();
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->link();

    m_program->setUniformValue("texture", 0);

    m_program->bindAttributeLocation("position", 0);
}
void glTest::resizeGL(int w, int h) {

}
void glTest::paintGL() {
    // draw scene
    printf("glTest paintGL\n");
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    static GLfloat const Vertices[] = {
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        0.0f, 1.0f, -1.0f
    };

    QMatrix4x4 pmvMatrix;
    pmvMatrix.ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 10.0f);
    int vertexLocation = m_program->attributeLocation("inPos");
    int matrixLocation = m_program->uniformLocation("matrix");
    m_program->enableAttributeArray(vertexLocation);
    m_program->setAttributeArray(vertexLocation, Vertices, 3);
    m_program->setUniformValue(matrixLocation, pmvMatrix);
    f->glDrawArrays(GL_TRIANGLES, 0, 3);
    m_program->disableAttributeArray(vertexLocation);
}
