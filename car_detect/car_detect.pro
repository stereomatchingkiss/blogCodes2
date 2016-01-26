#-------------------------------------------------
#
# Project created by QtCreator 2016-01-16T10:51:41
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = car_detect
TEMPLATE = app

include(../../pri/cv_cuda.pri)
include(../../pri/tbb.pri)
include(../../pri/tiny_cnn.pri)

INCLUDEPATH += ../..

SOURCES += main.cpp\
       mainwindow.cpp \
    object_classifier.cpp \
    ../../ocv_libs/core/resize.cpp

HEADERS  += mainwindow.hpp \
    object_classifier.hpp \
    ../../ocv_libs/core/resize.hpp

FORMS    += mainwindow.ui
