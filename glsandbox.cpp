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

    m_blue = 200;

    // Non-transformed geometry
    triangleRawCoords2D[0]=(QVector2D(0.0f, 0.0f));
    triangleRawCoords2D[1]=(QVector2D(-0.25f, -0.5f));
    triangleRawCoords2D[2]=(QVector2D(0.25f, -0.5f));

    // Color array
    colors[0]=QVector4D(1.0f,0,0,1.0f);
    colors[1]=QVector4D(0,1.0f,0,1.0f);
    colors[2]=QVector4D(0,0,1.0f,1.0f);

    // White color array
    colorWhite[0]=QVector4D(1.0f,1,1,1.0f);
    colorWhite[1]=QVector4D(0.5,0.5f,0.5f,1.0f);
    colorWhite[2]=QVector4D(0,0,0.0f,1.0f);


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
//    vbo->destroy();
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
    glClearColor(0.0f, 0.0f, 0.25f, 1.0f );
    shaderProgram = new QOpenGLShaderProgram;
    shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex,vShaderSource);
    shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment,fShaderSource);
    shaderProgram->bind();
    shaderProgram->link();

    vertexLocation = shaderProgram->attributeLocation("coord2d");
    vColorLocation = shaderProgram->attributeLocation("vColor");

    projMatrixLoc = shaderProgram->uniformLocation("projMatrix");
    mvMatrixLoc = shaderProgram->uniformLocation("mvMatrix");
    // Our camera never changes in this example.
    m_camera.setToIdentity();
    m_camera.translate(0, 0, -3);
    m_world.setToIdentity();

        timer->start(20);

    //    qDebug() << m_camera;
    //    m_camera.translate(0, 0, 0);
    //    qDebug() << m_camera;

//    shaderProgram->release();
}

void GLSandBox::paintGL()
{
    // Clear the window with current clearing color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

     shaderProgram->enableAttributeArray(vertexLocation);
    shaderProgram->enableAttributeArray(vColorLocation);

    shaderProgram->setAttributeArray(vertexLocation, triangleRawCoords2D);
    shaderProgram->setAttributeArray(vColorLocation, colors);

    shaderProgram->setUniformValue(projMatrixLoc, m_proj);
    shaderProgram->setUniformValue(mvMatrixLoc, m_camera * m_world);

    glLineWidth(2.5f);
    glPointSize(8);
    glDrawArrays(GL_POINTS,0,3);
    glDrawArrays(GL_LINE_LOOP, 0, 3);

        shaderProgram->setAttributeArray(vColorLocation, colorWhite);
        glDrawArrays(GL_TRIANGLES,0,3);

    shaderProgram->disableAttributeArray(vertexLocation);
    shaderProgram->disableAttributeArray(vColorLocation);
//    shaderProgram->release();
}

void GLSandBox::resizeGL(int w, int h)
{
    m_proj.setToIdentity();
    m_proj.perspective(30.0f, GLfloat(w) / h, 0.01f, 100.0f);
    //        qDebug() << "Pers projection:" << m_proj;

    QMatrix4x4 tempmat;
    tempmat.setToIdentity();
    tempmat.ortho(0,2,0,2,0.01f,200.0f);
    tempmat.translate(1,1,1);

    //       qDebug() << "Ortho projection:" << tempmat;
    m_proj = tempmat;
    //    qDebug() << "w: " << w << ", h: " << h;
}

void GLSandBox::changeColor()
{
        m_world.rotate(2,0,1,0);
//    m_camera.rotate(0.5,0,0,1);
    update();
}
