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

    // Transformed geometry
    triangleCoords2D[0]=(QVector2D(200.0f, 50.0f));
    triangleCoords2D[1]=(QVector2D(100.0f, 200.0f));
    triangleCoords2D[2]=(QVector2D(300.0f, 200.0f));

    // Non-transformed geometry
    triangleRawCoords2D[0]=(QVector2D(0.0f, 0.0f));
    triangleRawCoords2D[1]=(QVector2D(-0.25f, -0.5f));
    triangleRawCoords2D[2]=(QVector2D(0.25f, -0.5f));

    // Color array
    colors[0]=QVector4D(1.0f,0,0,1.0f);
    colors[1]=QVector4D(0,1.0f,0,1.0f);
    colors[2]=QVector4D(0,0,1.0f,1.0f);

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

    // Vertex buffer object:
    m_vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
    // Setup our vertex buffer object.
    m_logoVbo.create();
    m_logoVbo.bind();
    m_logoVbo.allocate(m_logo.constData(), m_logo.count() * sizeof(GLfloat));

    vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    vbo->create();
    vbo->allocate(m_logo.constData(), m_logo.count() * sizeof(GLfloat));


    // Blue background
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f );
    shaderProgram = new QOpenGLShaderProgram;
    shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex,vShaderSource);
    shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment,fShaderSource);
    shaderProgram->bind();
    shaderProgram->link();

    vertexLocation = shaderProgram->attributeLocation("coord2d");
    vColorLocation = shaderProgram->attributeLocation("vColor");
    projMatrixLoc = shaderProgram->uniformLocation("projMatrix");
    mvMatrixLoc = shaderProgram->uniformLocation("mvMatrix");
    //    matrixLocation = shaderProgram->uniformLocation("matrix");

    // Our camera never changes in this example.
    m_camera.setToIdentity();
<<<<<<< HEAD
    m_camera.translate(0, 0, -5);
    m_world.setToIdentity();
    timer->start(25);
=======
//    qDebug() << m_camera;
    m_camera.translate(0, 0, 0);
//    qDebug() << m_camera;

        m_world.setToIdentity();
>>>>>>> origin/master
}

void GLSandBox::paintGL()
{
    // Clear the window with current clearing color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    //QColor color(0, 50, 200, 255);

    //    QMatrix4x4 pmvMatrix;
    //    pmvMatrix.ortho(rect());
    //    qDebug() << "model view Matrix: " << pmvMatrix;

<<<<<<< HEAD
    //    m_world.setToIdentity();
    //    m_world.rotate(5,1.0f,0.0f,0.0f);
=======

    m_world.rotate(1.0f,1.0f,0.0f,0.0f);
//    qDebug() << m_world;
>>>>>>> origin/master

    shaderProgram->enableAttributeArray(vertexLocation);
    shaderProgram->enableAttributeArray(vColorLocation);
    //    shaderProgram->setAttributeArray(vertexLocation, triangleCoords2D);
    shaderProgram->setAttributeArray(vertexLocation, triangleRawCoords2D);
    shaderProgram->setAttributeArray(vColorLocation, colors);
    //    shaderProgram->setUniformValue(matrixLocation, pmvMatrix);

    shaderProgram->setUniformValue(projMatrixLoc, m_proj);
    shaderProgram->setUniformValue(mvMatrixLoc, m_camera * m_world);

    //    qDebug() << "Projection matrix:" << m_proj;
    //    qDebug() << "Modelview matrix:" << m_camera*m_world;


    glLineWidth(2.5f);
<<<<<<< HEAD
    glPointSize(8);
    glDrawArrays(GL_POINTS,0,3);
    glDrawArrays(GL_LINE_LOOP, 0, 3);
    //    glDrawArrays(GL_TRIANGLES,0,3);

    //    glEnable(GL_BLEND);
    //    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //    shaderProgram->setAttributeArray(vertexLocation, squareVertices,3);
    //    color = QColor(50,100,0,100);
    //    shaderProgram->setUniformValue(colorLocation, color);
    //    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    //    glDisable(GL_BLEND);
=======
    glPointSize(5);
    glDrawArrays(GL_POINTS,0,3);
//    glDrawArrays(GL_LINE_LOOP, 0, 3);
//    glDrawArrays(GL_TRIANGLES,0,3);

//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    shaderProgram->setAttributeArray(vertexLocation, squareVertices,3);
//    color = QColor(50,100,0,100);
//    shaderProgram->setUniformValue(colorLocation, color);
//    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
//    glDisable(GL_BLEND);
>>>>>>> origin/master

    shaderProgram->disableAttributeArray(vertexLocation);
    shaderProgram->disableAttributeArray(vColorLocation);
}

void GLSandBox::resizeGL(int w, int h)
{
<<<<<<< HEAD
    m_proj.setToIdentity();
    m_proj.perspective(30.0f, GLfloat(w) / h, 0.01f, 100.0f);
    //        qDebug() << "Pers projection:" << m_proj;

    QMatrix4x4 tempmat;
    tempmat.setToIdentity();
    tempmat.ortho(0,2,0,2,0.01f,200.0f);
    tempmat.translate(1,1,1);

    //       qDebug() << "Ortho projection:" << tempmat;
    m_proj = tempmat;
=======
    qDebug() << "w: " << w << ", h: " << h;
      m_proj.setToIdentity();
//      m_proj.ortho(rect());
//      m_proj.ortho(0,w,0,h,0.01f,100.0f);
//      qDebug() << m_proj;
//      m_proj.ortho(-100,200,-100,200,0.01f,100.0f);
//       m_proj.perspective(45.0f, GLfloat(w) / h, 0.01f, 100.0f);
//       qDebug() << m_proj;
>>>>>>> origin/master
}

void GLSandBox::changeColor()
{
    m_world.rotate(1,1,0.5,0);
    m_camera.rotate(0.5,0,0,1);
    update();
}
