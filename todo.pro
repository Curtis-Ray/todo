include(qtsingleapplication\\src\\qtsingleapplication.pri)
include(qtcolorpicker\\src\\qtcolorpicker.pri)

QT += core gui

TARGET = todo
TEMPLATE = app

SOURCES += main.cpp todo.cpp \
    colorwidget.cpp

HEADERS += todo.h \
    colorwidget.h

FORMS += todo.ui settings.ui

RESOURCES += icons.qrc
