#-------------------------------------------------
#
# Project created by QtCreator 2015-07-19T22:23:15
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GLSandBox
TEMPLATE = app

SOURCES += main.cpp\
        glsandbox.cpp

HEADERS  += glsandbox.h

DISTFILES += \
    shader2d.vert \
    shader2d.frag
