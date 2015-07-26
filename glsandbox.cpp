#include "glsandbox.h"
#include <QOpenGLShaderProgram>
#include <QCoreApplication>

GLSandBox::GLSandBox(QWidget *parent)
    : QOpenGLWidget(parent),
      m_program(0)
{
    m_core = QCoreApplication::arguments().contains(QStringLiteral("--coreprofile"));

    m_transparent = QCoreApplication::arguments().contains(QStringLiteral("--transparent"));
    if (m_transparent)
        setAttribute(Qt::WA_TranslucentBackground);

    m_blue = 200;

    triangleCoords.append(QVector2D(60.0f,  10.0f));
    triangleCoords.append(QVector2D(110.0f, 110.0f));
    triangleCoords.append(QVector2D(10.0f,  110.0f));


    // Transformed geometry
//    triangleCoords2D[0]=(QVector2D(10.0f, 10.0f));
//    triangleCoords2D[1]=(QVector2D(10.0f, 60.0f));
//    triangleCoords2D[2]=(QVector2D(60.0f, 10.0f));

    // Non-transformed geometry
    triangleCoords2D[0]=(QVector2D(0.0f, 0.0f));
    triangleCoords2D[1]=(QVector2D(-0.25f, -0.25f));
    triangleCoords2D[2]=(QVector2D(0.25f, -0.25f));

    timer = new QTimer;
    connect(timer,SIGNAL(timeout()),this,SLOT(changeColor()));
}

GLSandBox::~GLSandBox()
{
    cleanup();
}

QSize GLSandBox::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLSandBox::sizeHint() const
{
    return QSize(400, 400);
}

void GLSandBox::cleanup()
{
    makeCurrent();
    delete m_program;
    m_program = 0;
    doneCurrent();
}

void GLSandBox::initializeGL()
{
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLSandBox::cleanup);

    initializeOpenGLFunctions();

    // Blue background
    glClearColor(0.1f, 0.0f, 0.0f, 1.0f );
    m_program = new QOpenGLShaderProgram;
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex,vertex2DShader);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment,frag2DShader);
    m_program->link();
    m_program->bind();

    vertexLocation = m_program->attributeLocation("coord2d");
    matrixLocation = m_program->uniformLocation("matrix");
    colorLocation = m_program->uniformLocation("color");
    vColorLocation = m_program->uniformLocation("vColor");
}

void GLSandBox::paintGL()
{
    // Clear the window with current clearing color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    QColor color(0, 50, 200, 255);

    QMatrix4x4 pmvMatrix;
    pmvMatrix.ortho(rect());

    m_program->enableAttributeArray(vertexLocation);
    m_program->setAttributeArray(vertexLocation, triangleCoords2D);
    m_program->setUniformValue(matrixLocation, pmvMatrix);
    m_program->setUniformValue(vColorLocation, color);
    m_program->setUniformValue(colorLocation, color);

    glLineWidth(2.5f);
    glPointSize(5);
//    glDrawArrays(GL_POINTS,0,3);
//    glDrawArrays(GL_LINE_LOOP, 0, 3);
    glDrawArrays(GL_TRIANGLES,0,3);

//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    m_program->setAttributeArray(vertexLocation, squareVertices,3);
//    color = QColor(50,100,0,100);
//    m_program->setUniformValue(colorLocation, color);
//    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
//    glDisable(GL_BLEND);

    m_program->disableAttributeArray(vertexLocation);
}

void GLSandBox::resizeGL(int w, int h)
{
    //    glViewport(0,0,w,h);
    //    m_proj.setToIdentity();
    //    m_proj.perspective(45.0f, GLfloat(w) / h, 0.01f, 100.0f);
}

void GLSandBox::changeColor()
{
}
