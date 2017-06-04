#-------------------------------------------------
#
# Project created by QtCreator 2016-09-14T23:18:52
#
#-------------------------------------------------

QT       += core gui
QT += charts
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = vibrostand
TEMPLATE = app
CONFIG += c++14

DEFINES += QCUSTOMPLOT_USE_OPENGL

INCLUDEPATH += $$PWD/kfr/include
INCLUDEPATH += $$PWD/aquila-lib/include

SOURCES += main.cpp\
		mainwindow.cpp \
	qcustomplot.cpp\
	bearing.cpp \
	spectumanalyzer.cpp \
	filter/filt.cpp \
    windowfilter.cpp

LIBS += -L$$PWD/aquila-lib/lib -lAquila


HEADERS  += mainwindow.h \
	qcustomplot.h\
	bearing.h \
	ffft/* \
	spectumanalyzer.h \
	filter/filt.h \
    windowfilter.h

FORMS    += \
	mainwindow_windows.ui \
	mainwindow96dpi.ui
