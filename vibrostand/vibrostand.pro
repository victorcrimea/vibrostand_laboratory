#-------------------------------------------------
#
# Project created by QtCreator 2016-09-14T23:18:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = vibrostand
TEMPLATE = app
CONFIG += c++14

DEFINES += QCUSTOMPLOT_USE_OPENGL

SOURCES += main.cpp\
		mainwindow.cpp \
	noise.cpp \
    qcustomplot.cpp

HEADERS  += mainwindow.h \
	noise.h \
    qcustomplot.h

FORMS    += \
	mainwindow_windows.ui \
    mainwindow96dpi.ui
