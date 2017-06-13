#-------------------------------------------------
#
# Project created by QtCreator 2016-09-14T23:18:52
#
#-------------------------------------------------

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = vibrostand
TEMPLATE = app
CONFIG += c++14

DEFINES += QCUSTOMPLOT_USE_OPENGL

SOURCES += main.cpp\
		mainwindow.cpp \
	qcustomplot.cpp\
	bearing.cpp \
	filter/filt.cpp \
	windowfilter.cpp \
	aboutwindow.cpp \
    spectrumanalyzer.cpp

win32 {
	LIBS += -lglu32 -lopengl32
}

!win32 {
	LIBS += -lglut
}

HEADERS  += mainwindow.h \
	qcustomplot.h\
	bearing.h \
	ffft/* \
	filter/filt.h \
	windowfilter.h \
	aboutwindow.h \
    spectrumanalyzer.h

FORMS    += \
	mainwindow_windows.ui \
	mainwindow96dpi.ui \
	aboutwindow.ui
