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

#define NO_VERTICES 5

static const char *vPath = "../GLSandBox/shader2d.vert";
static const char *fPath = "../GLSandBox/shader2d.frag";

static const GLfloat colorVerts[] = {
    1.0f,0.0f,0.0f,1.0f,
    0.0f,1.0f,0.0f,1.0f,
    0.0f,0.0f,1.0f,1.0f,
    1.0f,1.0f,1.0f,1.0f
};

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
    void sineGenerator();
    void moveCamera();

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void resizeGL(int width, int height) Q_DECL_OVERRIDE;

private:
    bool m_core;
    QOpenGLShaderProgram *shaderProgram;
    QOpenGLVertexArrayObject *vao;
    QOpenGLBuffer *vbo;
    QOpenGLBuffer *vbo2;

    QMatrix4x4 m_proj;
    QMatrix4x4 m_camera;
    QMatrix4x4 m_world;
    bool m_transparent;

    int vertexLocation;
    int matrixLocation;
    int projMatrixLoc;
    int mvMatrixLoc;
    int colorLocation;
    int vColorLocation;

    bool growing;

    QList<QVector2D> polygonVerts;
    GLfloat verts[NO_VERTICES];
    GLfloat cameraTranslate[NO_VERTICES];
    quint16 vertCount;
    quint16 vertPointer;
    quint16 camPointer;

    QTimer *timer;
    double xval;
    GLfloat camX;

};
#endif // GLSANDBOX_H
