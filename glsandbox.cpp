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
    //    QSurfaceFormat sFormat;
    //    sFormat.setSamples(8);
    //    this->setFormat(sFormat);

    // Control variables:
    xval = 0;
    xvalStep = 0.01;
    vertCount = ACTIVE_VERTICES;

    vertPointer = 0;
    camPointer=0;
    camX = 0;
    camStep = 1/double(vertCount/2-1);
    growing = true;

    prepareXPos(ACTIVE_VERTICES*2);

    printVertices();

    timer = new QTimer;
    connect(timer,SIGNAL(timeout()),this,SLOT(moveCamera()));
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

    //    vertexLocation = shaderProgram->attributeLocation("coord2d");
    vColorLocation = shaderProgram->attributeLocation("vColor");

    projMatrixLoc = shaderProgram->uniformLocation("projMatrix");
    mvMatrixLoc = shaderProgram->uniformLocation("mvMatrix");

    m_camera.setToIdentity();
    m_camera.translate(0, 0, -2);
    m_world.setToIdentity();
    //    m_world.translate(-1,0,0);

    vao = new QOpenGLVertexArrayObject(this);
    vao->create();
    vao->bind();

    vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    vbo->create();
    vbo->setUsagePattern(QOpenGLBuffer::DynamicDraw);
    vbo->bind();
    vbo->allocate(verts,(ACTIVE_VERTICES*2)*2*sizeof(GLfloat));

    shaderProgram->enableAttributeArray("coord2d");
    shaderProgram->setAttributeBuffer("coord2d",GL_FLOAT,0,2);

    vbo2 = new QOpenGLBuffer;
    vbo2->create();
    vbo2->setUsagePattern(QOpenGLBuffer::StaticDraw);
    vbo2->bind();
    vbo2->allocate(colorVerts,sizeof(colorVerts));

    shaderProgram->enableAttributeArray("vColor");
    shaderProgram->setAttributeBuffer("vColor",GL_FLOAT,0,4);

    vbo2->release();
    vbo->release();
    vao->release();
    shaderProgram->release();

    QMatrix4x4 tempmat;
    tempmat.setToIdentity();
    tempmat.ortho(0,1,0,2,0.01f,200.0f);
    tempmat.translate(1,1,1);
    //            qDebug() << "Ortho projection:" << tempmat;
    m_proj = tempmat;

    timer->start(ANIMSPEED);
}

void GLSandBox::paintGL()
{
    // Clear the window with current clearing color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_MULTISAMPLE);

    shaderProgram->bind();
    vao->bind();
    vbo->bind();

    //    quint16 vertSize = vertCount*2;
    //    double scale = 2/(double(vertCount)-1);
    //    for(quint16 i=0;i<vertCount;i++)
    //    {
    //        verts[i*2]=(scale*i-1);
    //        verts[(i*2)+1]= polygonVerts.at(i).y();
    //    }

    vbo->allocate(verts,(ACTIVE_VERTICES*2)*2*sizeof(GLfloat));
    shaderProgram->enableAttributeArray("coord2d");
    shaderProgram->setAttributeBuffer("coord2d",GL_FLOAT,0,2);

    shaderProgram->setUniformValue(projMatrixLoc, m_proj);
    shaderProgram->setUniformValue(mvMatrixLoc, m_camera * m_world);

    glLineWidth(2.5f);
    glPointSize(6.5f);

    glDrawArrays(GL_POINTS,0,(ACTIVE_VERTICES*2));
    //        glDrawArrays(GL_LINE_LOOP, 0, 4);
    //    glDrawArrays(GL_TRIANGLES,0,3);
    //        glDrawArrays(GL_TRIANGLE_FAN,0,vertCount);
    //    glDrawArrays(GL_TRIANGLE_STRIP,0,vertCount);
    //    glDrawArrays(GL_LINE_STRIP,0,vertCount);
    //        glDrawArrays(GL_POINTS,vertCount-1,1);

    vbo->release();
    vao->release();
    shaderProgram->release();
}

void GLSandBox::resizeGL(int w, int h)
{
    //    m_proj.setToIdentity();
    //    m_proj.perspective(30.0f, GLfloat(w) / h, 0.01f, 100.0f);
    //        qDebug() << "Pers projection:" << m_proj;

    //    QMatrix4x4 tempmat;
    //    tempmat.setToIdentity();
    //    tempmat.ortho(0,2,0,2,0.01f,200.0f);
    //    tempmat.translate(1,1,1);
    //        qDebug() << "Ortho projection:" << tempmat;
    m_proj.setToIdentity();
    m_proj.ortho(0,1,0,2,0.01f,200.0f);
    m_proj.translate(1,1,1);
}

void GLSandBox::printVertices()
{
    //    for(quint16 i=0;i<NO_VERTICES;i+=2)
    //    {
    //        qDebug() << verts[i+1];
    //    }
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
        verts[i] = stepSize*(i/2)-1;
        //        verts[i+1] = (double(i)/2+1)/20;
        verts[i+1] = 0.0f;
        //verts[i+1] = 0.0f;
        result.append(QString("X: %1, Y: %2").arg(verts[i]).arg(verts[i+1]));
        result.append("\n");
    }
    qDebug() << result;
}

void GLSandBox::moveCamera()
{
    // Generate new point
    xval+= xvalStep;
    if(xval>=1)
        xval=0;
    //    double yval = sin(xval);
    double yval = xval;

    // Increase x position pointer
    vertPointer++;
    if(vertPointer>ACTIVE_VERTICES)
    {
        vertPointer=0;
    }
    qDebug() << "vertPointer = " << vertPointer;

    // Assign y value
    // 1. Assign it at the "front" of the queue: the current point on the right.
    // The queue can be organized as follows:
    // || [0][1][2]...[ACTIVE_VERTICES-1]||[ACTIVE_VERTICES][ACTIVE_VERTICES+1]...[ACTIVE_VERTICES*2-1]
    verts[2*(vertPointer+ACTIVE_VERTICES-1)+1] = yval;
    if(vertPointer>1)
    {
        verts[2*(vertPointer-2)+1]=yval;
    }


//    if(vertPointer<ACTIVE_VERTICES)
//    {
//        verts[(ACTIVE_VERTICES-1)+2*vertPointer+1]=yval;
//    }


    QString output;
//    output.append("Vals: ");
    for(quint16 i=0; i<ACTIVE_VERTICES*4;i+=2)
    {
        output.append(QString("Y%1: %2| ").arg(i/2).arg(verts[i+1],6));
    }
    qDebug() << output;


    // Move camera
    m_camera.setToIdentity();
    m_camera.translate(0,0,-3);
    m_camera.translate(-verts[2*vertPointer]-1,0,0);
    //    qDebug() << "camera moved " << -verts[2*vertPointer] << " units to the left";


    update();

}

void GLSandBox::sineGenerator()
{
    xval += 0.05;
    if(xval>=6.28318)
        xval=0;
    // 1. Updating a single point:
    //    polygonVerts[1].setY(sin(xval));

    // 2. Updating a rolling point:
    //vertPointer++;
    //if(vertPointer>=vertCount)
    //{
    //    vertPointer=0;
    //}
    //polygonVerts[vertPointer].setY(sin(xval));

    // 3. Adding a point on the right:
    //    QVector2D newPoint = polygonVerts[vertCount-1];
    //    newPoint.setY(sin(xval));
    //    polygonVerts.removeFirst();
    //    polygonVerts.append(newPoint);

    verts[2*vertPointer+1]=sin(xval);
    vertPointer++;

    if(vertPointer>=vertCount)
    {
        vertPointer=0;
        camPointer=0;
    }
    else if(vertPointer>vertCount/2)
    {
        camPointer++;
    }
    //    verts[2*vertPointer+1]=sin(xval);
    m_camera.setToIdentity();
    //    m_camera.translate(-cameraTranslate[camPointer],0,-3);

    //    if(vertPointer>=vertCount)
    //    {
    //        vertPointer=0;
    //        camPointer=0;
    //        m_camera.setToIdentity();
    //        m_camera.translate(0,0,-3);
    //        m_camera.translate(-cameraTranslate[camPointer],0,0);
    //    }

    //    for(quint16 i=0;i<vertCount/2;i++)
    //    {
    //        qDebug() << verts[2*i];
    //    }
    //    qDebug() << "camera: " << m_camera;


    // 4. Adding a point on the left:
    //    QVector2D newPoint = polygonVerts[0];
    //    newPoint.setY(sin(xval));
    //    polygonVerts.removeLast();
    //    polygonVerts.insert(0,newPoint);

    update();
}
