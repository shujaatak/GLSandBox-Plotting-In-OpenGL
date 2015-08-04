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
    growing=true;
    inc_poly=true;

    // Non-transformed geometry
    triangleRawCoords2D[0]=(QVector2D(0.0f, 0.0f));
    triangleRawCoords2D[1]=(QVector2D(-0.25f, -0.5f));
    triangleRawCoords2D[2]=(QVector2D(0.25f, -0.5f));

    // Color array
    colors[0]=QVector4D(1.0f,0,0,1.0f);
    colors[1]=QVector4D(0,1.0f,0,1.0f);
    colors[2]=QVector4D(0,0,1.0f,1.0f);
    colors[3]=QVector4D(1.0,1.0,1.0f,1.0f);

    // White color array
    colorWhite[0]=QVector4D(1.0f,1,1,1.0f);
    colorWhite[1]=QVector4D(0.5,0.5f,0.5f,1.0f);
    colorWhite[2]=QVector4D(0,0,0.0f,1.0f);

    rawVerts[0] = 0.0f;
    rawVerts[1] = 0.0f;
    rawVerts[2] = -0.25f;
    rawVerts[3] = -0.25f;
    rawVerts[4] = 0.25f;
    rawVerts[5] = -0.25f;

    //    polygonVerts = new QList<QVector2D>;
    polygonVerts.append(QVector2D(-0.25,0.0));
    polygonVerts.append(QVector2D(-0.25,-0.25));
    polygonVerts.append(QVector2D(0.25,-0.25));
    polygonVerts.append(QVector2D(0.25,0.0));
    vertCount = 3;


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
    vao->destroy();
    vbo->destroy();
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

    //    vertexLocation = shaderProgram->att;ributeLocation("coord2d");
    vColorLocation = shaderProgram->attributeLocation("vColor");

    projMatrixLoc = shaderProgram->uniformLocation("projMatrix");
    mvMatrixLoc = shaderProgram->uniformLocation("mvMatrix");
    // Our camera never changes in this example.
    m_camera.setToIdentity();
    m_camera.translate(0, 0, -5);
    m_world.setToIdentity();

    vao = new QOpenGLVertexArrayObject(this);
    vao->create();
    vao->bind();

    vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    vbo->create();
    vbo->setUsagePattern(QOpenGLBuffer::DynamicDraw);
    vbo->bind();
    vbo->allocate(verts,vertCount*2*sizeof(GLfloat));

    shaderProgram->enableAttributeArray("coord2d");
    shaderProgram->setAttributeBuffer("coord2d",GL_FLOAT,0,2);

    vbo2 = new QOpenGLBuffer;
    vbo2->create();
    vbo2->setUsagePattern(QOpenGLBuffer::StaticDraw);
    vbo2->bind();
    vbo2->allocate(colorVerts,sizeof(colorVerts));

    shaderProgram->enableAttributeArray("vColor");
    shaderProgram->setAttributeBuffer("vColor",GL_FLOAT,0,4);

    shaderProgram->release();

    timer->start(25);
}

void GLSandBox::paintGL()
{
    // Clear the window with current clearing color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    shaderProgram->bind();
    vao->bind();
    vbo->bind();

    //    quint8 vertsize = polygonVerts.size()*2;
    quint8 vertSize = vertCount*2;
    //    GLfloat verts[vertSize];
    for(quint8 i=0;i<vertCount;i++)
    {
        verts[i*2]=polygonVerts.at(i).x();
        verts[(i*2)+1]= polygonVerts.at(i).y();
    }

    vbo->allocate(verts,vertSize*sizeof(GLfloat));
    shaderProgram->enableAttributeArray("coord2d");
    shaderProgram->setAttributeBuffer("coord2d",GL_FLOAT,0,2);

    shaderProgram->setUniformValue(projMatrixLoc, m_proj);
    shaderProgram->setUniformValue(mvMatrixLoc, m_camera * m_world);

    glLineWidth(2.5f);
    glPointSize(4.5f);
    //    glDrawArrays(GL_POINTS,0,vertCount);
    //    glDrawArrays(GL_LINE_LOOP, 0, 3);
    //    glDrawArrays(GL_TRIANGLES,0,3);
    glDrawArrays(GL_TRIANGLE_FAN,0,vertCount);

    vbo->release();
    vao->release();
    shaderProgram->release();
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
}

void GLSandBox::changeColor()
{
    m_world.rotate(2,0,1,0);
    m_camera.rotate(1,0,0,1);
    if(growing)
    {
        polygonVerts.replace(0,QVector2D(polygonVerts.at(0).x(),
                                         polygonVerts.at(0).y()+0.05f));
        if(polygonVerts.at(0).y()>=0.9)
            growing=false;
    }
    else
    {
        polygonVerts.replace(0,QVector2D(polygonVerts.at(0).x(),
                                         polygonVerts.at(0).y()-0.05f));
        if(polygonVerts.at(0).y()<=0)
            growing=true;
    }

    if(vertCount==3)
    {
        vertCount=4;
        polygonVerts[0].setX(-0.25f);
        polygonVerts[3].setY(polygonVerts[0].y());
    }
    else
    {
        vertCount=3;
        polygonVerts[0].setX(0.0f);
    }
    update();
}
