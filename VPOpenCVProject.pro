#-------------------------------------------------
#
# Project created by QtCreator 2014-02-16T20:47:31
#
#-------------------------------------------------

cache()

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VPOpenCVProject
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h \
    pch.h \
    colorhistogram.h \
    histogram.h

FORMS    += mainwindow.ui

INCLUDEPATH += "/usr/local/include"
INCLUDEPATH += "/usr/local/include/opencv"
INCLUDEPATH += "/usr/local/include/opencv2"

LIBS += -L/usr/local/lib

LIBS += -lopencv_core
LIBS += -lopencv_highgui
LIBS += -lopencv_imgproc
LIBS += -lopencv_ml
LIBS += -lopencv_video
LIBS += -lopencv_features2d
LIBS += -lopencv_calib3d
LIBS += -lopencv_objdetect
LIBS += -lopencv_contrib
LIBS += -lopencv_legacy
LIBS += -lopencv_flann
LIBS += -lopencv_nonfree

CONFIG += precompile_header

PRECOMPILED_HEADER = pch.h
