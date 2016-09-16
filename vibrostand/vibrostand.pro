#-------------------------------------------------
#
# Project created by QtCreator 2016-09-14T23:18:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = vibrostand
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    noise.cpp

HEADERS  += mainwindow.h \
    noise.h

FORMS    += mainwindow.ui \
    mainwindow_windows.ui
