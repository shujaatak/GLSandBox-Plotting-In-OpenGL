#ifndef GLSANDBOX_H
#define GLSANDBOX_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>

#include <QTimer>

static GLfloat const triangleVertices[] = {
    60.0f,  10.0f,  0.0f,
    110.0f, 110.0f, 0.0f,
    10.0f,  110.0f, 0.0f
};

static GLfloat const squareVertices[] = {
    80.0f, 100.0f, 0.0f,
    130.0f,  100.0f, 0.0f,
    130.0f, 10.0f, 0.0f,
    80.0f,  10.0f,  0.0f,
};


// Shader sources
static const char *vertexShaderSimple =
        "attribute highp vec4 vertex;\n"
            "uniform highp mat4 matrix;\n"
            "void main(void)\n"
            "{\n"
            "   gl_Position = matrix * vertex;\n"
            "}";
static const char *fragmentShaderSimple =
    "uniform mediump vec4 color;\n"
    "void main(void)\n"
    "{\n"
    "   gl_FragColor = color;\n"
    "}";

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class GLSandBox : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    GLSandBox(QWidget *parent = 0);
    ~GLSandBox();

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;
    QTimer *plotTimer;

public slots:
    void cleanup();
    void changeColor();

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void resizeGL(int width, int height) Q_DECL_OVERRIDE;

private:
    bool m_core;
    QOpenGLShaderProgram *m_program;
    QMatrix4x4 m_proj;
    QMatrix4x4 m_camera;
    QMatrix4x4 m_world;
    bool m_transparent;

    int vertexLocation;
    int matrixLocation;
    int colorLocation;

    int m_blue;
    QList<QVector2D> triangleCoords;
//    QVector2D triangleCoords[3];

    QTimer *timer;

};

#endif // GLSANDBOX_H
