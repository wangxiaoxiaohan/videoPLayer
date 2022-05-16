#include "glyuvwidget2.h"
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QDebug>
#include "GlobalData.h"
#include "glob.h"
//#include "widget.h"
#define ATTRIB_VERTEX 0
#define ATTRIB_TEXTURE 1
// 顶点矩阵
static const GLfloat vertexVertices[] = {
    -1.0f, -1.0f,
     1.0f, -1.0f,
     -1.0f, 1.0f,
     1.0f, 1.0f,
};
//纹理矩阵
static const GLfloat textureVertices[] = {
    0.0f,  1.0f,
    1.0f,  1.0f,
    0.0f,  0.0f,
    1.0f,  0.0f,
};

glyuvwidget::glyuvwidget(QWidget *parent):
    QOpenGLWidget(parent)
{
}

glyuvwidget::~glyuvwidget()
{
    makeCurrent();
    //vbo.destroy();
    globalData::getInstance()->g_GLList.append(id_y);//回收纹理
    globalData::getInstance()->g_GLList.append(id_u);
    globalData::getInstance()->g_GLList.append(id_v);

    m_pShaderProgram->disableAttributeArray(ATTRIB_VERTEX);
    m_pShaderProgram->disableAttributeArray(ATTRIB_TEXTURE);
    m_pShaderProgram->release();
    m_pShaderProgram->removeAllShaders();
    delete m_pShaderProgram;
    m_pShaderProgram=NULL;
    delete [] m_pBufYuv420p;
    m_pBufYuv420p=NULL;
    doneCurrent();
}

void glyuvwidget::slotShowYuv(uchar *ptr, uint width, uint height)
{
    if(NULL == ptr){
        //qDebug() << "-------PlayOneFrame inBuf==NULL";
        return;
    }
    if(NULL == m_pBufYuv420p){//如果正在运行中改变摄像头的分辨率有可能导致内存不足而奔溃
       // qDebug() << "-------new buf";
        int bufSize = 1920*1080*3/2;
        m_pBufYuv420p = new unsigned char[bufSize];
    }

    m_nVideoW = width;
    m_nVideoH = height;
    int bufSize = m_nVideoW * m_nVideoH * 3/2;
    memcpy(m_pBufYuv420p,ptr, bufSize);
    //m_pBufYuv420p = ptr;
    repaint();
}

void glyuvwidget::initializeGL()
{
    //初始化OpenGL函数
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    //创建着色器程序容器
    m_pShaderProgram = new QOpenGLShaderProgram();

    //将顶点着色器源码添加到程序容器
    m_pShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                              "attribute  vec4 vertexIn;"
                                              "attribute  vec2 textureIn;"
                                              "varying   vec2 textureOut;"
                                              "void main(void) {"
                                              "gl_Position = vertexIn;"
                                              "textureOut = textureIn;}");
    //将片段着色器添加到程序容器
    m_pShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                              "varying lowp vec2 textureOut;"
                                              "uniform  sampler2D tex_y;"
                                              "uniform  sampler2D tex_u;"
                                              "uniform  sampler2D tex_v;"
                                              "void main(void){"
                                              "mediump vec3 yuv;"
                                              "lowp  vec3 rgb;"
                                              "yuv.x = texture2D(tex_y, textureOut).r;"
                                              "yuv.y = texture2D(tex_u, textureOut).r - 0.5;"
                                              "yuv.z = texture2D(tex_v, textureOut).r - 0.5;"
                                              "rgb = mat3( 1,       1,         1,"
                                              "0,       -0.39465,  2.03211,"
                                              "1.13983, -0.58060,  0) * yuv;"
                                              "gl_FragColor = vec4(rgb, 1);}" );

    //绑定属性vertexIn到指定位置ATTRIB_VERTEX,该属性在顶点着色源码其中有声明
    m_pShaderProgram->bindAttributeLocation("vertexIn", ATTRIB_VERTEX);
    //绑定属性textureIn到指定位置ATTRIB_TEXTURE,该属性在顶点着色源码其中有声明
    m_pShaderProgram->bindAttributeLocation("textureIn", ATTRIB_TEXTURE);
    //链接所有所有添入到的着色器程序
    m_pShaderProgram->link();
    //激活所有链接
    m_pShaderProgram->bind();
    //读取着色器中的数据变量tex_y, tex_u, tex_v的位置,这些变量的声明可以在
    //片段着色器源码中可以看到图形渲染管线的第一个部分是顶点着色器(Vertex Shader)，它把一个单独的顶点作为输入。顶点着色器主要的目的是把3D坐标转为另一种3D坐标（后面会解释），同时顶点着色器允许我们对顶点属性进行一些基本处理。
    textureUniformY =  m_pShaderProgram->uniformLocation("tex_y");
    textureUniformU =  m_pShaderProgram->uniformLocation("tex_u");
    textureUniformV =  m_pShaderProgram->uniformLocation("tex_v");

    //设置属性ATTRIB_VERTEX的顶点矩阵值以及格式
    glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, vertexVertices);
    glEnableVertexAttribArray(ATTRIB_VERTEX);
    //设置属性ATTRIB_TEXTURE的纹理矩阵值以及格式
    glVertexAttribPointer(ATTRIB_TEXTURE, 2, GL_FLOAT, GL_FALSE, 0, textureVertices);
    glEnableVertexAttribArray(ATTRIB_TEXTURE);

    if(globalData::getInstance()->g_GLList.size()==0)
    {
        glGenTextures(1,&id_y); //生成纹理
        //qDebug()<< "生成纹理 "<<id_y;
    }
    else
    {
       id_y = globalData::getInstance()->g_GLList.first();
       globalData::getInstance()->g_GLList.removeFirst();
//       glDeleteTextures(1,&id_y);

    }
    glBindTexture(GL_TEXTURE_2D,id_y);//绑定纹理
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,(GLfloat)GL_LINEAR);//设置纹理参数
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,(GLfloat)GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

    if(globalData::getInstance()->g_GLList.size()==0)
    {
        glGenTextures(1,&id_u); //生成纹理
    }
    else
    {
       id_u = globalData::getInstance()->g_GLList.first();
       globalData::getInstance()->g_GLList.removeFirst();
//       glDeleteTextures(1,&id_u);
    }
    glBindTexture(GL_TEXTURE_2D, id_u);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    if(globalData::getInstance()->g_GLList.size()==0)
    {
        glGenTextures(1,&id_v); //生成纹理
    }
    else
    {
       id_v = globalData::getInstance()->g_GLList.first();
       globalData::getInstance()->g_GLList.removeFirst();
//       glDeleteTextures(1,&id_v);
    }
    glBindTexture(GL_TEXTURE_2D, id_v);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glClearColor(0.3f,0.3f,0.3f,0.0);//设置背景色
}


void glyuvwidget::resizeGL(int w, int h)
{
    // 计算窗口横纵比
    qreal aspect = qreal(w) / qreal(h ? h : 1);
    // 设置近平面值 3.0, 远平面值 7.0, 视场45度
    const qreal zNear = 3.0, zFar = 7.0, fov = 45.0;
    // 重设投影
    projection.setToIdentity();
    // 设置透视投影
    projection.perspective(fov, static_cast<float>(aspect), zNear, zFar);
}

void glyuvwidget::paintGL()
{
    if(NULL != m_pBufYuv420p)
    {
        //清理屏幕
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        //加载y数据纹理
        glActiveTexture(GL_TEXTURE0);//激活纹理单元GL_TEXTURE0
        glBindTexture(GL_TEXTURE_2D, id_y);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_nVideoW, m_nVideoH, 0, GL_RED, GL_UNSIGNED_BYTE, (char*)m_pBufYuv420p);//使用内存中m_pBufYuv420p数据创建真正的y数据纹理
        //只能用0,1,2等表示纹理单元的索引，这是opengl不人性化的地方
        //0对应纹理单元GL_TEXTURE0，1对应纹理单元GL_TEXTURE1，2对应纹理的单元GL_TEXTURE2
        glUniform1i(textureUniformY, 0); //指定y纹理要使用新值

        //加载u数据纹理
        glActiveTexture(GL_TEXTURE1);//激活纹理单元GL_TEXTURE1
        glBindTexture(GL_TEXTURE_2D, id_u);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_nVideoW/2, m_nVideoH/2, 0, GL_RED, GL_UNSIGNED_BYTE, (char*)m_pBufYuv420p+m_nVideoW*m_nVideoH);
        glUniform1i(textureUniformU, 1);//指定u纹理要使用新值

        //加载v数据纹理
        glActiveTexture(GL_TEXTURE2);//激活纹理单元GL_TEXTURE2
        glBindTexture(GL_TEXTURE_2D, id_v);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_nVideoW/2, m_nVideoH/2, 0, GL_RED, GL_UNSIGNED_BYTE, (char*)m_pBufYuv420p+m_nVideoW*m_nVideoH*5/4);
        glUniform1i(textureUniformV, 2);//指定v纹理要使用新值
        glDisable(GL_DEPTH_TEST);//should be put before glDrawArrays
        //使用顶点数组方式绘制图形
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
}




//void glyuvwidget::paintGL()
//{
//    QMatrix4x4 m;
//   m.perspective(60.0f, 4.0f/3.0f, 0.1f, 100.0f );//透视矩阵随距离的变化，图形跟着变化。屏幕平面中心就是视点（摄像头）,需要将图形移向屏幕里面一定距离。
//   m.ortho(-2,+2,-2,+2,-10,10);//近裁剪平面是一个矩形,矩形左下角点三维空间坐标是（left,bottom,-near）,右上角点是（right,top,-near）所以此处为负，表示z轴最大为10；
//                                //远裁剪平面也是一个矩形,左下角点空间坐标是（left,bottom,-far）,右上角点是（right,top,-far）所以此处为正，表示z轴最小为-10；
//                                //此时坐标中心还是在屏幕水平面中间，只是前后左右的距离已限制。
//    //清理屏幕
//    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
//    glActiveTexture(GL_TEXTURE0);  //激活纹理单元GL_TEXTURE0,系统里面的
//    glBindTexture(GL_TEXTURE_2D,idY); //绑定y分量纹理对象id到激活的纹理单元
//    //使用内存中的数据创建真正的y分量纹理数据
//    glTexImage2D(GL_TEXTURE_2D,0,GL_RED,videoW,videoH,0,GL_RED,GL_UNSIGNED_BYTE,yuvPtr);
//    //https://blog.csdn.net/xipiaoyouzi/article/details/53584798 纹理参数解析
//    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

//    glActiveTexture(GL_TEXTURE1); //激活纹理单元GL_TEXTURE1
//    glBindTexture(GL_TEXTURE_2D,idU);
//    //使用内存中的数据创建真正的u分量纹理数据
//    glTexImage2D(GL_TEXTURE_2D,0,GL_RED,videoW >> 1, videoH >> 1,0,GL_RED,GL_UNSIGNED_BYTE,yuvPtr + videoW * videoH);
//    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

//    glActiveTexture(GL_TEXTURE2); //激活纹理单元GL_TEXTURE2
//    glBindTexture(GL_TEXTURE_2D,idV);
//    //使用内存中的数据创建真正的v分量纹理数据
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, videoW >> 1, videoH >> 1, 0, GL_RED, GL_UNSIGNED_BYTE, yuvPtr+videoW*videoH*5/4);
//    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    //指定y纹理要使用新值
//    glUniform1i(textureUniformY, 0);
//    //指定u纹理要使用新值
//    glUniform1i(textureUniformU, 1);
//    //指定v纹理要使用新值
//    glUniform1i(textureUniformV, 2);
//    //使用顶点数组方式绘制图形
//    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
//}

//void glyuvwidget::initializeGL()
//{
//    initializeOpenGLFunctions();
//    glEnable(GL_DEPTH_TEST);

//    static const GLfloat vertices[]{
//        //顶点坐标
//        -1.0f,-1.0f,
//        -1.0f,+1.0f,
//        +1.0f,+1.0f,
//        +1.0f,-1.0f,
//        //纹理坐标
//        0.0f,1.0f,
//        0.0f,0.0f,
//        1.0f,0.0f,
//        1.0f,1.0f,
//    };


//    vbo.create();
//    vbo.bind();
//    vbo.allocate(vertices,sizeof(vertices));
//    //顶点着色器
//    QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex,this);
//    const char *vsrc =
//   "attribute vec4 vertexIn; \
//    attribute vec2 textureIn; \
//    varying vec2 textureOut;  \
//    void main(void)           \
//    {                         \
//        gl_Position = vertexIn; \
//        textureOut = textureIn; \
//    }";
//    vshader->compileSourceCode(vsrc);
//    //片元着色器
//    QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment,this);
//    const char *fsrc = "varying vec2 textureOut; \
//    uniform sampler2D tex_y; \
//    uniform sampler2D tex_u; \
//    uniform sampler2D tex_v; \
//    void main(void) \
//    { \
//        vec3 yuv; \
//        vec3 rgb; \
//        yuv.x = texture2D(tex_y, textureOut).r; \
//        yuv.y = texture2D(tex_u, textureOut).r - 0.5; \
//        yuv.z = texture2D(tex_v, textureOut).r - 0.5; \
//        rgb = mat3( 1,       1,         1, \
//                    0,       -0.39465,  2.03211, \
//                    1.13983, -0.58060,  0) * yuv; \
//        gl_FragColor = vec4(rgb, 1); \
//    }";
//    fshader->compileSourceCode(fsrc);

//    program = new QOpenGLShaderProgram(this);
//    program->addShader(vshader);
//    program->addShader(fshader);
//    program->bindAttributeLocation("vertexIn",VERTEXIN);
//    program->bindAttributeLocation("textureIn",TEXTUREIN);
//    program->link();
//    program->bind();
//    program->enableAttributeArray(VERTEXIN);
//    program->enableAttributeArray(TEXTUREIN);
//    program->setAttributeBuffer(VERTEXIN,GL_FLOAT,0,2,2*sizeof(GLfloat));
//    program->setAttributeBuffer(TEXTUREIN,GL_FLOAT,8*sizeof(GLfloat),2,2*sizeof(GLfloat));
//    textureUniformY = program->uniformLocation("tex_y");
//    textureUniformU = program->uniformLocation("tex_u");
//    textureUniformV = program->uniformLocation("tex_v");
//    textureY = new QOpenGLTexture(QOpenGLTexture::Target2D);
//    textureU = new QOpenGLTexture(QOpenGLTexture::Target2D);
//    textureV = new QOpenGLTexture(QOpenGLTexture::Target2D);
//    textureY->create();
//    textureU->create();
//    textureV->create();
//    idY = textureY->textureId();
//    idU = textureU->textureId();
//    idV = textureV->textureId();
//   // glClearColor(0.0,0.0,0.0,0.0);
//    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//    glClear(GL_COLOR_BUFFER_BIT);
//}
