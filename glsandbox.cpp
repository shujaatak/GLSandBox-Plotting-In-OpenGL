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

    m_blue = 0;
    triangleCoords.append(QVector2D(60.0f,  10.0f));
    triangleCoords.append(QVector2D(110.0f, 110.0f));
    triangleCoords.append(QVector2D(10.0f,  110.0f));

    timer = new QTimer;
    connect(timer,SIGNAL(timeout()),this,SLOT(changeColor()));
//    timer->start(5);

    // From SuperBible:
//    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
//        glutInitWindowSize(800, 600);
//        glutCreateWindow("Triangle");
//        glutReshapeFunc(ChangeSize);
//        glutDisplayFunc(RenderScene);

//        GLenum err = glewInit();
//        if (GLEW_OK != err) {
//            fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
//            return 1;
//            }

//        SetupRC();

//        glutMainLoop();


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
    // In this example the widget's corresponding top-level window can change
    // several times during the widget's lifetime. Whenever this happens, the
    // QOpenGLWidget's associated context is destroyed and a new one is created.
    // Therefore we have to be prepared to clean up the resources on the
    // aboutToBeDestroyed() signal, instead of the destructor. The emission of
    // the signal will be followed by an invocation of initializeGL() where we
    // can recreate all resources.
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLSandBox::cleanup);

    initializeOpenGLFunctions();

    // SetupRC
    glClearColor(0, 0, 0, m_transparent ? 0 : 1);

    m_program = new QOpenGLShaderProgram;
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSimple);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment,
        "uniform mediump vec4 color;\n"
        "void main(void)\n"
        "{\n"
        "   gl_FragColor = color;\n"
        "}");
    m_program->link();
    m_program->bind();

    vertexLocation = m_program->attributeLocation("vertex");
    matrixLocation = m_program->uniformLocation("matrix");
    colorLocation = m_program->uniformLocation("color");

//    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, m_core ? vertexShaderSourceCore : vertexShaderSource);
//    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, m_core ? fragmentShaderSourceCore : fragmentShaderSource);
//    m_program->bindAttributeLocation("vertex", 0);
//    m_program->bindAttributeLocation("normal", 1);
//    m_program->link();
//    m_program->bind();

//    m_camera.setToIdentity();
//    m_camera.translate(0, 0, -1);

    // Light position is fixed.

//    qDebug() << "OpenGL Version: " << qPrintable(*glGetString(GL_VERSION));
   }

void GLSandBox::paintGL()
{
    QVector2D triangleVerts[] =
    {
     QVector2D(60.0f,  10.0f),
     triangleCoords.at(1),
     QVector2D(10.0f,  110.0f),
    };

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QColor color(50, 50, m_blue, 255);

    QMatrix4x4 pmvMatrix;
    pmvMatrix.ortho(rect());

    m_program->setAttributeArray(vertexLocation, triangleVerts);
    m_program->enableAttributeArray(vertexLocation);
    m_program->setUniformValue(matrixLocation, pmvMatrix);
    m_program->setUniformValue(colorLocation, color);

    glDrawArrays(GL_LINE_LOOP, 0, 3);

    m_program->disableAttributeArray(vertexLocation);
}

void GLSandBox::resizeGL(int w, int h)
{

//    m_proj.setToIdentity();
//    m_proj.perspective(45.0f, GLfloat(w) / h, 0.01f, 100.0f);
}

void GLSandBox::changeColor()
{
    m_blue +=10;
    if(m_blue>=255)
        m_blue=0;

    int xcoord = triangleCoords.at(1).x();
    xcoord+=1;
    if(xcoord>=200)
        xcoord=1;
    QVector2D temp = triangleCoords.at(1);
    temp.setX(xcoord);
    triangleCoords[1] = temp;

    update();
}
