#ifndef GLSANDBOX_H
#define GLSANDBOX_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <QVector2D>
#include <QVector4D>
#include <QFile>
#include <QList>
#include <QOpenGLShaderProgram>
#include <QTimer>

#define ACTIVE_VERTICES 100
#define ANIMSPEED 50
#define SINESPEED 10

static const char *vPath = "../GLSandBox/shader2d.vert";
static const char *fPath = "../GLSandBox/shader2d.frag";

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
    void moveCamera();

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void resizeGL(int width, int height) Q_DECL_OVERRIDE;

private:
    void prepareXPos(quint16);
    bool m_core;
    QOpenGLShaderProgram *shaderProgram;
    QOpenGLVertexArrayObject *vao;
    QOpenGLBuffer *vbo;

    QMatrix4x4 m_proj;
    QMatrix4x4 m_camera;
    QMatrix4x4 m_world;
    bool m_transparent;

    int matrixLocation;
    int projMatrixLoc;
    int mvMatrixLoc;
    int vColorLocation;

    GLfloat verts[2000];
    quint16 vertCount;
    quint16 vertPointer;

    QTimer *timer;
    QTimer *drawTimer;
    double xval;
    double xvalStep;

};
#endif // GLSANDBOX_H
