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
        mainwindow.cpp \
    CameraCalibrator.cpp \
    Camera/ipcommand.cpp \
    camerawindow.cpp \
    Camera/ipcommandlist.cpp \
    Camera/saltandpeppercommand.cpp \
    saltandpepperdialog.cpp

HEADERS  += mainwindow.h \
    pch.h \
    colorhistogram.h \
    histogram.h \
    linefinder.h \
    harrisDetector.h \
    CameraCalibrator.h \
    matcher.h \
    Camera/ipcommand.h \
    camerawindow.h \
    Camera/ipcommandlist.h \
    Camera/saltandpeppercommand.h \
    saltandpepperdialog.h

FORMS    += mainwindow.ui \
    camerawindow.ui \
    saltandpepperdialog.ui

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

RESOURCES += \
    ImageResources.qrc
