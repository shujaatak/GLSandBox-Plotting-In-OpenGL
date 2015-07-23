#include "glsandbox.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GLSandBox w;
    w.show();

    return a.exec();
}
