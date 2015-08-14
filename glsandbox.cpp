#include "glsandbox.h"
#include <QOpenGLShaderProgram>
#include <QCoreApplication>

GLSandBox::GLSandBox(QWidget *parent)
    : QOpenGLWidget(parent),
      shaderProgram(0)
{
    m_core = QCoreApplication::arguments().contains(QStringLiteral("--coreprofile"));

    m_transparent = QCoreApplication::arguments().contains(QStringLiteral("--transparent"));
    if (m_transparent)
        setAttribute(Qt::WA_TranslucentBackground);

    // For anti-aliasing:
    QSurfaceFormat sFormat;
    sFormat.setSamples(8);
    this->setFormat(sFormat);

    // Control variables:
    xval = 0;
    xvalStep = 0.15;
    vertCount = ACTIVE_VERTICES;

    prepareXPos(vertCount*2);
    vertPointer = 0;


    timer = new QTimer;
    connect(timer,SIGNAL(timeout()),this,SLOT(moveCamera()));
    drawTimer = new QTimer;
    connect(timer,SIGNAL(timeout()),this,SLOT(update()));
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
    vao->destroy();
    vbo->destroy();
    delete vao;
    delete vbo;
    delete shaderProgram;
    shaderProgram = 0;
    doneCurrent();
}

void GLSandBox::initializeGL()
{
    QByteArray vShaderSource;
    QString fShaderSource;
    QFile vShader(vPath);
    QFile fShader(fPath);
    if(vShader.open(QIODevice::ReadOnly))
    {
        vShaderSource.append(vShader.readAll());
        vShader.close();
    }
    else
    {
        qDebug() << "Vertex file not found";
        exit(0);
    }
    if(fShader.open(QIODevice::ReadOnly))
    {
        fShaderSource.append(fShader.readAll());
        fShader.close();
    }
    else
    {
        qDebug() << "Fragment file not found";
        exit(0);
    }

    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLSandBox::cleanup);

    initializeOpenGLFunctions();

    // Blue background
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f );
    shaderProgram = new QOpenGLShaderProgram;
    shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex,vShaderSource);
    shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment,fShaderSource);
    shaderProgram->bind();
    shaderProgram->link();

    vColorLocation = shaderProgram->attributeLocation("vColor");

    projMatrixLoc = shaderProgram->uniformLocation("projMatrix");
    mvMatrixLoc = shaderProgram->uniformLocation("mvMatrix");

    m_camera.setToIdentity();
    m_camera.translate(0, 0, -2);
    m_world.setToIdentity();

    vao = new QOpenGLVertexArrayObject(this);
    vao->create();
    vao->bind();

    vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    vbo->create();
    vbo->setUsagePattern(QOpenGLBuffer::DynamicDraw);
    vbo->bind();
    vbo->allocate(verts,(vertCount*2)*2*sizeof(GLfloat));

    shaderProgram->enableAttributeArray("coord2d");
    shaderProgram->setAttributeBuffer("coord2d",GL_FLOAT,0,2);

    vbo->release();
    vao->release();
    shaderProgram->release();

    QMatrix4x4 tempmat;
    tempmat.setToIdentity();
    tempmat.ortho(1,2,0,2,0.01f,200.0f);
    tempmat.translate(1,1,1);
    m_proj = tempmat;

    timer->start(SINESPEED);
    drawTimer->start(ANIMSPEED);
}

void GLSandBox::paintGL()
{
    // Clear the window with current clearing color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_MULTISAMPLE);

    shaderProgram->bind();
    vao->bind();
    vbo->bind();

    vbo->allocate(verts,(vertCount*2)*2*sizeof(GLfloat));
    shaderProgram->enableAttributeArray("coord2d");
    shaderProgram->setAttributeBuffer("coord2d",GL_FLOAT,0,2);

    shaderProgram->setUniformValue(projMatrixLoc, m_proj);
    shaderProgram->setUniformValue(mvMatrixLoc, m_camera * m_world);

    glLineWidth(2.0f);
    glDrawArrays(GL_LINE_STRIP,0,(vertCount*2));

    vbo->release();
    vao->release();
    shaderProgram->release();
}

void GLSandBox::resizeGL(int w, int h)
{
    m_proj.setToIdentity();
    m_proj.ortho(1,2,0,2,0.01f,200.0f);
    m_proj.translate(1,1,1);
}

void GLSandBox::prepareXPos(quint16 vertexCount)
{
    // We will fill up the x position of each vert depending on the amount of vertices
    // This function gets the total amount of vertices required (window*2)
    GLfloat stepSize = 2/double(vertexCount-2);
    QString result;
    qDebug() << "step size: " << stepSize;

    for(quint16 i=0;i<vertexCount*2;i+=2)
    {
        verts[i] = stepSize*(i/2);
        verts[i+1] = 0.0f;
        result.append(QString("X: %1, Y: %2").arg(verts[i]).arg(verts[i+1]));
        result.append("\n");
    }
    //    qDebug() << result;
}

void GLSandBox::moveCamera()
{
    // Generate new point
    xval+= xvalStep;
    if(xval>=6.283185)
        xval=0;
    double yval = sin(xval);

    // Increase x position pointer
    vertPointer++;
    if(vertPointer>vertCount)
    {
        vertPointer=0;
    }
    //    qDebug() << "vertPointer = " << vertPointer;

    // Assign y value
    // 1. Assign it at the "front" of the queue: the current point on the right.
    // The queue can be organized as follows:
    // || [0][1][2]...[vertCount-1]||[vertCount][vertCount+1]...[vertCount*2-1]
    verts[2*(vertPointer+vertCount-1)+1] = yval;
    if(vertPointer>1)
    {
        verts[2*(vertPointer-2)+1]=yval;
    }

    // Move camera
    m_camera.setToIdentity();
    m_camera.translate(0,0,-3);
    m_camera.translate(-verts[2*vertPointer],0,0);
}
